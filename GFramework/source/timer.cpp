#include "precompiled.h"

namespace cs350
{
  Timer::Timer()
    : secondsPerCount(0.0), dt(-1.0), baseTime(0), pausedTime(0), 
      prevTime(0), currTime(0), stopped(false)
  {
    __int64 countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    secondsPerCount = 1.0 / static_cast<double>(countsPerSec);
  }

  Timer::~Timer()
  {
  }

  float Timer::getGameTime() const
  {
    //Don't count time since we stopped
    if(stopped)
      return static_cast<float>((stopTime - baseTime) * secondsPerCount);
    //And take out any time that we were paused for.
    else
      return static_cast<float>(((currTime - pausedTime) - baseTime) * secondsPerCount);
  }

  float Timer::getTimeElapsed() const
  {
    return static_cast<float>(dt);
  }

  void Timer::reset()
  {
    __int64 currentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
    baseTime = currentTime;
    prevTime = currentTime;
    stopTime = 0;
    stopped = false;
  }

  void Timer::start()
  {
    __int64 startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

    //Starting back up again?
    if(stopped)
    {
      pausedTime += startTime - stopTime;
      prevTime = startTime;
      stopTime = 0;
      stopped = false;
    }
  }

  void Timer::stop()
  {
    //record the stopping time
    if(!stopped)
    {
      QueryPerformanceCounter((LARGE_INTEGER*)&stopTime);
      stopped = true;
    }
  }

  void Timer::tick()
  {
    //No time elapsed
    if(stopped)
    {
      dt = 0.0;
      return;
    }

    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

    dt = (currTime - prevTime) * secondsPerCount; 

    prevTime = currTime;

    //Don't let our dt go negative.
    if(dt < 0.0)
      dt = 0.0;
  }

}

