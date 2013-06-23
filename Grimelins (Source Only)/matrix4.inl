/*! ====================================================================================================================
  @file matrix4.inl

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.16.2011

  @brief
    Matrix Inline functions

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

namespace Grimelins {

// Constructors
MTX4INLINE Matrix4::Matrix4(void) {
  std::memcpy( v, mMatrix4Identity.v, sizeof(v) );
}

MTX4INLINE Matrix4::Matrix4(Matrix4 const& rhs) {
  std::memcpy( v, rhs.v, sizeof(v) );
}


MTX4INLINE Matrix4::Matrix4( const ScriptMatrix4& rhs ){
  std::memcpy( v, rhs.v, sizeof(v) );
}


MTX4INLINE Matrix4::Matrix4(f32 mm00, f32 mm01, f32 mm02, f32 mm03,
                            f32 mm10, f32 mm11, f32 mm12, f32 mm13,
                            f32 mm20, f32 mm21, f32 mm22, f32 mm23,
                            f32 mm30, f32 mm31, f32 mm32, f32 mm33)
                            : m00(mm00), m01(mm01), m02(mm02), m03(mm03),
                              m10(mm10), m11(mm11), m12(mm12), m13(mm13),
                              m20(mm20), m21(mm21), m22(mm22), m23(mm23),
                              m30(mm30), m31(mm31), m32(mm32), m33(mm33) 
{
}

MTX4INLINE Vector4 Matrix4::operator*( Vector4 const& rhs ) const
{
  return Vector4(
    m00 * rhs.x + m01 * rhs.y + m02 * rhs.z + m03 * rhs.w, 
    m10 * rhs.x + m11 * rhs.y + m12 * rhs.z + m13 * rhs.w, 
    m20 * rhs.x + m21 * rhs.y + m22 * rhs.z + m23 * rhs.w, 
    m30 * rhs.x + m31 * rhs.y + m32 * rhs.z + m33 * rhs.w);
}

MTX4INLINE Vector3 Matrix4::MulVec( Vector3 const& rhs ) const
{
  return Vector3(
    m00 * rhs.x + m01 * rhs.y + m02 * rhs.z, 
    m10 * rhs.x + m11 * rhs.y + m12 * rhs.z, 
    m20 * rhs.x + m21 * rhs.y + m22 * rhs.z);
}

MTX4INLINE Vector3 Matrix4::MulPnt( Vector3 const& rhs ) const
{
  return Vector3(
    m00 * rhs.x + m01 * rhs.y + m02 * rhs.z + m03, 
    m10 * rhs.x + m11 * rhs.y + m12 * rhs.z + m13, 
    m20 * rhs.x + m21 * rhs.y + m22 * rhs.z + m23);
}

MTX4INLINE Matrix4& Matrix4::operator=( Matrix4 const& rhs )
{
  if(this != &rhs)
    std::memcpy(v, rhs.v, sizeof(v));

  return (*this);
}

MTX4INLINE Matrix4 Matrix4::operator+( Matrix4 const& rhs ) const
{
  return Matrix4(
    m00 + rhs.m00, m01 + rhs.m01, m02 + rhs.m02, m03 + rhs.m03,
    m10 + rhs.m10, m11 + rhs.m11, m12 + rhs.m12, m13 + rhs.m13,
    m20 + rhs.m20, m21 + rhs.m21, m22 + rhs.m22, m23 + rhs.m23,
    m30 + rhs.m30, m31 + rhs.m31, m32 + rhs.m32, m33 + rhs.m33);
}

MTX4INLINE Matrix4 Matrix4::operator-(const Matrix4& rhs) const
{
  return Matrix4(
    m00 - rhs.m00, m01 - rhs.m01, m02 - rhs.m02, m03 - rhs.m03,
    m10 - rhs.m10, m11 - rhs.m11, m12 - rhs.m12, m13 - rhs.m13,
    m20 - rhs.m20, m21 - rhs.m21, m22 - rhs.m22, m23 - rhs.m23,
    m30 - rhs.m30, m31 - rhs.m31, m32 - rhs.m32, m33 - rhs.m33);
}

MTX4INLINE Matrix4 Matrix4::operator*(const Matrix4& rhs) const
{
  return Matrix4(
    m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20 + m03 * rhs.m30, 
    m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21 + m03 * rhs.m31, 
    m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22 + m03 * rhs.m32, 
    m00 * rhs.m03 + m01 * rhs.m13 + m02 * rhs.m23 + m03 * rhs.m33, 

    m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20 + m13 * rhs.m30,
    m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31, 
    m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32,
    m10 * rhs.m03 + m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33, 

    m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20 + m23 * rhs.m30,
    m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31, 
    m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32, 
    m20 * rhs.m03 + m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33, 

    m30 * rhs.m00 + m31 * rhs.m10 + m32 * rhs.m20 + m33 * rhs.m30,
    m30 * rhs.m01 + m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31, 
    m30 * rhs.m02 + m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32, 
    m30 * rhs.m03 + m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33);
}

MTX4INLINE Matrix4& Matrix4::operator+=(const Matrix4& rhs) 
{
  m00 += rhs.m00;
  m01 += rhs.m01;
  m02 += rhs.m02;
  m03 += rhs.m03;
  m10 += rhs.m10;
  m11 += rhs.m11;
  m12 += rhs.m12;
  m13 += rhs.m13;
  m20 += rhs.m20;
  m21 += rhs.m21;
  m22 += rhs.m22;
  m23 += rhs.m23;
  m30 += rhs.m30;
  m31 += rhs.m31;
  m32 += rhs.m32;
  m33 += rhs.m33;

  return (*this);
}

MTX4INLINE Matrix4& Matrix4::operator-=(const Matrix4& rhs) 
{
  m00 -= rhs.m00;
  m01 -= rhs.m01;
  m02 -= rhs.m02;
  m03 -= rhs.m03;
  m10 -= rhs.m10;
  m11 -= rhs.m11;
  m12 -= rhs.m12;
  m13 -= rhs.m13;
  m20 -= rhs.m20;
  m21 -= rhs.m21;
  m22 -= rhs.m22;
  m23 -= rhs.m23;
  m30 -= rhs.m30;
  m31 -= rhs.m31;
  m32 -= rhs.m32;
  m33 -= rhs.m33;

  return (*this);
}

MTX4INLINE Matrix4& Matrix4::operator*=(const Matrix4& rhs) 
{
  return (*this) = (*this) * rhs;
}

MTX4INLINE Matrix4 Matrix4::Transposed(void) const
{
  return Matrix4(m00, m10, m20, m30,
                 m01, m11, m21, m31,
                 m02, m12, m22, m32,
                 m03, m13, m23, m33);
}


MTX4INLINE Matrix4 Matrix4::Inverse(void) const
{
  Matrix4 inv(*this);
  f64 det;
  Matrix4 ret;
  ret.v[0]  =  v[5]*v[10]*v[15] - v[5]*v[11]*v[14] - v[9]*v[6]*v[15] + v[9]*v[7]*v[14] + v[13]*v[6]*v[11] - v[13]*v[7]*v[10];
  ret.v[4]  = -v[4]*v[10]*v[15] + v[4]*v[11]*v[14] + v[8]*v[6]*v[15] - v[8]*v[7]*v[14] - v[12]*v[6]*v[11] + v[12]*v[7]*v[10];
  ret.v[8]  =  v[4]*v[9] *v[15] - v[4]*v[11]*v[13] - v[8]*v[5]*v[15] + v[8]*v[7]*v[13] + v[12]*v[5]*v[11] - v[12]*v[7]*v[9];
  ret.v[12] = -v[4]*v[9] *v[14] + v[4]*v[10]*v[13] + v[8]*v[5]*v[14] - v[8]*v[6]*v[13] - v[12]*v[5]*v[10] + v[12]*v[6]*v[9];
  ret.v[1]  = -v[1]*v[10]*v[15] + v[1]*v[11]*v[14] + v[9]*v[2]*v[15] - v[9]*v[3]*v[14] - v[13]*v[2]*v[11] + v[13]*v[3]*v[10];
  ret.v[5]  =  v[0]*v[10]*v[15] - v[0]*v[11]*v[14] - v[8]*v[2]*v[15] + v[8]*v[3]*v[14] + v[12]*v[2]*v[11] - v[12]*v[3]*v[10];
  ret.v[9]  = -v[0]*v[9] *v[15] + v[0]*v[11]*v[13] + v[8]*v[1]*v[15] - v[8]*v[3]*v[13] - v[12]*v[1]*v[11] + v[12]*v[3]*v[9];
  ret.v[13] =  v[0]*v[9] *v[14] - v[0]*v[10]*v[13] - v[8]*v[1]*v[14] + v[8]*v[2]*v[13] + v[12]*v[1]*v[10] - v[12]*v[2]*v[9];
  ret.v[2]  =  v[1]*v[6] *v[15] - v[1]*v[7] *v[14] - v[5]*v[2]*v[15] + v[5]*v[3]*v[14] + v[13]*v[2]*v[7]  - v[13]*v[3]*v[6];
  ret.v[6]  = -v[0]*v[6] *v[15] + v[0]*v[7] *v[14] + v[4]*v[2]*v[15] - v[4]*v[3]*v[14] - v[12]*v[2]*v[7]  + v[12]*v[3]*v[6];
  ret.v[10] =  v[0]*v[5] *v[15] - v[0]*v[7] *v[13] - v[4]*v[1]*v[15] + v[4]*v[3]*v[13] + v[12]*v[1]*v[7]  - v[12]*v[3]*v[5];
  ret.v[14] = -v[0]*v[5] *v[14] + v[0]*v[6] *v[13] + v[4]*v[1]*v[14] - v[4]*v[2]*v[13] - v[12]*v[1]*v[6]  + v[12]*v[2]*v[5];
  ret.v[3]  = -v[1]*v[6] *v[11] + v[1]*v[7] *v[10] + v[5]*v[2]*v[11] - v[5]*v[3]*v[10] - v[9] *v[2]*v[7]  + v[9] *v[3]*v[6];
  ret.v[7]  =  v[0]*v[6] *v[11] - v[0]*v[7] *v[10] - v[4]*v[2]*v[11] + v[4]*v[3]*v[10] + v[8] *v[2]*v[7]  - v[8] *v[3]*v[6];
  ret.v[11] = -v[0]*v[5] *v[11] + v[0]*v[7] *v[9]  + v[4]*v[1]*v[11] - v[4]*v[3]*v[9]  - v[8] *v[1]*v[7]  + v[8] *v[3]*v[5];
  ret.v[15] =  v[0]*v[5] *v[10] - v[0]*v[6] *v[9]  - v[4]*v[1]*v[10] + v[4]*v[2]*v[9]  + v[8] *v[1]*v[6]  - v[8] *v[2]*v[5];
  det = v[0]*ret.v[0] + v[1]*ret.v[4] + v[2]*ret.v[8] + v[3]*ret.v[12];

  if (det == 0)
    return inv;

  det = 1.0 / det;

  for(u32 i = 0; i < 16; ++i)
    inv.v[i] = static_cast<f32>(ret.v[i] * det);

  return inv;
}

MTX4INLINE Vector3 Matrix4::GetTranslation() const
{
  return Vector3(m03, m13, m23);
}

MTX4INLINE Matrix4& Matrix4::SetTranspose(void)
{
  std::swap(m01, m10);
  std::swap(m02, m20);
  std::swap(m03, m30);
  std::swap(m12, m21);
  std::swap(m13, m31);
  std::swap(m23, m32);

  return *this;
}

MTX4INLINE Matrix4& Matrix4::SetZero(void)
{
  std::memset( v, 0, sizeof(v) );
  return *this;
}

MTX4INLINE Matrix4& Matrix4::SetIdentity(void)
{
  std::memcpy( v, mMatrix4Identity.v, sizeof(v) );
  return *this;
}

} // namespace Virtus
