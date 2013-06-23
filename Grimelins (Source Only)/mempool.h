/*! ====================================================================================================================
  @file mempool.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.19.2011

  @brief 
    Pools object of same size

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_MEMPOOL_H
#define VIRTUS_MEMPOOL_H

namespace Grimelins
{
  namespace Utilities
  {
    namespace Memory
    {
      #define BYTE sizeof(char)

      // If the client doesn't specify these:
      static const int DEFAULT_OBJECTS_PER_PAGE = 4;  
      static const int DEFAULT_MAX_PAGES = 3;

      /////////// Memory Exceptions ////////////
      class MemoryException
      {
      public: // declarations
        enum Excode 
        {
          E_NO_MEMORY,       /// Out of physical memory (operator new fails)
          E_NO_PAGES,        /// Out of logical memory (max pages has been reached)
          E_BAD_ADDRESS,     /// Block address is not on a page
          E_BAD_BOUNDARY,    /// Block address is on a page, but not on any block-boundary
          E_MULTIPLE_FREE,   /// Block has already been freed
          E_CORRUPTED_BLOCK, /// Block has been corrupted (pad bytes have been overwritten)
          E_BAD_CONFIG       /// Pool was mis configured
        };

      public: //autos
        MemoryException(Excode ErrCode, const std::string& Message) : m_Code(ErrCode), m_Msg(Message) {};
        virtual ~MemoryException() { }

      public: // methods
        Excode code(void) const { return m_Code; }
        virtual const char *what(void) const { return m_Msg.c_str(); }

      private: //data
        Excode m_Code;
        std::string m_Msg;
      };

      /////////// Pool Configuration ////////////
      struct PoolCfg
      {
        PoolCfg(   
                  bool BypassPool = false,                                 /// Bypass pooling and allocate natively
                  unsigned ObjectsPerPage = DEFAULT_OBJECTS_PER_PAGE,      /// Maximum number of objects per memory page
                  unsigned MaxPages = DEFAULT_MAX_PAGES,                   /// Maximum number of memory pages
                  bool Diagnostics = false,                                /// Memory Diagnostics
                  unsigned PadBytes = 0,                                   /// Pad memory blocks
                  unsigned HeaderBlockBytes = 0,                           /// Number of bytes used for header blocks
                  unsigned Alignment = 0,                                  /// Number of alignment bytes
                  unsigned UserBytes = 0                                   /// Number of client usable header bytes (usually used for trace)
               ) 
        : m_BypassPool(BypassPool), 
          m_ObjectsPerPage(ObjectsPerPage), 
          m_MaxNumPages(MaxPages), 
          m_Diagnostics(Diagnostics), 
          m_PadBytes(PadBytes),
          m_HeaderBlockBytes(HeaderBlockBytes),
          m_Alignment(Alignment),
          m_UserBytes(UserBytes)
        {
          m_LeftAlignBytes = 0;
          m_InterAlignBytes = 0;
        }

        bool     m_BypassPool;
        unsigned m_ObjectsPerPage;
        unsigned m_MaxNumPages;
        bool     m_Diagnostics;
        unsigned m_PadBytes;
        unsigned m_HeaderBlockBytes;
        unsigned m_Alignment;
        unsigned m_UserBytes;
        unsigned m_LeftAlignBytes;
        unsigned m_InterAlignBytes;
      };

      /////////// Pool Statistics ////////////
      struct PoolStats
      {
        PoolStats(void) : m_ObjectSize(0), 
                      m_NumFreeBlocks(0), 
                      m_NumAllocatedBlocks(0), 
                      m_NumMemoryPages(0),
                      m_MemoryPageSize(0), 
                      m_MaxAllocatedObjects(0), 
                      m_NumAllocations(0), 
                      m_NumDeallocations(0) 
        {

        };

        unsigned m_ObjectSize;          /// Size of each object
        unsigned m_NumFreeBlocks;       /// Number of objects on the free list
        unsigned m_NumAllocatedBlocks;  /// Number of objects in use by client
        unsigned m_NumMemoryPages;      /// Number of pages allocated
        unsigned m_MemoryPageSize;      /// Size of a page: ObjectsPerPage_ * ObjectSize_ + sizeof(void*)
        unsigned m_MaxAllocatedObjects; /// Most objects in use by client at one time
        unsigned m_NumAllocations;      /// Total requests to allocate memory
        unsigned m_NumDeallocations;    /// Total requests to free memory
      };

      /////////// Treat blocks like linked list ////////////
      struct MemLink
      {
        MemLink* Next;
      };

      /////////// Memory Pool ////////////
      class Pool
      {
      public: // Declarations
        typedef void (*fnDumpHook)(const void *, unsigned int, void* clientdat);
        typedef void (*fnValidationHook)(const void *, unsigned int, void* clientdat);

        // Memory patterns
        static const unsigned char UNALLOCATED_PATTERN = 0xAA;
        static const unsigned char ALLOCATED_PATTERN = 0xBB;
        static const unsigned char FREED_PATTERN = 0xCC;
        static const unsigned char PAD_PATTERN = 0xDD;
        static const unsigned char ALIGN_PATTERN = 0xEE;
        static const unsigned char USERSTAMP_PATTERN = 0xFF;
        static const size_t MIN_OBJ_SIZE = sizeof(char*);

      private: // Declarations
        static unsigned const char MASK_IS_IN_USE = 0x1;

        enum BlockIntegrity
        {
          BLOCK_OK,
          BLOCK_NOT_ON_PAGE,
          BLOCK_NOT_ON_BOUNDAY,
          BLOCK_PADDING_CORRUPTED
        };

      public: // Autos
        Pool(unsigned ObjectSize, const PoolCfg& config) throw(MemoryException);
        ~Pool() throw();

      public: // Methods
        void* Allocate() throw(MemoryException);
        void Free(void *Object) throw(MemoryException);
        unsigned Defragment(void);

        // Testing/Debugging/Statistic methods
        unsigned DumpMemoryInUse(fnDumpHook fn, void* userdat = NULL) const;
        unsigned ValidatePages(fnValidationHook fn, void* userdat = NULL) const;
        void Stamp(void* block, void* pattern, size_t bytes) throw (MemoryException);
        bool IsBlockInPool(void* block);

        void SetDebugState(bool State);            /// true=enable, false=disable
        PoolCfg GetConfiguration(void) const;       /// returns the configuration parameters
        PoolStats GetStatistics(void) const;           /// returns the statistics for the allocator

      private: // Methods
        // Make private to prevent copy construction and assignment
        Pool(const Pool &oa);
        Pool &operator=(const Pool &oa);

        char* allocatePage(void);
        void patternPage(char* page);
        bool isBlockInUse(char* block) const;
        bool isBlockOnPage(char* block, char* page) const;
        BlockIntegrity validateBlock(char* block) const;


      private: // Data
        PoolCfg m_Config;               /// Configuration parameters
        PoolStats m_Stats;                 /// Accumulating statistics
        char* m_PageList;              /// List of memory pages
        char* m_FreeList;              /// List of blocks that are free
        unsigned m_BlockSize;          /// Size of one block (incl. pad, header, user bytes)
        unsigned m_BlockOffsetToData;  /// Offset from beginning of block to user chunk
      };


    } // namespace Memory
  } // namespace Utilities
} // namespace virtus


#endif // Include Guard