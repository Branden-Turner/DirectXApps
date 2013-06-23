/*! ====================================================================================================================
  @file memman.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.18.2011

  @brief 
    Memory management

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */


  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPOTRANT IMPORTANT IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
              
              THIS FILE CONTAINS EXTREMELY IMPORTANT AND FRAGILE CODE. THE  INTRODUCTION  OF
              BUGS IS VERY EASY. PLEASE MODIFY WITH EXTREME CARE AND EXTENSIVE TESTING AFTER
              THE MODIFICATION IS IMPLEMENTED. IF YOU DO NOT KNOW HOW THIS MEMORY MANAGEMENT
              MODEL WORKS, OR DO NOT UNDERSTAND THE CODE, FOR  THE LOVE OF GOD DO  NOT TOUCH
              THIS.

     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPOTRANT IMPORTANT IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#include "precompiled.h"

#if CONFIG_MEMORY_MANAGEMENT == 1
#define MAX_OUTPUT_STRING_WIDTH 50

  #include <new>
  #include "apputilities.h"
  #include "mempool.h"
  #include "memman.h"

  /*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */

      #undef new
      #undef malloc
      #undef calloc
      #undef free
      #undef list

      using namespace std;


  /*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */


  void* operator new(size_t size)
  {
    void* p = Grimelins::Utilities::Memory::Allocate(size);

    if (!p)
      throw std::bad_alloc();

    return p;
  }

  void operator delete(void* data)
  {
    if(!data)
      return;
    Grimelins::Utilities::Memory::Free(data);
  }

  void* operator new[](size_t size)
  {
    void* p = Grimelins::Utilities::Memory::Allocate(size);

    if (!p)
      throw std::bad_alloc();

    return p;
  }

  void operator delete[](void* data)
  {
    if(!data)
      return;
    Grimelins::Utilities::Memory::Free(data);
  }



  namespace Grimelins
  {
    namespace Utilities
    {
      namespace Memory
      {


        Statistics::Statistics(void)
        { 
          memset(this, 0, sizeof(Statistics));
        }

        Statistics::~Statistics(void)
        {
          if (TypeStatistics)
            TypeStatistics->~list();
        }

        Statistics::Statistics(Statistics const& rhs) 
        { 
          memset(this, 0, sizeof(Statistics));
          copy(rhs);
        }

        Statistics& Statistics::operator=(Statistics const& rhs)
        {
          copy(rhs);
          return *this;
        }

        void Statistics::reset(void) 
        { 
          if (TypeStatistics)
          {
            TypeStatistics->~list();
            free(TypeStatistics);
          }

          memset(this, 0, sizeof(Statistics));
        }

        void Statistics::copy(Statistics const& rhs)
        {
          if (TypeStatistics)
          {
            TypeStatistics->~list();
            free(TypeStatistics);
          }

          memcpy(this, &rhs, sizeof(Statistics));

          // Just copied rhs's pointer to dynamically allocated TypeStatistics
          if (TypeStatistics)
          {
            list<TypeInformation>* temp = SCAST(list<TypeInformation>*, malloc(sizeof(list<TypeInformation>)));
            new (temp) list<TypeInformation>;
            *temp = *TypeStatistics;
            TypeStatistics = temp;
          }
        }

        void Statistics::AllocateTypeStatistics(void)
        {
          if (TypeStatistics)
            return;

          TypeStatistics = SCAST(list<TypeInformation>*, malloc(sizeof(list<TypeInformation>)));
          new (TypeStatistics) list<TypeInformation>;
        }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Hidden helpers and variables

        // Don't use custom types or defines here, best to not touch if you don't know what you're doing


        static u1         sInitialized = false;
        static u32        sCapacity = 0;
        static Pool**     sPools = NULL;
        static ch**       sUnPooledMem = NULL;
        static u32        sUnPooledMemCapacity = 0;
        static const u16  sMinPoolCollection = Pool::MIN_OBJ_SIZE - 1;
        static u1         sLeaksDetected = false;

    #if CONFIG_MEMORY_STATISTICS
        static Statistics sStatistics;
    #endif


        /// <summary>
        ///   Grows the shared pool space to the desired size.
        /// </summary>
        ///
        /// <param name="size">Requested pool space.</param>
        ///
        /// <returns>void</returns>
        static void GrowTo(u32 size)
        {
          // Allocate new pool array
          Pool** newPools = SCAST(Pool**, calloc(size, sizeof(Pool*)));

          // Copy pool pointers from old array to new one
          for (u32 i = 0; i < sCapacity; ++i)
            newPools[i] = sPools[i];


          if (sPools)
            free(sPools);

          sPools = newPools;
          sCapacity = size;

    #if CONFIG_MEMORY_STATISTICS
          ++sStatistics.NumPoolSpaceGrows;
    #endif
        }

        /// <summary>
        ///   Allocates a pool of requested obj size.
        /// </summary>
        ///
        /// <param name="size">Size of objects to be pooled</param>
        ///
        /// <returns>The newly allocated pool</returns>
        static Pool* MakePool(size_t size)
        {
    #if CONFIG_MEMORY_STATISTICS
          ++sStatistics.NumberPools;
    #endif
          // Create new memory pool for this size

    #if VIRTUS_CONFIG_DEBUG_MODE || VIRTUS_CONFIG_PROFILE_MODE
          PoolCfg cfg(false, CONFIG_MEMORY_OBJECTS_PER_PAGE, CONFIG_MEMORY_OBJECTS_PER_PAGE, true, CONFIG_MEMORY_PAD_BYTES, CONFIG_MEMORY_USE_HEADER_BLOCKS, 
            CONFIG_MEMORY_ALIGNMENT, sizeof(MemoryStamp));
    #else
          PoolCfg cfg(false, CONFIG_MEMORY_OBJECTS_PER_PAGE, CONFIG_MEMORY_OBJECTS_PER_PAGE, false, 0, 1, 
            CONFIG_MEMORY_ALIGNMENT, 0);
    #endif

          Pool* pool = SCAST(Pool*, malloc(sizeof(Pool))); // Allocate memory for pool
          new (pool) Pool(size, cfg);                      // Construct pool in memory

          return pool;
        }

        /// <summary>
        ///   Finds the pool an object of given size belongs to. If no pool
        ///   is available one will be created.
        /// </summary>
        ///
        /// <param name="size">Size of the object</param>
        ///
        /// <returns>Retrieved / Created pool</returns>
        static Pool* GetPool(size_t size)
        {
          if (size <= Pool::MIN_OBJ_SIZE)
          {
            if (Pool::MIN_OBJ_SIZE > sCapacity)
              GrowTo(Pool::MIN_OBJ_SIZE);

            // Object resides in special shared pool for small objects
            if (!sPools[sMinPoolCollection])
              sPools[sMinPoolCollection] = MakePool(Pool::MIN_OBJ_SIZE);

            return sPools[sMinPoolCollection];
          }
          else
          {
            if (size > sCapacity)
              GrowTo(size);

            if (!sPools[size - 1])
              sPools[size - 1] = MakePool(size);

            return sPools[size - 1];
          }
        }

        /// <summary>
        ///   Finds the memory pool a given block belongs to.
        /// </summary>
        ///
        /// <param name="data">The memory block to match.</param>
        ///
        /// <returns>The found memory pool. If not pool was found function returns NULL.</returns>
        static Pool* FindMemoryPool(void* data)
        {
    #if CONFIG_MEMORY_STATISTICS
          ++sStatistics.NumPoolLookups;
    #endif

          for (u32 i = 0; i < sCapacity; ++i)
            if (sPools[i] && sPools[i]->IsBlockInPool(data))
              return sPools[i];

    #if CONFIG_MEMORY_STATISTICS
          ++sStatistics.NumFailedPoolLookups;
    #endif

          return NULL;
        }

        /// <summary>
        ///   Cleans up traces of memory management and allocated pools.
        ///   Called automatically by atexit()
        /// </summary>
        /// 
        /// <remarks>
        ///   Memory leaks are not dumped by default.
        /// </remarks>
        static void Uninitialize(void)
        {
          if (CONFIG_MEMORY_DUMP_ON_EXIT)
          {
            DumpMemory(true);

            ErrorIf(sLeaksDetected, "Memory Leaks Detected!\n\n Check Output window for trace.");
          }

#if CONFIG_MEMORY_STATISTICS
          sStatistics.reset();
#endif

          for (u32 i = 0; i < sCapacity; ++i)
          {
            if (sPools[i])
            {
              sPools[i]->~Pool();
              free(sPools[i]);
            }
          }

          free(sPools);
          sPools = NULL;
          sCapacity = 0;
        }

        /// <summary>
        ///   Initializes memory management by preparing the pool space.
        /// </summary>
        static int Initialize(void)
        {
           ErrorIf(sInitialized, "Multiple memory manager intializations received.");

          if (sInitialized)
            return 1;

          sInitialized = true;
          atexit(Uninitialize);

          if (CONFIG_MEMORY_NUM_POOLS != 0) 
            GrowTo(CONFIG_MEMORY_NUM_POOLS);

          return 1;
        }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public Functions

        void* AllocateUnpooledBlock(size_t size)
        {
#if CONFIG_MEMORY_DIAGNOSTICS
            size += sizeof(MemoryStamp) + sizeof(s32);
#endif
          char* block;

          if (CONFIG_MEMORY_ALIGNMENT)
            block = (char*) Application::aligned_malloc(size, CONFIG_MEMORY_ALIGNMENT);
          else
             block = (char*) malloc(size);

#if CONFIG_MEMORY_DIAGNOSTICS
            s32* sizeStore = (s32*)(block);
            *sizeStore = size;
            return block + sizeof(MemoryStamp) + sizeof(s32);
#endif

          return block;
        }

        void FreeUnpooledBlock(char* dat)
        {
#if CONFIG_MEMORY_DIAGNOSTICS
            dat -= (sizeof(MemoryStamp) + sizeof(s32));
#endif

          if (CONFIG_MEMORY_ALIGNMENT)
            Application::aligned_free(dat);
          else
            free(dat);
        }

        void* AllocateUnpooled(size_t size)
        {
          if (sUnPooledMemCapacity == 0)
          {
            sUnPooledMem = (char**)malloc(CONFIG_MEMORY_UNPOOLED_CAPACITY * sizeof(char*));
            std::memset(sUnPooledMem, 0, CONFIG_MEMORY_UNPOOLED_CAPACITY * sizeof(char*));
            sUnPooledMemCapacity = CONFIG_MEMORY_UNPOOLED_CAPACITY;
          }

          for (u32 i = 0; i < sUnPooledMemCapacity; ++i)
          {
            if (sUnPooledMem[i] == NULL)
            {
              sUnPooledMem[i] = (char*)AllocateUnpooledBlock(size);
              return sUnPooledMem[i];
            }
          }

          // Couldn't find spot, need to grow table
#if CONFIG_MEMORY_STATISTICS
          ++sStatistics.NumUnpooledTableGrows;
#endif

          int newCap = sUnPooledMemCapacity * CONFIG_MEMORY_UNPOOLED_GROW_FACTOR;
          char** newTable = (ch**)malloc(newCap * sizeof(char*));

          
          std::memcpy(newTable, sUnPooledMem, sUnPooledMemCapacity * sizeof(char*));
          std::memset(newTable + sUnPooledMemCapacity, 0, (newCap - sUnPooledMemCapacity) * sizeof(char*));

          free(sUnPooledMem);
          sUnPooledMem = newTable;

          sUnPooledMem[newCap] = (char*)AllocateUnpooledBlock(size);
          char* ret = sUnPooledMem[newCap];

          sUnPooledMemCapacity = newCap;
          return ret;
        }

        u1 FreeUnpooled(char* data)
        {
          char* lastFree = 0;

          for (u32 i = 0; i < sUnPooledMemCapacity; ++i)
          {
            if (sUnPooledMem[i] == data)
            {
              FreeUnpooledBlock(data);
              sUnPooledMem[i] = NULL;
              return true;
            }
            else if (!lastFree && sUnPooledMem[i] == 0)
              lastFree = sUnPooledMem[i];
            else if (lastFree != NULL)
            {
              lastFree = sUnPooledMem[i];
              sUnPooledMem[i] = 0;
              lastFree = NULL;
            }
          }

          return false;
        }

        /// <summary>
        ///   Allocates the requested amount of memory.
        /// </summary>
        ///
        /// <param name="size">Size of the requested memory block</param>
        ///
        /// <returns>Pointer to the requested memory block.</returns>
        void* Allocate(size_t size)
        {
          if (!sInitialized)
            Initialize();

    #if CONFIG_MEMORY_STATISTICS
          sStatistics.BlockMemoryInUse += size;
          ++sStatistics.TotalAllocations;
          ++sStatistics.AllocatedBlocks;

          // New maximum?
          if (sStatistics.AllocatedBlocks > sStatistics.AllocatedBlocksMaximum)
            sStatistics.AllocatedBlocksMaximum = sStatistics.AllocatedBlocks;
    #endif

            // It might be desirable to not pool objects of large size:
          if (CONFIG_MEMORY_MAX_POOLED_SIZE && size > CONFIG_MEMORY_MAX_POOLED_SIZE)
            return AllocateUnpooled(size);

          try
          {
            return GetPool(size)->Allocate();
          }
          catch (MemoryException& e)
          {
            ErrorIf("Memory Error", e.what());
            throw(Exception(E_MEM_ERROR, e.what()));
          }
          catch (std::bad_alloc& e)
          {
            ErrorIf("Memory Error", "Construction of Memory Pool failed");
            throw(Exception(E_MEM_ERROR, e.what()));
          }
        }

        /// <summary>
        ///   Deallocates a previously allocated memory block.
        /// </summary>
        ///
        /// <param name="data">The previously allocated memory block.</param>
        ///
        /// <returns>void</returns>
        void Free(void* data)
        {
          if (!sInitialized)
            throw Exception(E_MEM_NOT_INITIALZED, "Attempt to deallocate after memory manager went offline.");

          Pool* pool = FindMemoryPool(data);

          if (!pool)
          {
            bool res = FreeUnpooled((char*)data);

            if (res)
              return;

            throw Exception(E_MEM_ERROR, "Attempt to deallocate memory that belongs to unknown pool.");
          }
          

          try 
          {
    #if CONFIG_MEMORY_STATISTICS
            sStatistics.BlockMemoryInUse -= pool->GetStatistics().m_ObjectSize;
            ++sStatistics.TotalDeallocations;
            --sStatistics.AllocatedBlocks;
    #endif
            pool->Free(data);
          }
          catch (MemoryException& e)
          {
            ErrorIf("Memory Error", e.what());
            throw(Exception(E_MEM_ERROR, e.what()));
          }
        }

        void Defragment(void)
        {
    #if CONFIG_MEMORY_STATISTICS
          ++sStatistics.DefragmentationRuns;
    #endif
        }


        namespace
        {
          /// <summary>
          ///   Callback function for pools. Dumps the respective memory block to
          ///   debug output console with debugging information.
          /// </summary>
          ///
          /// <param name="block">The block dumped</param>
          /// <param name="actual_size">Size of block</param>
          /// <param name="cookie">Whether to print addresses or not</param>
          ///
          /// <returns>void</returns>
          void BlockDump(const void* block, unsigned int actual_size, void* cookie)
          {
            sLeaksDetected = true;
            bool* showaddr = SCAST(bool*, cookie);

            u32 size = actual_size;
            // limit to 16 bytes
            if (actual_size > 16)
              size = 16;

            unsigned char* data = CCAST(unsigned char*, RCAST(const unsigned char*, block));
    #if CONFIG_MEMORY_DIAGNOSTICS
            unsigned char* userbyte = RCAST(unsigned char*, data - CONFIG_MEMORY_PAD_BYTES - sizeof(MemoryStamp));

            MemoryStamp* stamp = NULL;

            if (*userbyte != Pool::USERSTAMP_PATTERN)
              stamp = RCAST(MemoryStamp*, userbyte);
            if (stamp)
            {
              printDebug("%s (%i): \n", stamp->File, stamp->Line);

              char type[MAX_OUTPUT_STRING_WIDTH + 1] = {0};
              memcpy(type, stamp->Type, MAX_OUTPUT_STRING_WIDTH);

              if (*showaddr)
                printDebug("\tBlock at 0x%p, %i bytes long (Type: %s). {%i}\n", block, actual_size, type, stamp->AllocNumber);
              else
                printDebug("\tBlock at 0x00000000, %i bytes long (Type: %s). {%i}\n", actual_size, type, stamp->AllocNumber);
            }
            else
    #endif
            {
              printDebug("Unknown source:\n");

              if (*showaddr)
                printDebug("\tBlock at 0x%p, %i bytes long.\n", block, actual_size);
              else
                printDebug("\tBlock at 0x00000000, %i bytes long.\n", actual_size);
            }

            // If we were passed a NULL pointer, do nothing
            if (!block)
              return;

            printDebug("\t Data: <");
            for (unsigned int i = 0; i < size; i++)
            {
              unsigned char c = *data++;
              if (c > 31 && c < 128) // printable range
                printDebug("%c", c);
              else
                printDebug(" ");
            }
            printDebug(">");

            data = CCAST(unsigned char*, RCAST(const unsigned char*, block));

            for (unsigned int i = 0; i < size; i++)
              printDebug("\t %02X", (int)*data++);
            printDebug("\n");
          }

    #if CONFIG_MEMORY_DIAGNOSTICS
          /// <summary>
          ///		Dumps memory blocks into given MemoryStatistics struct.
          /// </summary>
          ///
          /// <param name="data">The memory block to dump</param>
          /// <param name="size">Size of memory block</param>
          /// <param name="cookie">The MemoryStatistics structure</param>
          ///
          /// <returns>void</returns>
          void StatsBlockDump(const void* data, unsigned int size, void* cookie)
          {
            Statistics* stats = RCAST(Statistics*, cookie);

            char* block = SCAST(char*, CCAST(void*, data));

            unsigned char* userbyte = RCAST(unsigned char*, block - CONFIG_MEMORY_PAD_BYTES - sizeof(MemoryStamp));

            if (*userbyte == Pool::USERSTAMP_PATTERN)
              return;

            MemoryStamp* stamp = RCAST(MemoryStamp*, userbyte);


            list<TypeInformation>::iterator type 
              = std::find(stats->TypeStatistics->begin(), stats->TypeStatistics->end(), ::string(stamp->Type));

            if (type == stats->TypeStatistics->end())
            {
              // Found new type information
              TypeInformation typInf;
              typInf.TypeMemoryContribution = 0; // Updated outside of conditional
              typInf.TypeName = stamp->Type;
              typInf.TypeSize = size;

              stats->TypeStatistics->push_back(typInf);
              type = --(stats->TypeStatistics->end());
            }

            // Update statistics for type
            type->TypeMemoryContribution += size;        
          }
    #endif
        }


        /// <summary>
        ///		Dumps all memory currently in use with debug information into the
        ///   set debug output console.
        /// </summary>
        ///
        /// <param name="showaddress">Whether or not to show the address of blocks</param>
        ///
        /// <returns>void</returns>
        void DumpMemory(bool showaddress)
        {
          printDebug("Dumping objects ->\n");
          u32 blocks = 0;
          
          for (u32 i = 0; i < sCapacity; ++i)
            if (sPools[i])
              blocks += sPools[i]->DumpMemoryInUse(BlockDump, &showaddress);

          for (u32 i = 0; i < sUnPooledMemCapacity; ++i)
          {
            if (sUnPooledMem[i])
            {
              ++blocks;
              sLeaksDetected = true;             

              
              ch* data = sUnPooledMem[i];

#if (CONFIG_MEMORY_DIAGNOSTICS)

              u32 actual_size = (u32)*(s32*)(sUnPooledMem[i] - sizeof(MemoryStamp) - sizeof(s32));
              u32 size = (actual_size > 16) ? 16 : actual_size;

                MemoryStamp* stamp = RCAST(MemoryStamp*, data - sizeof(MemoryStamp));;

                printDebug("%s (%i): \n", stamp->File, stamp->Line);

                char type[MAX_OUTPUT_STRING_WIDTH + 1] = {0};
                memcpy(type, stamp->Type, MAX_OUTPUT_STRING_WIDTH);

                if (showaddress)
                  printDebug("\tUnpooled Block at 0x%p, %i bytes long (Type: %s). {%i}\n", data, actual_size, type, stamp->AllocNumber);
                else
                  printDebug("\tUnpooled Block at 0x00000000, %i bytes long (Type: %s). {%i}\n", actual_size, type, stamp->AllocNumber);

                printDebug("\t Data: <");
                for (unsigned int i = 0; i < size; i++)
                {
                  unsigned char c = *data++;
                  if (c > 31 && c < 128) // printable range
                    printDebug("%c", c);
                  else
                    printDebug(" ");
                }
                printDebug(">");

                for (unsigned int i = 0; i < size; i++)
                  printDebug("\t %02X", (int)*data++);
                printDebug("\n");
#else
                printDebug("Unknown source:\n");

                if (showaddress)
                  printDebug("\tUnpooled Block at 0x%p, ? bytes long.\n", data);
                else
                  printDebug("\tUnpooled Block at 0x00000000, ? bytes long.\n");
#endif

            }

          }
          printDebug("Object dump complete. [%i]\n", blocks);
        }

        
        /// <summary>
        ///		Dumps statistical information into the selected debug console.
        /// </summary>
        ///
        /// <param name="typeInformation">Whether or not to print type statistics.</param>
        ///
        /// <returns>void</returns>
        void DumpStatistics(bool typeInformation)
        {
          printDebug("\n===============================================================\n");
          printDebug("                          MEMORY STATISTICS                      \n");
          printDebug("\n");

          if (!CONFIG_MEMORY_STATISTICS)
            printDebug("WARNING: MEMORY_STATISTICS IS SWITHCED OFF. RESULTS WILL BE INACCURATE.");

          Statistics stats = GetStatistics();

          printDebug("Total memory in use:                        %i bytes\n", stats.TotalMemoryInUse);
          printDebug("Raw client memory in use (block memory):    %i\n", stats.BlockMemoryInUse);
          printDebug("Memory fragmentation:                       %f\n", stats.Fragmentation);
          printDebug("Number of memory pools:                     %i\n", stats.NumberPools);
          printDebug("Total number of allocations:                %i\n", stats.TotalAllocations);
          printDebug("Total number of deallocations:              %i\n", stats.TotalDeallocations);
          printDebug("Total pages in use:                         %i\n", stats.TotalPagesInUse);
          printDebug("Number of allocated blocks:                 %i\n", stats.AllocatedBlocks);
          printDebug("Number of free blocks:                      %i\n", stats.FreeBlocks);
          printDebug("Most blocks ever allocated:                 %i\n", stats.AllocatedBlocksMaximum);
          printDebug("Number of defragmentation runs:             %i\n", stats.DefragmentationRuns);
          printDebug("Number of validation runs:                  %i\n", stats.ValidationRuns);
          printDebug("Number of pool space enlargments:           %i\n", stats.NumPoolSpaceGrows);
          printDebug("Number of pool lookups:                     %i\n", stats.NumPoolLookups);
          printDebug("Number of failed pool lookups:              %i\n", stats.NumFailedPoolLookups);
          printDebug("Numbe of unpooled lookup table gows:        %i\n", stats.NumUnpooledTableGrows);

          if (typeInformation)
          {
            printDebug("\n\n");
            printDebug("    ---------------\n");
            printDebug("    TYPE BREAKDOWN:\n");
            printDebug("    ---------------\n\n");
            
            list<TypeInformation>::iterator it = stats.TypeStatistics->begin();
            list<TypeInformation>::iterator endIt = stats.TypeStatistics->end();


            printDebug("%*s\t%-15s\t%-20s\n", MAX_OUTPUT_STRING_WIDTH, "Typename", "Size (bytes)", "Memory Contribution (percent)");

            char buffer[MAX_OUTPUT_STRING_WIDTH + 1] = {0};

            for (; it != endIt; ++it)
            {
              memcpy(buffer, it->TypeName, MAX_OUTPUT_STRING_WIDTH);
              printDebug("%*s\t%-15i\t%-f\n", MAX_OUTPUT_STRING_WIDTH, buffer, it->TypeSize, it->TypeMemoryContribution * 100.f);
            }
          }

          printDebug("\n===============================================================\n");
        }


        Statistics GetStatistics(void)
        {
          ErrorIf(!CONFIG_MEMORY_STATISTICS, "Memory Statistics are turned off, results will be inaccurate.");

    #if CONFIG_MEMORY_STATISTICS
          ErrorIf(!sInitialized, "Can not gather statistics, memory management uninitialized");

          if (!sInitialized)
            return sStatistics;
            
          Statistics stats(sStatistics);

          for (u32 i = 0; i < sCapacity; ++i)
          {
            if (sPools[i])
            {
              PoolStats s = sPools[i]->GetStatistics();
              
              stats.TotalMemoryInUse += s.m_MemoryPageSize * s.m_NumMemoryPages;
              stats.TotalPagesInUse += s.m_NumMemoryPages;
              stats.FreeBlocks += s.m_NumFreeBlocks;
            }
          }

          stats.AllocateTypeStatistics();

          for (u32 i = 0; i < sCapacity; ++i)
            if(sPools[i])
              sPools[i]->DumpMemoryInUse(StatsBlockDump, &stats);


          list<TypeInformation>::iterator it = stats.TypeStatistics->begin();
          list<TypeInformation>::iterator endIt = stats.TypeStatistics->end();

          if (stats.BlockMemoryInUse)
          {
            for (; it != endIt; ++it)
              it->TypeMemoryContribution /= SCAST(f32, stats.BlockMemoryInUse);
          }

          if (stats.BlockMemoryInUse)
            stats.Fragmentation = SCAST(f32, stats.BlockMemoryInUse) / SCAST(f32, stats.FreeBlocks);

          return stats;
    #endif
          return Statistics();
        }


        void* __BYPASSMALLOC(size_t size)
        {
          return malloc(size);
        }
        
        void __BYPASSFREE(void* ptr)
        {
          free(ptr);
        }

    #if CONFIG_MEMORY_DIAGNOSTICS

        namespace
        {
          u32 suAllocationNumber = 0;
          u32 suAllocationBreakp = 0;
        }

        /// <summary>
        ///   Stamps a memory block's userbytes with a trace (point of allocation origin tracking)
        /// </summary>
        ///
        /// <remarks>
        ///   Function only available in debug mode
        /// </remarks>
        ///
        /// <param name="data">The memory block to stamp</param>
        /// <param name="tr">The point of origin information (source file and line)</param>
        /// <param name="typeName">Name of the type allocated</param>
        ///
        /// <returns>void</returns>
        void Stamp(void* data, Trace& tr, char const* typeName)
        {
          ErrorIf(!sInitialized, "Attempt to deallocate memory after manager went offline.");


          if (++suAllocationNumber == suAllocationBreakp)
            Break();

          Pool* pool = FindMemoryPool(data);

          if (pool)
          {
            try
            {
              MemoryStamp stamp(typeName, tr.File, tr.Line, suAllocationNumber);
              pool->Stamp(data, RCAST(void*, &stamp), sizeof(MemoryStamp));
            }
            catch (MemoryException& e)
            {
              ErrorIf("Memory Error", e.what());
              throw(Exception(E_MEM_ERROR, e.what()));
            }
          }
          else
          {
            // Object wasn't pooled.
            for (u32 i = 0; i < sUnPooledMemCapacity; ++i)
            {
              if(sUnPooledMem[i] == data)
              {
                // Found it
                MemoryStamp stamp(typeName, tr.File, tr.Line, suAllocationNumber);
                std::memcpy(sUnPooledMem[i] - sizeof(MemoryStamp), (void*)&stamp, sizeof(MemoryStamp));
                break;
              }

            }
          }
        }

        void SetAllocationBreakPoint(u32 allocation_number)
        {
          ErrorIf(suAllocationBreakp != 0, "You are overwriting a previous memory breakpoint (%i) with (%i)."
            "This is a warning, you may ignore this.", suAllocationBreakp, allocation_number);
          suAllocationBreakp = allocation_number;
        }

    #endif


      } // namespace Memory
    } // namespace Utilities
  } // namespace virtus


#elif CONFIG_MEMORY_MANAGEMENT == 0

namespace Grimelins
{
  namespace Utilities
  {
    namespace Memory
    {
      void* __BYPASSMALLOC(size_t size)
      {
        return malloc(size);
      }

      void __BYPASSFREE(void* ptr)
      {
        free(ptr);
      }
    }
  }
}

#endif // #if MEMORY_MANAGEMENT