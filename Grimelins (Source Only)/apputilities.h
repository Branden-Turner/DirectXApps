/*! ====================================================================================================================
  @file osutilities.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.04.2011

  @brief 
    Operating system utitlities wrapper.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_APPUTILITIES_H
#define VIRTUS_APPUTILITIES_H

#include "appdat.h"

namespace Grimelins
{
  namespace Application
  { 

    // Dialogs on OS level
    void FullStopError( ch const* message );
    u1   YesNoQuestion( ch const* question );

    // Context related
    void Sleep( f32 seconds );
    u1 QueryPerformanceFrequency( lInt* frequency );
    u1 QueryPerformanceCounter( lInt* counter );

    // Memory Related
    void* aligned_malloc( size_t size, size_t alignment );
    void aligned_free( void* data );

    // OS Information
    bool IsOnlyInstance( char const* title );
    bool HardDiskSpaceAvailable( u64 spaceRequired );
    u1 GetProductAndVersion(std::string& strProductName, std::string& strProductVersion);


    // Error Handling
    std::string GetLastError( ch const* lpszFunction );

    // Libraries
    OSMODULE LoadLib( std::string filename );
    OSFARPROC GetFunctionAddress(OSMODULE inst, std::string const& fn);

    // Threading
    void CloseHandle(OSHANDLE handle);
    OSHANDLE MakeThread(ThreadProc fn, void* data, u1 suspendedStart = false, u32* idDescriptor = NULL);
    void WaitForThread(OSHANDLE Thread, f32 time = 0.f);
    void KillThread(OSHANDLE Thread, s32 code = 0);
    u1 PauseThread(OSHANDLE Thread);
    u1 UnPauseThread(OSHANDLE Thread);
    void NameThread(u32 id, ch const* name);

  } // namespace Application
} // namespace Virtus

#endif // Include Guard