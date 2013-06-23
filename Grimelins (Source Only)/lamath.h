/*! ====================================================================================================================
  @file math.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.14.2011

  @brief 
    Virtus internal math library.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_MATH_H
#define VIRTUS_MATH_H

#include <math.h>
#include <algorithm>

#undef min
#undef max

namespace Grimelins
{
  static const f32 PI = 3.1415926535897932384626433832795f; // PI to a ridiculous length
  static const f32 PITWO = PI * 2.0f;

  static const f32 fourthPI = PI / 4.0f;
  static const f32 halfPI = PI / 2.0f;
  static const f32 halfturnPI = PI * 180.0f;
  static const f32 toRad = PITWO/360.0f;
  static const f32 toDeg = 360.0f/PITWO;
  static const f32 epsilon = 0.00001f;                    //< floating point epsilon for single precision.
  static const f32 epsilonSquared = epsilon * epsilon;    //< epsilon value squared

  // function to test if two real numbers are (almost) equal
  // We test if two numbers a and b are such that (a-b) are in [-EPSILON; EPSILON]
  inline bool approxEqual(f32 a, f32 b) {
    f32 difference = a - b;
    return (difference < epsilon && difference > -epsilon);
  }
  
  template <typename T>
   T clampOut(T const& v, T const& left, T const& right)
  {
    if(v>left && v<right)
    {
      T half;

      if(right < 0)
        half = (-right +left)/(T)2.0 + right;
      else
        half = (right - left)/(T)2.0 + left;

      if(v < half)
        return left;
      else
        return right;
    }
    return v;
  }

  template <typename T>
  inline T clamp(T const& v, T const& low, T const& high)
  {
    return std::max(std::min(high, v), low);
  }

  class MathException
  {
  public:
    enum ErrorCode
    {
      PRIME_SEARCH_INVALID_ELEMENTS,
      PRIME_SEARCH_UNSUFFNUM_PRIMES
    };

    MathException(ErrorCode code) : m_Error(code) {}
    ErrorCode What(void) const { return m_Error; }

  private:
    ErrorCode m_Error;
  };


  template <typename T>
  T INFINITY(void);

#define CLAMPED(Min, Max, Val) std::min(Max, std::max(Min, Val))
#define fequal(val1, val2) (fabs(val1 - val2) < epsilon)



} // namespace virtus

#endif // Include Guard

#include "lamath.inl"