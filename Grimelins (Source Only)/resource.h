/*! ====================================================================================================================
  @file resource.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 29.Sep.2011

  @brief 
    
    Used by Virtus.rc

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by GrimelinsToo.rc
//
#define IDI_MAINICON                       101

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        102
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif


// Define Easy mode checking for debug mode
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


//////////////////////////////////////////////////////////////////////////
// CONFIGURATION

#define WINVER 0x05010300          // Target platform: Win XP SP3

#if VIRTUS_CONFIG_DEBUG_MODE


// General.
#define CONFIG_GENERAL_TITLE "Grimelins Debug"     // Title of application

// Memory.
#define CONFIG_MEMORY_MANAGEMENT 0              // Enable memory management
#define CONFIG_MEMORY_STATISTICS 1              // Collect statistics
#define CONFIG_MEMORY_DIAGNOSTICS 1             // Memory diagnostics (pattern fill)
#define CONFIG_MEMORY_NUM_POOLS 0               // Number of pools loaded at startup
#define CONFIG_MEMORY_UNPOOLED_CAPACITY  512    // Startup capacity of un pooled lookup table
#define CONFIG_MEMORY_UNPOOLED_GROW_FACTOR 2    // The factor the un pooled lookup table grows by
#define CONFIG_MEMORY_DUMP_ON_EXIT 1            // Dump memory in use when exiting
#define CONFIG_MEMORY_USE_HEADER_BLOCKS 1       // Use header blocks for client blocks
#define CONFIG_MEMORY_MAX_PAGES 512             // Maximum number of pages for one block type
#define CONFIG_MEMORY_MAX_POOLED_SIZE 512       // Maximum block size that is pooled
#define CONFIG_MEMORY_ALIGNMENT 16              // Physical alignment of client blocks
#define CONFIG_MEMORY_OBJECTS_PER_PAGE 32       // Number of objects on each page
#define CONFIG_MEMORY_PAD_BYTES 16              // Pad size around client memory

// Requirements.
#define CONFIG_REQ_HD_SPACE 524288000           // Hard drive space requirements
#define CONFIG_REQ_RAM 1073741824               // RAM space requirement
#define CONFIG_REQ_CPU_SCORE 1000               // CPU score

// Threads.
#define CONFIG_CORE_THREAD_PER_CORE 2           // Number of threads per logical core

//Paths
#define CONFIG_PATH_FONT "./assets/font/"       // Font folder
#define CONFIG_PATH_MESH "./assets/mesh/"       // Mesh folder
#define CONFIG_PATH_SCRIPT "./assets/script/"   // Script folder
#define CONFIG_PATH_SHADER "./assets/shader/"   // Shaders folder
#define CONFIG_PATH_SOUND "./assets/sound/"     // Sounds folder
#define CONFIG_PATH_TEXTURE "./assets/texture/" // Texutes folder
#define CONFIG_PATH_LEVELS "./assets/levels/"   // levels folder
#define CONFIG_PATH_OBJECTS "./assets/objects/" // objects folder

// Graphics.
#define CONFIG_GFX_START_FULLSCREEN 0           // Start in fullscreen
#define CONFIG_GFX_DEFAULT_WINDOW_WIDTH 1920    // Default window width
#define CONFIG_GFX_DEFAULT_WINDOW_HEIGHT 1080    // Default window height


// Printing.
#define CONFIG_PRINT_VS 1                       // Print to visual studio console
#define CONFIG_DEFAULT_LOG_FILE "lastrung.log"  // Default log file to print to

#elif VIRTUS_CONFIG_PROFILE_MODE


// General.
#define CONFIG_GENERAL_TITLE "Grimelins Profile"   // Title of application

// Memory.
#define CONFIG_MEMORY_MANAGEMENT 0              // Enable memory management
#define CONFIG_MEMORY_STATISTICS 1              // Collect statistics
#define CONFIG_MEMORY_DIAGNOSTICS 1             // Memory diagnostics (pattern fill)
#define CONFIG_MEMORY_NUM_POOLS 0               // Number of pools loaded at startup
#define CONFIG_MEMORY_UNPOOLED_CAPACITY  512    // Startup capacity of un pooled lookup table
#define CONFIG_MEMORY_UNPOOLED_GROW_FACTOR 2    // The factor the un pooled lookup table grows by
#define CONFIG_MEMORY_DUMP_ON_EXIT 1            // Dump memory in use when exiting
#define CONFIG_MEMORY_USE_HEADER_BLOCKS 1       // Use header blocks for client blocks
#define CONFIG_MEMORY_MAX_PAGES 512             // Maximum number of pages for one block type
#define CONFIG_MEMORY_MAX_POOLED_SIZE 512       // Maximum block size that is pooled
#define CONFIG_MEMORY_ALIGNMENT 16              // Physical alignment of client blocks
#define CONFIG_MEMORY_OBJECTS_PER_PAGE 32       // Number of objects on each page
#define CONFIG_MEMORY_PAD_BYTES 4               // Pad size around client memory

// Requirements.
#define CONFIG_REQ_HD_SPACE 524288000           // Hard drive space requirements
#define CONFIG_REQ_RAM 1073741824               // RAM space requirement
#define CONFIG_REQ_CPU_SCORE 1000               // CPU score

// Threads
#define CONFIG_CORE_THREAD_PER_CORE 2           // Number of threads per logical core

// Paths
#define CONFIG_PATH_FONT "./assets/font/"       // Font folder
#define CONFIG_PATH_MESH "./assets/mesh/"       // Mesh folder
#define CONFIG_PATH_SCRIPT "./assets/script/"   // Script folder
#define CONFIG_PATH_SHADER "./assets/shader/"   // Shaders folder
#define CONFIG_PATH_SOUND "./assets/sound/"     // Sounds folder
#define CONFIG_PATH_TEXTURE "./assets/texture/" // Texutes folder
#define CONFIG_PATH_LEVELS "./assets/levels/"   // levels folder
#define CONFIG_PATH_OBJECTS "./assets/objects/" // objects folder

// Graphics.
#define CONFIG_GFX_START_FULLSCREEN 0           // Start in fullscreen
#define CONFIG_GFX_DEFAULT_WINDOW_WIDTH 1024    // Default window width
#define CONFIG_GFX_DEFAULT_WINDOW_HEIGHT 576    // Default window height

// Printing.
#define CONFIG_PRINT_VS 1                       // Print to visual studio console
#define CONFIG_DEFAULT_LOG_FILE "lastrung.log"  // Default log file to print to

#else



// General.
#define CONFIG_GENERAL_TITLE "Grimelins"        // Title of application

// Memory.
#define CONFIG_MEMORY_MANAGEMENT 0              // Enable memory management
#define CONFIG_MEMORY_STATISTICS 0              // Collect statistics
#define CONFIG_MEMORY_DIAGNOSTICS 0             // Memory diagnostics (pattern fill)
#define CONFIG_MEMORY_NUM_POOLS 0               // Number of pools loaded at startup
#define CONFIG_MEMORY_UNPOOLED_CAPACITY  512    // Startup capacity of un pooled lookup table
#define CONFIG_MEMORY_UNPOOLED_GROW_FACTOR 2    // The factor the un pooled lookup table grows by
#define CONFIG_MEMORY_DUMP_ON_EXIT 0            // Dump memory in use when exiting
#define CONFIG_MEMORY_USE_HEADER_BLOCKS 1       // Use header blocks for client blocks
#define CONFIG_MEMORY_MAX_PAGES 512             // Maximum number of pages for one block type
#define CONFIG_MEMORY_MAX_POOLED_SIZE 512       // Maximum block size that is pooled
#define CONFIG_MEMORY_ALIGNMENT 16              // Physical alignment of client blocks
#define CONFIG_MEMORY_OBJECTS_PER_PAGE 32       // Number of objects on each page
#define CONFIG_MEMORY_PAD_BYTES 0               // Pad size around client memory

// Requirements.
#define CONFIG_REQ_HD_SPACE 524288000           // Hard drive space requirements
#define CONFIG_REQ_RAM 1073741824               // RAM space requirement
#define CONFIG_REQ_CPU_SCORE 1000               // CPU score

// Threads.
#define CONFIG_CORE_THREAD_PER_CORE 2           // Number of threads per logical core

// Paths
#define CONFIG_PATH_FONT "./assets/font/"       // Font folder
#define CONFIG_PATH_MESH "./assets/mesh/"       // Mesh folder
#define CONFIG_PATH_SCRIPT "./assets/script/"   // Script folder
#define CONFIG_PATH_SHADER "./assets/shader/"   // Shaders folder
#define CONFIG_PATH_SOUND "./assets/sound/"     // Sounds folder
#define CONFIG_PATH_TEXTURE "./assets/texture/" // Texutes folder
#define CONFIG_PATH_LEVELS "./assets/levels/"   // levels folder
#define CONFIG_PATH_OBJECTS "./assets/objects/" // objects folder


// Graphics.
#define CONFIG_GFX_START_FULLSCREEN 1           // Start in fullscreen
#define CONFIG_GFX_DEFAULT_WINDOW_WIDTH 1920    // Default window width
#define CONFIG_GFX_DEFAULT_WINDOW_HEIGHT 1080    // Default window height

// Printing.
#define CONFIG_PRINT_VS 0                       // Print to visual studio console
#define CONFIG_DEFAULT_LOG_FILE "lastrung.log"  // Default log file to print to

#endif
