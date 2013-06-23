/*! ====================================================================================================================
  @file fwdefinitions.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Oct.15.2011

  @brief 
    Virtus definitions

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_FWDEFINITIONS_H
#define VIRTUS_FWDEFINITIONS_H


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// configuration segment.

#ifndef VIRTUS_CONFIG_MEMORY_MANAGEMENT
# define VIRTUS_CONFIG_MEMORY_MANAGEMENT 0              // Enable memory management
#endif

#ifndef VIRTUS_CONFIG_MEMORY_STATISTICS
# define VIRTUS_CONFIG_MEMORY_STATISTICS 0              // Collect statistics
#endif

#ifndef VIRTUS_CONFIG_MEMORY_DIAGNOSTICS
# define VIRTUS_CONFIG_MEMORY_DIAGNOSTICS 0             // Memory diagnostics (pattern fill)
#endif

#ifndef VIRTUS_CONFIG_MEMORY_NUM_POOLS
# define VIRTUS_CONFIG_MEMORY_NUM_POOLS 0               // Number of pools loaded at startup
#endif

#ifndef VIRTUS_CONFIG_MEMORY_UNPOOLED_CAPACITY
# define VIRTUS_CONFIG_MEMORY_UNPOOLED_CAPACITY  0      // Startup capacity of un pooled lookup table
#endif

#ifndef VIRTUS_CONFIG_MEMORY_UNPOOLED_GROW_FACTOR
# define VIRTUS_CONFIG_MEMORY_UNPOOLED_GROW_FACTOR 2    // The factor the un pooled lookup table grows by
#endif

#ifndef VIRTUS_CONFIG_MEMORY_DUMP_ON_EXIT
# define VIRTUS_CONFIG_MEMORY_DUMP_ON_EXIT 0            // Dump memory in use when exiting
#endif

#ifndef VIRTUS_CONFIG_MEMORY_USE_HEADER_BLOCKS
# define VIRTUS_CONFIG_MEMORY_USE_HEADER_BLOCKS 0       // Use header blocks for client blocks
#endif

#ifndef VIRTUS_CONFIG_MEMORY_MAX_PAGES
# define VIRTUS_CONFIG_MEMORY_MAX_PAGES 512             // Maximum number of pages for one block type
#endif

#ifndef VIRTUS_CONFIG_MEMORY_MAX_POOLED_SIZE
# define VIRTUS_CONFIG_MEMORY_MAX_POOLED_SIZE 512       // Maximum block size that is pooled
#endif

#ifndef VIRTUS_CONFIG_MEMORY_ALIGNMENT
# define VIRTUS_CONFIG_MEMORY_ALIGNMENT 1               // Physical alignment of client blocks
#endif

#ifndef VIRTUS_CONFIG_MEMORY_OBJECTS_PER_PAGE
# define VIRTUS_CONFIG_MEMORY_OBJECTS_PER_PAGE 32       // Number of objects on each page
#endif

#ifndef VIRTUS_CONFIG_MEMORY_PAD_BYTES
# define VIRTUS_CONFIG_MEMORY_PAD_BYTES 0               // Pad size around client memory
#endif

#ifndef VIRTUS_CONFIG_REQ_HD_SPACE
# define VIRTUS_CONFIG_REQ_HD_SPACE 0                   // Hard drive space requirements
#endif

#ifndef VIRTUS_CONFIG_REQ_RAM
# define VIRTUS_CONFIG_REQ_RAM 0                        // RAM space requirement
#endif

#ifndef VIRTUS_CONFIG_REQ_CPU_SCORE
# define VIRTUS_CONFIG_REQ_CPU_SCORE 0                  // CPU score
#endif

#ifndef VIRTUS_CONFIG_CORE_THREAD_PER_CORE
# define VIRTUS_CONFIG_CORE_THREAD_PER_CORE 1           // Number of threads per logical core
#endif

#ifndef VIRTUS_CONFIG_PRINT_VS
# define VIRTUS_CONFIG_PRINT_VS 1                       // Print to visual studio console
#endif

#ifndef VIRTUS_CONFIG_DEFAULT_LOG_FILE
# define VIRTUS_CONFIG_DEFAULT_LOG_FILE "lastrung.log"  // Default log file to print to
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// configuration segment.

#ifndef VIRTUS_CONFIG_DEBUG_MODE
# ifdef _DEBUG
#   define VIRTUS_CONFIG_DEBUG_MODE 1
# else
#   define VIRTUS_CONFIG_DEBUG_MODE 0
# endif
#endif

// Define Easy mode checking for profile mode
#ifndef VIRTUS_CONFIG_PROFILE_MODE
# ifdef _PROFILE
#   define VIRTUS_CONFIG_PROFILE_MODE 1
#   undef VIRTUS_CONFIG_DEBUG_MODE
#   define VIRTUS_CONFIG_DEBUG_MODE 0
# else
#   define VIRTUS_CONFIG_PROFILE_MODE 0
# endif
#endif

#ifndef VIRTUS_CONFIG_RELEASE_MODE
# if !VIRTUS_CONFIG_PROFILE_MODE && !VIRTUS_CONFIG_DEBUG_MODE
#   define VIRTUS_CONFIG_RELEASE_MODE 1
# else
#   define VIRTUS_CONFIG_RELEASE_MODE 0
# endif
#endif

// Debugger bells and whistles
#undef _HAS_ITERATOR_DEBUGGING
#undef _SECURE_SCL

#if VIRTUS_CONFIG_DEBUG_MODE
# define _HAS_ITERATOR_DEBUGGING 1    // Iterator debugging
# define _SECURE_SCL 1                // Checked iterators
#elif VIRTUS_CONFIG_PROFILE_MODE
# define _HAS_ITERATOR_DEBUGGING 0
# define _SECURE_SCL 0
#else
# define _HAS_ITERATOR_DEBUGGING 0
# define _SECURE_SCL 0
#endif


#ifdef _MSC_VER
# define Break() __debugbreak()
# pragma warning( disable : 4290 ) // suppress warning: C++ Exception Specification ignored
#else
# define Break() do { __asm int 3 } while(0)
#endif

#endif // Include Guard