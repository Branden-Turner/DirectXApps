/*! ====================================================================================================================
  @file lock.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.20.2011

  @brief 
    Locking of mutual exclusive sections.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_LOCK_H
#define VIRTUS_LOCK_H

#include "appmutex.h"

namespace Grimelins
{
    class Lock
    {
    public:
      Lock (Mutex& mutex) : mMutex(mutex) { mMutex.Acquire(); }
      ~Lock (void) { mMutex.Release(); }

    private:
      Mutex & mMutex;
    };
}

#endif // Include Guard