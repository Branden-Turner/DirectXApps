/*! ====================================================================================================================
  @file matrix6.cpp

  Codename Grimelins

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Nov.7.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

namespace Grimelins
{
  // Constructors
  MTX6INLINE Matrix6::Matrix6(void) {
    std::memcpy( v, mMatrix6Identity.v, sizeof(v) );
  }

  MTX6INLINE Matrix6::Matrix6(Matrix6 const& rhs) {
    std::memcpy( v, rhs.v, sizeof(v) );
  }


  MTX6INLINE Matrix6::Matrix6(f32 mm00, f32 mm01, f32 mm02, f32 mm03, f32 mm04, f32 mm05,
                              f32 mm10, f32 mm11, f32 mm12, f32 mm13, f32 mm14, f32 mm15,
                              f32 mm20, f32 mm21, f32 mm22, f32 mm23, f32 mm24, f32 mm25,
                              f32 mm30, f32 mm31, f32 mm32, f32 mm33, f32 mm34, f32 mm35,
                              f32 mm40, f32 mm41, f32 mm42, f32 mm43, f32 mm44, f32 mm45,
                              f32 mm50, f32 mm51, f32 mm52, f32 mm53, f32 mm54, f32 mm55)
    : m00(mm00), m01(mm01), m02(mm02), m03(mm03), m04(mm04), m05(mm05),
      m06(mm10), m07(mm11), m08(mm12), m09(mm13), m10(mm14), m11(mm15),
      m12(mm20), m13(mm21), m14(mm22), m15(mm23), m16(mm24), m17(mm25),
      m18(mm30), m19(mm31), m20(mm32), m21(mm33), m22(mm34), m23(mm35),
      m24(mm40), m25(mm41), m26(mm42), m27(mm43), m28(mm44), m29(mm45),
      m30(mm50), m31(mm51), m32(mm52), m33(mm53), m34(mm54), m35(mm55)
  {
  }

  MTX6INLINE Vector6 Matrix6::operator*( Vector6 const& rhs ) const
  {
    return Vector6(m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3] + m[0][4] * v[4] + m[0][5] * v[5],
                   m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3] + m[1][4] * v[4] + m[1][5] * v[5],
                   m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3] + m[2][4] * v[4] + m[2][5] * v[5],
                   m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3] + m[3][4] * v[4] + m[3][5] * v[5],
                   m[4][0] * v[0] + m[4][1] * v[1] + m[4][2] * v[2] + m[4][3] * v[3] + m[4][4] * v[4] + m[4][5] * v[5],
                   m[5][0] * v[0] + m[5][1] * v[1] + m[5][2] * v[2] + m[5][3] * v[3] + m[5][4] * v[4] + m[5][5] * v[5]);
  }

  MTX6INLINE Matrix6& Matrix6::operator=( Matrix6 const& rhs )
  {
    if(this != &rhs)
      std::memcpy(v, rhs.v, sizeof(v));

    return (*this);
  }

  MTX6INLINE Matrix6 Matrix6::operator+( Matrix6 const& rhs ) const
  {
    return Matrix6(
      m00 + rhs.m00, m01 + rhs.m01, m02 + rhs.m02, m03 + rhs.m03, m04 + rhs.m04, m05 + rhs.m05,
      m06 + rhs.m06, m07 + rhs.m07, m08 + rhs.m08, m09 + rhs.m09, m10 + rhs.m10, m11 + rhs.m11,
      m12 + rhs.m12, m13 + rhs.m13, m14 + rhs.m14, m15 + rhs.m15, m16 + rhs.m16, m17 + rhs.m17,
      m18 + rhs.m18, m19 + rhs.m19, m20 + rhs.m20, m21 + rhs.m21, m22 + rhs.m22, m23 + rhs.m23,
      m24 + rhs.m24, m25 + rhs.m25, m26 + rhs.m26, m27 + rhs.m27, m28 + rhs.m28, m29 + rhs.m29,
      m30 + rhs.m30, m31 + rhs.m31, m32 + rhs.m32, m33 + rhs.m33, m34 + rhs.m34, m35 + rhs.m35);
  }

  MTX6INLINE Matrix6 Matrix6::operator-(const Matrix6& rhs) const
  {
    return Matrix6(
    m00 - rhs.m00, m01 - rhs.m01, m02 - rhs.m02, m03 - rhs.m03, m04 - rhs.m04, m05 - rhs.m05,
    m06 - rhs.m06, m07 - rhs.m07, m08 - rhs.m08, m09 - rhs.m09, m10 - rhs.m10, m11 - rhs.m11,
    m12 - rhs.m12, m13 - rhs.m13, m14 - rhs.m14, m15 - rhs.m15, m16 - rhs.m16, m17 - rhs.m17,
    m18 - rhs.m18, m19 - rhs.m19, m20 - rhs.m20, m21 - rhs.m21, m22 - rhs.m22, m23 - rhs.m23,
    m24 - rhs.m24, m25 - rhs.m25, m26 - rhs.m26, m27 - rhs.m27, m28 - rhs.m28, m29 - rhs.m29,
    m30 - rhs.m30, m31 - rhs.m31, m32 - rhs.m32, m33 - rhs.m33, m34 - rhs.m34, m35 - rhs.m35);
  }

  MTX6INLINE Matrix6 Matrix6::operator*(const f32 value) const
  {
    return Matrix6(
    m00 * value, m01 * value, m02 * value, m03 * value, m04 * value, m05 * value,
    m06 * value, m07 * value, m08 * value, m09 * value, m10 * value, m11 * value,
    m12 * value, m13 * value, m14 * value, m15 * value, m16 * value, m17 * value,
    m18 * value, m19 * value, m20 * value, m21 * value, m22 * value, m23 * value,
    m24 * value, m25 * value, m26 * value, m27 * value, m28 * value, m29 * value,
    m30 * value, m31 * value, m32 * value, m33 * value, m34 * value, m35 * value);
  }

  MTX6INLINE Matrix6 Matrix6::getTranspose()
  {
    Matrix6 transpose;

    transpose.m[0][0] = m[0][0]; transpose.m[0][1] = m[1][0]; transpose.m[0][2] = m[2][0];
    transpose.m[0][3] = m[3][0]; transpose.m[0][4] = m[4][0]; transpose.m[0][5] = m[5][0];
    transpose.m[1][0] = m[0][1]; transpose.m[1][1] = m[1][1]; transpose.m[1][2] = m[2][1];
    transpose.m[1][3] = m[3][1]; transpose.m[1][4] = m[4][1]; transpose.m[1][5] = m[5][1];
    transpose.m[2][0] = m[0][2]; transpose.m[2][1] = m[1][2]; transpose.m[2][2] = m[2][2];
    transpose.m[2][3] = m[3][2]; transpose.m[2][4] = m[4][2]; transpose.m[2][5] = m[5][2];
    transpose.m[3][0] = m[0][3]; transpose.m[3][1] = m[1][3]; transpose.m[3][2] = m[2][3];
    transpose.m[3][3] = m[3][3]; transpose.m[3][4] = m[4][3]; transpose.m[3][5] = m[5][3];
    transpose.m[4][0] = m[0][4]; transpose.m[4][1] = m[1][4]; transpose.m[4][2] = m[2][4];
    transpose.m[4][3] = m[3][4]; transpose.m[4][4] = m[4][4]; transpose.m[4][5] = m[5][4];
    transpose.m[5][0] = m[0][5]; transpose.m[5][1] = m[1][5]; transpose.m[5][2] = m[2][5];
    transpose.m[5][3] = m[3][5]; transpose.m[5][4] = m[4][5]; transpose.m[5][5] = m[5][5];
    return transpose;
  }

}; // namespace Grimelins

