/*! ====================================================================================================================
  @file timer.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.25.2011

  @brief 
    Implementation of time related classes and functionality.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "apputilities.h"
#include "timer.h"

namespace Grimelins
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // class Timer
  // 

  // if (x==1)
 // {
  //  doResume("robot")
  //}
 

  Timer::Timer(bool start) : secondsPerCount(0.0f), 
                             countsPerSecond(0), 
                             prevTimeStamp(0),
                             currTimeStamp(0)
  {
    Application::QueryPerformanceFrequency(RCAST(lInt*, &countsPerSecond));
    secondsPerCount = 1.0 / SCAST(f64, countsPerSecond);

    if(start)
      StartTimer();
  }

  void Timer::StartTimer(void)
  {
    prevTimeStamp = 0;
    Application::QueryPerformanceCounter(RCAST(lInt*, &prevTimeStamp));
  }

  f32 Timer::Get(void) const
  {
    currTimeStamp = 0;
    Application::QueryPerformanceCounter(RCAST(lInt*, &currTimeStamp));
    return SCAST(f32, SCAST(f64, currTimeStamp - prevTimeStamp)*secondsPerCount); 
  }

  f64 Timer::Get64(void) const
  { 
    currTimeStamp = 0;
    Application::QueryPerformanceCounter(RCAST(lInt*, &currTimeStamp));
    return SCAST(f64, currTimeStamp - prevTimeStamp)*secondsPerCount; 
  }



  FrameTimer::FrameTimer( bool start)
  : prevFrameStamp(0)
  {
    if(start) StartTimer();
  }

  void FrameTimer::StartTimer( void )
  {
    prevFrameStamp = gCore->GetFrameCount();
  }

  Grimelins::f32 FrameTimer::Get( void ) const
  {
    return SCAST(float, gCore->GetFrameCount() - prevFrameStamp) / 60.0f;
  }

} // namespace Virtus