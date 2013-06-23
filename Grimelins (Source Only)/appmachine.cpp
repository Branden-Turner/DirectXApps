/*! ====================================================================================================================
  @file appmachine.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 16.Sep.2011

  @brief 
    Machine specific abstraction

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"

namespace Grimelins
{
  namespace Application
  {
   
    u32 GetNumberProcessors(void)
    {
      SYSTEM_INFO inf;
      GetSystemInfo(&inf);

      return inf.dwNumberOfProcessors;      
    }

  } // namespace Application
} // namespace Virtus