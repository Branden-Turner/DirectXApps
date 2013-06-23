/*! ====================================================================================================================
  @file watchdog.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.15.2011

  @brief 
    The watchdog for detecting crashes and writing minidumps to file.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_CRASHHANDLER_H
#define VIRTUS_CRASHHANDLER_H
#include "minidump.h"

namespace Grimelins
{
    
    class WatchDog
    {
    public: // Autos.
      WatchDog(void);
      ~WatchDog(void);

    public:
      static LONG WINAPI ExecuteHandler(EXCEPTION_POINTERS* pException);

    private: // Data.
      static WatchDog* mInstance;
      static MiniDumpWriter mDumpWriter;

    };
} // namespace Virtus

#endif // Include Guard

