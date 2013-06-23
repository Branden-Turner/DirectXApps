/*! ====================================================================================================================
  @file error.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.13.2011

  @brief 
    Error handline in the virtus engine.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_ERROR_H
#define VIRTUS_ERROR_H

namespace Grimelins
{
  // Just throw one of these errors from somewhere in your code. The error is guaranteed to be caught in the main
  // loop, and the user is notified. (This is a pretty drastic step)
  enum
  {
    E_UNKNOWN = 1000,           /// Unknown error, probably shouldn't be thrown

    // Hard Drive
    E_HD_NOT_ENOUGH_SPACE,      /// Not enough hard disk space for runtime

    // RAM
    E_MEM_PHYS_NOT_ENOUGH,      /// Not enough physical memory present (piece of shit machine)
    E_MEM_VIRT_NOT_ENOUGH,      /// Not enough memory can be allocated by OS
    E_MEM_CONT_NOT_NEOUGH,      /// Not enough memory can be allocated in contiguous block (OS is lying)

    // CPU
    E_CPU_TOO_SLOW,             /// CPU speed doesn't match requirements

    // Instance
    E_INSTANCE_NOT_ONLY,        /// Another instance of the game is running

    // Configuration
    E_CONFIG_MISSING,           /// Configuration file could not be read

    // Memory
    E_MEM_NOT_INITIALZED,       /// Memory manager was never initialized
    E_MEM_ERROR,                /// Error in memory manager

    // Graphics
    E_GFXDEVICE_NOT_AVAILABLE,  /// Problems with creating the graphics rendering device
    E_GFX_DRIVER_ERROR,         /// Driver error, ohshit.

    //Sound
    E_SOUND_NOT_INITIALIZED,    /// Sound failed to init.

    //Input
    E_UNKNOWN_KEY,

    // Core
    E_MISSING_DLL

  };
}

#endif