/*! ====================================================================================================================
  @file appmutex.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Oct.16.2011

  @brief 
    Mutual Exclusive section. Implements locking mechanism using critical section.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUSFW_APPMUTEX_H
#define VIRTUSFW_APPMUTEX_H

#include "appmutex.h"

namespace Grimelins
{

    class Mutex
    {
      friend class Lock;

    public: // aut
      Mutex  (void) { InitializeCriticalSection(&mCrit); }
      ~Mutex (void) { DeleteCriticalSection(&mCrit);     }

    public: // met
      void Acquire (void) { EnterCriticalSection(&mCrit); }
      void Release (void) { LeaveCriticalSection(&mCrit); }

    private: // dat
      CRITICAL_SECTION mCrit;
    };
} // namespace Virtus

#endif // Include Guard

