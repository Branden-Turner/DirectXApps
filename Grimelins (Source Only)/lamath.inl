/*! ====================================================================================================================
  @file lamath.inl

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.06.2012

  @brief 
    

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#undef max

namespace Grimelins {


  template <typename T>
  T INFINITY(void)
  {
    static T inf = std::numeric_limits<T>::max();
    return inf;
  }

} // namespace Grimelins