/*! ====================================================================================================================
  @file mempool.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.19.2011

  @brief 
    Pools objects of same size.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "apputilities.h"
#include "mempool.h"

/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */

    #undef malloc
    #undef free

/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */

namespace Grimelins
  {
  namespace Utilities
  {
    namespace Memory
    {
      /*
        This is a Page:
        +------------------------------------------------------------------------------------------+
        | PAGE POINTER | ALIGNMENT | BLOCK | INTERALIGNMENT | BLOCK | INTERALIGNMENT | ...         |
        +------------------------------------------------------------------------------------------+

        Where:
        PAGE POINTER points to the next page in use (NULL if no more pages exist)
        ALIGNMENT aligns the BLOCK on the configured memory alignment line
        BLOCK is the memory block as described below
        INTERALIGNMENT is alignment between blocks to keep the memory on alignment boundaries

        This is a BLOCK:
        +----------------------------------------------------------+
        | HEADER | USERBYTES | PADDING | USER DATA CHUNK | PADDING |
        +----------------------------------------------------------+

        Where:
        HEADER contains block mask information
        USERBYTES contains additional memory information (usually used for memory trace)
        PADDING is a DMZ around the user data
        USER DATA CHUNK is the memory intended for the end user
      */

      /////////// Public Autos ////////////

      Pool::Pool(unsigned ObjectSize, const PoolCfg& config) throw(MemoryException)
        : m_Config(config), m_PageList(NULL), m_FreeList(NULL) 
      {
        // Check configuration provided
        if (ObjectSize == 0)
          throw MemoryException(MemoryException::E_BAD_CONFIG, "Can not pool objects of size 0.");

        if (ObjectSize < sizeof(char*))
          throw MemoryException(MemoryException::E_BAD_CONFIG, "Can not pool objects of size less than char*.");

        if (m_Config.m_ObjectsPerPage == 0 && !m_Config.m_BypassPool)
          throw MemoryException(MemoryException::E_BAD_CONFIG, "Can not pool objects on pages of size 0.");

        if (m_Config.m_MaxNumPages == 0 && !m_Config.m_BypassPool)
          throw MemoryException(MemoryException::E_BAD_CONFIG, "Can not use pool with a maximum number of pages of 0.");

        if (config.m_Diagnostics && m_Config.m_BypassPool)
          throw MemoryException(MemoryException::E_BAD_CONFIG, "Can not diagnose memory use when bypassing pool.");

        m_Stats.m_ObjectSize = ObjectSize;
        m_BlockSize = m_Config.m_HeaderBlockBytes + m_Config.m_UserBytes + (m_Config.m_PadBytes * 2) + ObjectSize;
        m_BlockOffsetToData = m_Config.m_HeaderBlockBytes + m_Config.m_UserBytes + m_Config.m_PadBytes;

        if (m_Config.m_Alignment > 1)
        {
          // Calculate Left alignment buffer
          // Size is determined by number of bytes to next multiple of alignment
          u32 offsetToFirstBlock = sizeof(void*) + m_Config.m_PadBytes + m_Config.m_HeaderBlockBytes + m_Config.m_UserBytes;
          m_Config.m_LeftAlignBytes = m_Config.m_Alignment - (offsetToFirstBlock % m_Config.m_Alignment);

          // Calculate Inter alignment buffer
          u32 offsetBetweenBlocks = (m_Config.m_PadBytes * 2) + ObjectSize + m_Config.m_HeaderBlockBytes + m_Config.m_UserBytes;
          m_Config.m_InterAlignBytes = m_Config.m_Alignment - (offsetBetweenBlocks % m_Config.m_Alignment);
        }

        // Calculate total page size which is:
        // The page pointer + Left Alignment + blocksize * blocks + interalignments
        m_Stats.m_MemoryPageSize = sizeof(void*) + m_Config.m_LeftAlignBytes;
        m_Stats.m_MemoryPageSize +=  m_BlockSize * m_Config.m_ObjectsPerPage;
        m_Stats.m_MemoryPageSize += m_Config.m_InterAlignBytes * (m_Config.m_ObjectsPerPage - 1);

        if (!m_Config.m_BypassPool)
          allocatePage();
      }

      Pool::~Pool(void) throw()
      {
        while (m_PageList)
        {
          char* del = m_PageList;
          m_PageList = RCAST(char*, RCAST(MemLink*, m_PageList)->Next);
          Application::aligned_free(SCAST(void*, del));
        }
      }

      /////////// Public Methods ////////////

      void* Pool::Allocate(void) throw(MemoryException)
      {
        // Bypass ObjectAllocator
        if (m_Config.m_BypassPool)
        {
          void* block = Application::aligned_malloc(m_Stats.m_ObjectSize, m_Config.m_Alignment);

          if (!block)
            throw MemoryException(MemoryException::E_NO_MEMORY, "The system ran out of memory.");


          ++m_Stats.m_NumAllocations;
          ++m_Stats.m_NumAllocatedBlocks;

          if (m_Stats.m_NumAllocatedBlocks > m_Stats.m_MaxAllocatedObjects)
            m_Stats.m_MaxAllocatedObjects = m_Stats.m_NumAllocatedBlocks;

          return block;
        }

        if (m_Stats.m_NumFreeBlocks == 0)
          allocatePage();

        ++m_Stats.m_NumAllocations;
        --m_Stats.m_NumFreeBlocks;
        ++m_Stats.m_NumAllocatedBlocks;

        if (m_Stats.m_NumAllocatedBlocks > m_Stats.m_MaxAllocatedObjects)
          m_Stats.m_MaxAllocatedObjects = m_Stats.m_NumAllocatedBlocks;

        void* block = RCAST(void*, m_FreeList);
        m_FreeList = RCAST(char*, RCAST(MemLink*, m_FreeList)->Next);

        // Set the allocated pattern
        if (m_Config.m_Diagnostics)
          std::memset(block, ALLOCATED_PATTERN, m_Stats.m_ObjectSize);

        // Configure header block
        if (m_Config.m_HeaderBlockBytes)
        {
          char* header = RCAST(char*, block) - m_BlockOffsetToData;
          std::memset(header, 0, m_Config.m_HeaderBlockBytes - BYTE);
          std::memset(header + m_Config.m_HeaderBlockBytes - BYTE, MASK_IS_IN_USE, BYTE);
        }

        // Prepare User block
        if (m_Config.m_UserBytes)
        {
          char* userbytes = RCAST(char*, block) - m_Config.m_PadBytes - m_Config.m_UserBytes;
          std::memset(userbytes, USERSTAMP_PATTERN, m_Config.m_UserBytes);
        }

        return block;
      }

      void Pool::Free(void* Object) throw(MemoryException)
      {
        // Bypass ObjectAllocator
        if (m_Config.m_BypassPool)
        {
          Application::aligned_free(Object);
          ++m_Stats.m_NumDeallocations;
          --m_Stats.m_NumAllocatedBlocks;
          return;
        }

        if (m_Config.m_Diagnostics)
        {
          // Validate Block
          BlockIntegrity res = validateBlock(RCAST(char*, Object));

          switch (res)
          {
          case BLOCK_OK:
            break;
          case BLOCK_NOT_ON_PAGE:
            throw MemoryException(MemoryException::E_BAD_ADDRESS, 
              "Attempt to free object that was not allocated by ObjectAllocator.");
          case BLOCK_NOT_ON_BOUNDAY:
            throw MemoryException(MemoryException::E_BAD_BOUNDARY, 
              "Attempt to free an object at a bad boundary.");
          case BLOCK_PADDING_CORRUPTED:
            throw (MemoryException(MemoryException::E_CORRUPTED_BLOCK,
              "Block has been corrupted (PAD PATTERN CORRUPT)."));
          }

          // Make sure it is still in use (not on the free list)
          if (!isBlockInUse(RCAST(char*, Object)))
            throw MemoryException(MemoryException::E_MULTIPLE_FREE,
              "Attempt to free object that is already deallocated.");

          // All tests passed, pattern the free memory
          std::memset(Object, FREED_PATTERN, m_Stats.m_ObjectSize);
        }

        if (m_Config.m_HeaderBlockBytes)
        {
          char* header = RCAST(char*, Object) - m_BlockOffsetToData;
          std::memset(header, 0, m_Config.m_HeaderBlockBytes);
        }

        // Push freed block to front of list
        char* temp = m_FreeList;
        m_FreeList = RCAST(char*, Object);
        RCAST(MemLink*, m_FreeList)->Next = RCAST(MemLink*, temp);

        ++m_Stats.m_NumFreeBlocks;
        ++m_Stats.m_NumDeallocations;
        --m_Stats.m_NumAllocatedBlocks;
      }

      unsigned Pool::DumpMemoryInUse(fnDumpHook fn, void* userdat) const
      {
        char* walker = m_PageList;

        while (walker)
        {
          char* firstBlock = walker + sizeof(void*) + m_Config.m_LeftAlignBytes + m_BlockOffsetToData;
          char* lastBlock = walker + m_Stats.m_MemoryPageSize - m_Config.m_PadBytes - m_Stats.m_ObjectSize;

          // Check every object on the page (except the last one)
          while (firstBlock != lastBlock)
          {
            if (isBlockInUse(firstBlock))
              fn(firstBlock, m_Stats.m_ObjectSize, userdat);

            firstBlock += m_BlockSize + m_Config.m_InterAlignBytes;
          }

          // Check last object on the page
          if (isBlockInUse(lastBlock))
            fn(lastBlock, m_Stats.m_ObjectSize, userdat);

          walker = RCAST(char*, RCAST(MemLink*, walker)->Next);
        }


        return m_Stats.m_NumAllocatedBlocks;
      }

      unsigned Pool::ValidatePages(fnValidationHook fn, void* userdat) const
      {
        char* page = m_PageList;
        int corruptionCount = 0;

        // Walk the pages
        while (page)
        {
          char* currBlock = page + sizeof(void*) + m_Config.m_LeftAlignBytes + m_BlockOffsetToData;
          char* lastBlock = page + m_Stats.m_MemoryPageSize - m_Config.m_PadBytes - m_Stats.m_ObjectSize;

          // Check blocks on page (except last block)
          while (currBlock != lastBlock)
          {
            BlockIntegrity res = validateBlock(currBlock);

            if (res != BLOCK_OK)
            {
              fn(currBlock, m_Stats.m_ObjectSize, userdat);
              ++corruptionCount;
            }

            currBlock += m_BlockSize + m_Config.m_InterAlignBytes;
          }

          // Check last block on page
          BlockIntegrity res = validateBlock(lastBlock);

          if (res != BLOCK_OK)
          {
            fn(currBlock, m_Stats.m_ObjectSize, userdat);
            ++corruptionCount;
          }

          page = RCAST(char*, RCAST(MemLink*, page)->Next);
        }

        return corruptionCount;
      }


      void Pool::Stamp(void* block, void* Pattern, size_t bytes) throw (MemoryException)
      {
        if (bytes > m_Config.m_UserBytes)
          throw MemoryException(MemoryException::E_BAD_CONFIG, 
            "Attempt to write more userbytes than are available (per configuration).");

        if (m_Config.m_Diagnostics)
          validateBlock(RCAST(char*, block));

        void* userbytes = RCAST(char*, block) - m_Config.m_PadBytes - m_Config.m_UserBytes;
        std::memcpy(userbytes, Pattern, bytes);
      }

      bool Pool::IsBlockInPool(void* block)
      {
        // Don't actually care about integrity, just want to make sure we found
        // the memory pool the object resides in (Block must be on page).
        return validateBlock(RCAST(char*, block)) != BLOCK_NOT_ON_PAGE;
      }

      unsigned Pool::Defragment(void)
      {
        //unsigned pagesFreed = 0;
        //char* page = m_PageList;

        //while (page)
        //{
        //  GenericObject* i = reinterpret_cast<GenericObject*>(m_FreeList);
        //  unsigned freeBlocks = 0;

        //  for (; i; i = i->Next)
        //  {
        //    if (isOnPage(reinterpret_cast<char*>(i), page))
        //      ++freeBlocks;

        //    if (freeBlocks == m_Config.ObjectsPerPage_)
        //    {
        //      char *memory = page;

        //      // Progress to first block not on deleted page
        //      while (isOnPage(reinterpret_cast<char*>(m_FreeList), page))
        //        m_FreeList = reinterpret_cast<char*>(reinterpret_cast<GenericObject*>(m_FreeList)->Next);

        //      
        //      // Unlink all free objects on page to be freed
        //      GenericObject* walker = reinterpret_cast<GenericObject*>(m_FreeList);

        //      while (walker)
        //      {
        //        while (walker->Next && isOnPage(reinterpret_cast<char*>(walker->Next), page))
        //          walker->Next = walker->Next->Next;
        //        
        //        walker = walker->Next;
        //      }

        //      // delete page
        //      if (m_PageList == page)
        //      {
        //        m_PageList = reinterpret_cast<char*>(reinterpret_cast<GenericObject*>(m_PageList)->Next);
        //        page = reinterpret_cast<char*>(page);
        //      }
        //      else
        //      { 
        //        GenericObject* i;
        //        for (i = reinterpret_cast<GenericObject*>(m_PageList); i->Next != reinterpret_cast<GenericObject*>(page); i = i->Next) 
        //        {
        //        }

        //        i->Next = reinterpret_cast<GenericObject*>(page)->Next;
        //        page = reinterpret_cast<char*>(i);
        //      }

        //      operator delete(memory);
        //      page = reinterpret_cast<char*>(i);

        //      ++pagesFreed;
        //      break;
        //    }
        //  }
        //  
        //  if (freeBlocks != m_Config.ObjectsPerPage_)
        //    page = reinterpret_cast<char*>(reinterpret_cast<GenericObject*>(page)->Next);
        //}

        //return pagesFreed;
        return 0;
      }


      /////////// Debugging Features ////////////
      void Pool::SetDebugState(bool State)
      {
        m_Config.m_Diagnostics = State;
      }

      PoolCfg Pool::GetConfiguration(void) const
      {
        return m_Config;
      }

      PoolStats Pool::GetStatistics(void) const
      {
        return m_Stats;
      }

      /////////// Private Methods ////////////

      char* Pool::allocatePage(void)
      {
        // Check if further allocation is allowed
        if (m_Config.m_MaxNumPages && m_Stats.m_NumMemoryPages == m_Config.m_MaxNumPages)
        {
          throw MemoryException(MemoryException::E_NO_PAGES, "Reached maximum number of pages.");
        }

        // Allocate the page space required
        char* page = SCAST(char*, Application::aligned_malloc(m_Stats.m_MemoryPageSize, m_Config.m_Alignment));

        if (!page)
          throw MemoryException(MemoryException::E_NO_MEMORY, "The system ran out of memory.");

        if (m_Config.m_Diagnostics)
          patternPage(page);

        // Set the page pointer
        RCAST(MemLink*, page)->Next = RCAST(MemLink*, m_PageList);
        m_PageList = page;

        // Calculate boundaries of block space
        char* firstPageBlock = page + sizeof(void*) + m_Config.m_LeftAlignBytes;
        char* lastPageBlock = page + m_Stats.m_MemoryPageSize - m_BlockSize;

        // Walk the page space and split it up into blocks of object size
        // by extending the free list
        char* last = NULL;
        char* walker = firstPageBlock;

        while (walker != lastPageBlock)
        {
          RCAST(MemLink*, walker + m_BlockOffsetToData)->Next = RCAST(MemLink*, last);
          last = RCAST(char*, RCAST(MemLink*, walker + m_BlockOffsetToData));
          walker += m_BlockSize + m_Config.m_InterAlignBytes;
        }

        // The last object on the page was left untouched by loop, completing list:
        RCAST(MemLink*, walker + m_BlockOffsetToData)->Next = RCAST(MemLink*, last);
        m_FreeList = walker + m_BlockOffsetToData;

        // Update statistics
        m_Stats.m_NumFreeBlocks += m_Config.m_ObjectsPerPage;
        ++m_Stats.m_NumMemoryPages;

        return page;
      }

      void Pool::patternPage(char* page)
      {
        char* walker = page;

        walker += sizeof(void*);

        if (m_Config.m_Alignment > 0)
        {
          std::memset(walker, ALIGN_PATTERN, m_Config.m_LeftAlignBytes);
          walker += m_Config.m_LeftAlignBytes;
        }

        char* lastBlock = page + m_Stats.m_MemoryPageSize - m_BlockSize;

        while (walker != lastBlock)
        {
          std::memset(walker, 0, m_Config.m_HeaderBlockBytes);
          walker += m_Config.m_HeaderBlockBytes;
          std::memset(walker, USERSTAMP_PATTERN, m_Config.m_UserBytes);
          walker += m_Config.m_UserBytes;
          std::memset(walker, PAD_PATTERN, m_Config.m_PadBytes);
          walker += m_Config.m_PadBytes;
          std::memset(walker, UNALLOCATED_PATTERN, m_Stats.m_ObjectSize);
          walker += m_Stats.m_ObjectSize;
          std::memset(walker, PAD_PATTERN, m_Config.m_PadBytes);
          walker += m_Config.m_PadBytes;
          std::memset(walker, ALIGN_PATTERN, m_Config.m_InterAlignBytes);
          walker += m_Config.m_InterAlignBytes;
        }

        // Everything is patterned except the last block
        std::memset(walker, 0, m_Config.m_HeaderBlockBytes);
        walker += m_Config.m_HeaderBlockBytes;
        std::memset(walker, USERSTAMP_PATTERN, m_Config.m_UserBytes);
        walker += m_Config.m_UserBytes;
        std::memset(walker, PAD_PATTERN, m_Config.m_PadBytes);
        walker += m_Config.m_PadBytes;
        std::memset(walker, UNALLOCATED_PATTERN, m_Stats.m_ObjectSize);
        walker += m_Stats.m_ObjectSize;
        std::memset(walker, PAD_PATTERN, m_Config.m_PadBytes);
      }

      Pool::BlockIntegrity Pool::validateBlock(char* block) const
      {
        // Check if Object is on a page
        char* walker = m_PageList;

        while (walker)
        {
          // Is Object on page?
          if (block >= walker && block < (walker + m_Stats.m_MemoryPageSize))
          {
            // Is Object at correct boundary?
            unsigned left = sizeof(void*) + m_Config.m_LeftAlignBytes + m_BlockOffsetToData;

            if (block >= (walker + left))
            {
              // Normalize address (first block is at addr 0)
              int normAddr = RCAST(int, block) - RCAST(int, walker) - left;

              if (!(normAddr % (m_BlockSize + m_Config.m_InterAlignBytes)))
              {
                // Check the padding if applicable
                if (m_Config.m_PadBytes)
                {
                  // Check padding before block
                  unsigned char* walker = RCAST(unsigned char*, block - m_Config.m_PadBytes);
                  unsigned char* end = RCAST(unsigned char*, block);

                  for (; walker != end; ++walker)
                    if (*walker != PAD_PATTERN)
                      return BLOCK_PADDING_CORRUPTED;

                  // Check padding after block
                  walker = RCAST(unsigned char*, block + m_Stats.m_ObjectSize);
                  end = walker + m_Config.m_PadBytes;

                  for (; walker != end; ++walker)
                    if (*walker != PAD_PATTERN)
                      return BLOCK_PADDING_CORRUPTED;
                }

                return BLOCK_OK; // Everything is fine
              }
            }

            // Boundary incorrect
            return BLOCK_NOT_ON_BOUNDAY;
          }

          walker = RCAST(char*, RCAST(MemLink*, walker)->Next);
        }

        // Object not found on any page
        return BLOCK_NOT_ON_PAGE;

      }

      bool Pool::isBlockInUse(char* block) const
      {
        // Is it possible to have a fast and constant lookup if the block is free
        // or not?
        if (m_Config.m_HeaderBlockBytes)
        {
          // Find Header block
          unsigned char* flag = RCAST(unsigned char*, block) - m_Config.m_PadBytes - m_Config.m_UserBytes - BYTE;

          // Check right most bit (which stores whether the block is free or not)
          if (*flag != MASK_IS_IN_USE)
            return false;
        }
        // Have to walk the entire FreeList to check if the block is in use
        else
        {
          char* walker = m_FreeList;

          while (walker)
          {
            if (block == walker)
              return false;

            walker = RCAST(char*, RCAST(MemLink*, walker)->Next);
          }
        }

        return true;
      }

      bool Pool::isBlockOnPage(char* block, char* page) const
      {
        return (block >= page && block < (page + m_Stats.m_MemoryPageSize));
      }

    } // namesapce Memory
  } // namespace Utilities
} // namespace virtus