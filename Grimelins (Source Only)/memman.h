/*! ====================================================================================================================
  @file memman.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.18.2011

  @brief 
    Project wide memory management.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
  
  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPOTRANT IMPORTANT IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
              
              THIS FILE CONTAINS EXTREMELY IMPORTANT AND FRAGILE CODE. THE  INTRODUCTION  OF
              BUGS IS VERY EASY. PLEASE MODIFY WITH EXTREME CARE AND EXTENSIVE TESTING AFTER
              THE MODIFICATION IS IMPLEMENTED. IF YOU DO NOT KNOW HOW THIS MEMORY MANAGEMENT
              MODEL WORKS, OR DO NOT UNDERSTAND THE CODE, FOR  THE LOVE OF GOD DO  NOT TOUCH
              THIS.

     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPOTRANT IMPORTANT IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


#ifndef VIRTUS_MEMMAN_H
#define VIRTUS_MEMMAN_H

#include "optional.h"

  //TODO: Finish statistic (fragmentation & percentages don't add up to 100%)
  //TODO: Change sPools from array to linked list that is sorted (less wasted space)
  //TODO: Compute fragmentation accurately
  //TODO: Write defragment() method

#ifndef CONFIG_MEMORY_MANAGEMENT
# define CONFIG_MEMORY_MANAGEMENT 0
#endif

#ifndef CONFIG_MEMORY_STATISTICS
# define CONFIG_MEMORY_STATISTICS 0
#endif

#if CONFIG_MEMORY_MANAGEMENT == 1


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // In release mode the memory manager has the same behavior as in debug mode, but without additional statistics upkeep
  // like trace information, pad checking and other debug diagnostics

  #undef new
  #undef malloc
  #undef free
  #undef VIRTUS_NEW
  #undef VIRTUS_FREE
  #undef VIRTUS_MALLOC

  // Global operator overloads
  void* operator new(size_t size);
  void operator delete(void* data);
  void* operator new[](size_t size);
  void operator delete[](void* data);

  namespace Grimelins
  {
    namespace Utilities
    {
      namespace Memory
      {

        struct TypeInformation
        {
          const char* TypeName;         /// Name of the type
          f32 TypeMemoryContribution;   /// Percentage of total memory use for this type
          u32 TypeSize;                 /// Size of the type

          bool operator==(string const& rhs)
          {
            return strcmp(TypeName, rhs.c_str()) == 0;
          }
        };

        struct Statistics
        {
          u32 TotalMemoryInUse;             /// Number of bytes allocated
          u32 BlockMemoryInUse;             /// Number of bytes allocated (excl. padding, headerblocks, userbytes, etc.)
          f32 Fragmentation;                /// Fragmentation percentage
          u32 NumberPools;                  /// Number of memory pools in use

          u32 TotalAllocations;             /// Total number of allocations since initialization
          u32 TotalDeallocations;           /// Total number of deallocations since initialization

          u32 TotalPagesInUse;              /// Total number of memory pages in use.
          u32 FreeBlocks;                   /// Number of free blocks that exist in pools.
          u32 AllocatedBlocks;              /// Number of allocated blocks that exist in pools.

          u32 AllocatedBlocksMaximum;       /// Maximum number of blocks allocated since initialization

          u32 DefragmentationRuns;          /// Number of times the memory was defragmented
          u32 ValidationRuns;               /// Number of times memory was checked for corruption

          u32 NumPoolSpaceGrows;            /// Number of times the pool space had to be grown
          u32 NumPoolLookups;               /// Number of times a block was matched against a pool
          u32 NumFailedPoolLookups;         /// Number of times a block was not found in any pool

          u32 NumUnpooledTableGrows;        /// Number of times the unpooled memory tracking table grew

          list<TypeInformation>* TypeStatistics;      /// List of types with respective information

          Statistics(void);
          virtual ~Statistics(void);
          Statistics(Statistics const& rhs);
          Statistics& operator=(Statistics const& rhs);
          void AllocateTypeStatistics(void);
          void reset(void);

        private:
          void copy(Statistics const& rhs);
        };

        void* Allocate(size_t size);
        void Free(void* data);
        void Defragment(void);
        void DumpMemory(bool showaddress = false);
        void DumpStatistics(bool typeInformation = false);
        Statistics GetStatistics(void);
        void* __BYPASSMALLOC(size_t size);
        void __BYPASSFREE(void* ptr);

      }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // If in Debug Mode: Each memory block will be stamped with a trace (point of allocation origin)
    // In release the memory simply will be allocated using the overloaded operators


    #if CONFIG_MEMORY_DIAGNOSTICS
    #define MALLOC_TYPE_NAME "_mlcRawMem"

      namespace Utilities
      {
        namespace Memory
        {
          /// Container for point of origin tracking
          struct Trace
          {
            s8 const* File;
            s32 Line;

            Trace(s8 const* file, s32 line) : File(file), Line(line) {}
          };

          /// Actual brand that is stamped right in front of data block
          struct MemoryStamp
          {
            s8 const* Type;
            s8 const* File;
            s32 Line;
            u32 AllocNumber;

            MemoryStamp(s8 const* type, s8 const* file, s32 line, u32 alcn) : Type(type), File(file), Line(line), AllocNumber(alcn) {}
          };


          void Stamp(void* data, Trace& tr, char const* typeName);
          void SetAllocationBreakPoint(u32 allocation_number);
        }
      }


      // Operator stamps the trace onto the memory
      template <typename T>
      T* operator+(Utilities::Memory::Trace& trace, T* data)
      {
        Utilities::Memory::Stamp(data, trace, typeid(T).name());
        return data;
      }

      #define VIRTUS_NEW Grimelins::Utilities::Memory::Trace(__FILE__, __LINE__) + ::new
      #define new VIRTUS_NEW

      #define VIRTUS_MALLOC(size) Grimelins::Utilities::Memory::Stamp(\
        Grimelins::Utilities::Memory::Allocate(size), Grimelins::Utilities::Memory::Trace(__FILE__, __LINE__), MALLOC_TYPE_NAME)
      #define malloc(size) VIRTUS_MALLOC(size)

      #define calloc "UNIMPLEMENTED";

      #define VIRTUS_FREE(dat) Grimelins::Utilities::Memory::Free(dat)
      #define free VIRTUS_FREE

    #else
      #define VIRTUS_NEW new  
      #define VIRTUS_MALLOC malloc
      #define VIRTUS_FREE free
    #endif // #if VIRTUS_CONFIG_DEBUG_MODE

  } // namespace virtus

#elif CONFIG_MEMORY_MANAGEMENT == 0 // #if MEMORY_MANAGEMENT

#define VIRTUS_NEW new
#define VIRTUS_FREE free
#define VIRTUS_MALLOC malloc

namespace Grimelins
{
  namespace Utilities
  {
    namespace Memory
    {
      void* __BYPASSMALLOC(size_t size);
      void __BYPASSFREE(void* ptr);
    }
  }
}

#endif // #if MEMORY_POOLING

#endif // Include Guard