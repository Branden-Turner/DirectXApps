/*! ====================================================================================================================
  @file appdiagnostics.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.14.2011

  @brief 
    Diagnostics sub system of application layer.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_APPDIAGNOSTICS_H
#define VIRTUS_APPDIAGNOSTICS_H


namespace Grimelins
{
  namespace Application
  {

    s32 GetCPUSpeed( s32 Sampletime );
    void CheckInstance( ch const* title );
    void CheckHardDisk( u64 spaceRequired );
    void CheckMemory( u64 physRamNeeded, u64 vRamNeeded );

    s32 StartTimingCPU( void );
    void UpdateCPUTime( void );
    s32 CalcCPUSpeed( void );

  } // namespace Application
} // namespace Virtus

#endif // Include Guard
