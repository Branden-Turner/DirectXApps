/*! ====================================================================================================================
  @file vector6.inl

  Codename Grimelins

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Nov.15.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

namespace Grimelins
{
  VEC6INLINE Vector6::Vector6(void)
    : a(0.0f), b(0.0f), c(0.0f), d(0.0f), e(0.0f), f(0.0f)
  {
  }

  VEC6INLINE Vector6::Vector6(f32 aa, f32 bb, f32 cc, f32 dd, f32 ee, f32 ff)
    : a(aa), b(bb), c(cc), d(dd), e(ee), f(ff)
  {
  }

  VEC6INLINE Vector6& Vector6::operator=(const Vector6& rhs)
  {
    if( this != &rhs )
      std::memcpy( v, rhs.v, sizeof(v) );

    return *this;
  }

  VEC6INLINE Vector6 Vector6::operator+(const Vector6& rhs) const
  {
    return Vector6(a + rhs.a, b + rhs.b, c + rhs.c, d + rhs.d, e + rhs.e, f + rhs.f);
  }

  VEC6INLINE Vector6 Vector6::operator-(const Vector6& rhs) const
  {
    return Vector6(a - rhs.a, b - rhs.b, c - rhs.c, d - rhs.d, e - rhs.e, f - rhs.f);
  }

  VEC6INLINE Vector6 Vector6::operator*(f32 rhs) const
  {
    return Vector6(a * rhs, b * rhs, c * rhs, d * rhs, e * rhs, f * rhs);
  }

}; // namespace Grimelins
