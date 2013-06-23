/*! ====================================================================================================================
  @file jacobi1x6.inl

  Codename Grimelins

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Nov.18.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
namespace Grimelins
{
  JAC6INLINE Jacobi1x6::Jacobi1x6()
    : a(0.0f),b(0.0f),c(0.0f),d(0.0f),e(0.0f),f(0.0f)
  {
  }

  JAC6INLINE Jacobi1x6::Jacobi1x6(f32 aa, f32 bb, f32 cc, f32 dd, f32 ee, f32 ff)
    : a(aa), b(bb), c(cc), d(dd), e(ee), f(ff)
  {
  }

  JAC6INLINE Jacobi1x6& Jacobi1x6::operator=(const Jacobi1x6& mat)
  {
    if( this != &mat )
      std::memcpy( v, mat.v, sizeof(v) );

    return *this;
  }

  JAC6INLINE Jacobi1x6 Jacobi1x6::operator+(const Jacobi1x6& mat) const
  {
    return Jacobi1x6(a + mat.a, b + mat.b, c + mat.c, d + mat.d, e + mat.e, f + mat.f);
  }

  JAC6INLINE Jacobi1x6 Jacobi1x6::operator-(const Jacobi1x6& mat) const
  {
    return Jacobi1x6(a - mat.a, b - mat.b, c - mat.c, d - mat.d, e - mat.e, f - mat.f);
  }

  JAC6INLINE Jacobi1x6 Jacobi1x6::operator*(f32 value) const
  {
    return Jacobi1x6(a * value, b * value, c * value, d * value, e * value, f * value);
  }

  JAC6INLINE Jacobi1x6 Jacobi1x6::operator*(const Matrix6& mat) const
  {
    return Jacobi1x6(a*mat.m00 + b*mat.m01 + c*mat.m02 + d*mat.m03 + e*mat.m04 + f*mat.m05,
                     a*mat.m06 + b*mat.m07 + c*mat.m08 + d*mat.m09 + e*mat.m10 + f*mat.m11,
                     a*mat.m12 + b*mat.m13 + c*mat.m14 + d*mat.m15 + e*mat.m16 + f*mat.m17,
                     a*mat.m18 + b*mat.m19 + c*mat.m20 + d*mat.m21 + e*mat.m22 + f*mat.m23,
                     a*mat.m24 + b*mat.m25 + c*mat.m26 + d*mat.m27 + e*mat.m28 + f*mat.m29,
                     a*mat.m30 + b*mat.m31 + c*mat.m32 + d*mat.m33 + e*mat.m34 + f*mat.m35);
  }

  JAC6INLINE f32 Jacobi1x6::operator*(const Vector6& vec) const
  {
    return (a * vec.a + b * vec.b + c * vec.c + d * vec.d + e* vec.e + f * vec.f);
  }

  JAC6INLINE Vector6 Jacobi1x6::transpose() const
  {
    return Vector6(a,b,c,d,e,f);
  }

  JAC6INLINE Jacobi1x6 operator*(f32 value, const Jacobi1x6& mat)
  {
    return mat * value;
  }

}; // namespace Grimelins