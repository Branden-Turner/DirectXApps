/*! ====================================================================================================================
  @file watchdog.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.15.2011

  @brief 
    The watchdog for detecting crashes and writing minidumps to file.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "watchdog.h"

namespace Grimelins
{
    

    //////////////////////////////////////////////////////////////////////////
    // Static Data

    WatchDog* WatchDog::mInstance = NULL;
    MiniDumpWriter WatchDog::mDumpWriter;


    //////////////////////////////////////////////////////////////////////////
    // Autos

    WatchDog::WatchDog(void)
    {
      assert(mInstance == NULL && "Only one watch dog allowed per thread");
      mInstance = this;
      SetUnhandledExceptionFilter( WatchDog::ExecuteHandler );
    }

    WatchDog::~WatchDog(void)
    {
      mInstance = NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    // Public Interface

    LONG WINAPI WatchDog::ExecuteHandler( EXCEPTION_POINTERS* pException )
    {
      mDumpWriter.DumpException( pException );
      return EXCEPTION_CONTINUE_SEARCH;
    }

} // namespace Virtus

