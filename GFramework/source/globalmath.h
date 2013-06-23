#ifndef MATH_H
#define MATH_H

namespace cs350
{
  static const float PI = 3.1415926535897932384626433832795f; // PI to a ridiculous length
  static const float PITWO = PI * 2.0f;

  static const float fourthPI = PI / 4.0f;
  static const float halfPI = PI / 2.0f;
  static const float halfturnPI = PI * 180.0f;
  static const float toRad = PITWO/360.0f;
  static const float toDeg = 360.0f/PITWO;
  static const float epsilon = 0.001f;                    //< floating point epsilon for single precision.
  static const float epsilonSquared = epsilon * epsilon;    //< epsilon value squared

  // function to test if two real numbers are (almost) equal
  // We test if two numbers a and b are such that (a-b) are in [-EPSILON; EPSILON]
  inline bool approxEqual(float a, float b)
  {
    float difference = a - b;
    return (difference < epsilon && difference > -epsilon);
  }

  inline void clamp( float& clamped, const float& minimum, const float& maximum)
  {
    clamped = min(max(clamped, minimum), maximum);
  }
}

#endif