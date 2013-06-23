/*! ====================================================================================================================
  @file isystem.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.23.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "isystem.h"
#include "timer.h"

namespace Grimelins
{
    ISystem::ISystem(void) : mLayer(0)
    {
    }

    //----------------------------------------------------------------------
    // Name: GetSystemDT
    // Desc: Update frame time by averaging the current time over the last
    //       five time steps
    //----------------------------------------------------------------------

    f32 ISystem::GetSystemDT()
    {
      static Timer mTimer(true);
      static u32 Index = 0;
      static f32 TimeSteps[8] = {0.0f};
      static u32 ArraySize = sizeof(TimeSteps) / sizeof(f32);
      
      Index = ( (Index + 1 ) & 7 );

      TimeSteps[Index] = mTimer.Get();
      mTimer.StartTimer();

      f32 AverageFrame = 0.0f;

      for(u32 i = 0; i < ArraySize; ++i)
        AverageFrame += TimeSteps[i];

      AverageFrame /= SCAST(f32, ArraySize);

      return AverageFrame;
    }
}; // namespace Virtus
