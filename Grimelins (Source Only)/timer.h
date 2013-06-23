/*! ====================================================================================================================
  @file timer.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.02.2011

  @brief 
    High precision timer

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_TIMER_H
#define VIRTUS_TIMER_H

namespace Grimelins
{

  /////////// Timer ////////////
  // [May.25.2011 Joseph Busch]
  class Timer
  {
  public: // Aut
    Timer(bool start = false);

  public: // Meth
    void StartTimer(void);
    f32 Get(void) const;
    f64 Get64(void) const;

  private: // Dat
    f64 secondsPerCount;
    u64 countsPerSecond;
    u64 prevTimeStamp;
    mutable u64 currTimeStamp;
  };

  class FrameTimer
  {
  public: // Aut
    FrameTimer(bool start = false);

  public: // Meth
    void StartTimer(void);
    f32 Get(void) const;

  private: // Dat
    u32 prevFrameStamp;
  };
} // namespace Virtus



#endif // Include Guard