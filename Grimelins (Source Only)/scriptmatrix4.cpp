/*! ====================================================================================================================
  @file ScriptMatrix4.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.16.2011

  @brief
    ScriptMatrix4x4 Library of Awesome

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "scriptmatrix4.h"

namespace Grimelins {

SCRIPT_ClassStart(ScriptMatrix4);
SCRIPT_CTOR_Enabled();
SCRIPT_PlainData(m00);
SCRIPT_PlainData(m01);
SCRIPT_PlainData(m02);
SCRIPT_PlainData(m03);
SCRIPT_PlainData(m10);
SCRIPT_PlainData(m11);
SCRIPT_PlainData(m12);
SCRIPT_PlainData(m13);
SCRIPT_PlainData(m20);
SCRIPT_PlainData(m21);
SCRIPT_PlainData(m22);
SCRIPT_PlainData(m23);
SCRIPT_PlainData(m30);
SCRIPT_PlainData(m31);
SCRIPT_PlainData(m32);
SCRIPT_PlainData(m33);
SCRIPT_Method(SetTranspose);
SCRIPT_Method(SetZero);
SCRIPT_Method(SetIdentity);
SCRIPT_Method(Transposed);
SCRIPT_Method(Inverse);
SCRIPT_Method(GetTranslation);
SCRIPT_Method(MulVec);
SCRIPT_Method(MulPnt);
SCRIPT_Method(GetIdentity);
SCRIPT_Method(RotationX);
SCRIPT_Method(RotationY);
SCRIPT_Method(RotationZ);
SCRIPT_Method(RotationAxis);
SCRIPT_Method(AlignVector);
SCRIPT_Method(AlignVectorTranspose);

SCRIPT_MethodOverload(operator-, "_sub", ScriptMatrix4, (const ScriptMatrix4&) const);
SCRIPT_MethodOverload(operator+, "_add", ScriptMatrix4, (const ScriptMatrix4&) const);
SCRIPT_MethodOverload(operator*, "_mul", ScriptMatrix4, (const ScriptMatrix4&) const);

SCRIPT_MethodOverload(operator-=, "subWith", ScriptMatrix4&, (const ScriptMatrix4&) );
SCRIPT_MethodOverload(operator+=, "addWith", ScriptMatrix4&, (const ScriptMatrix4&) );
SCRIPT_MethodOverload(operator*=, "mulWith", ScriptMatrix4&, (const ScriptMatrix4&) );

SCRIPT_MethodOverload(operator*, "mulV", ScriptVec4, (const ScriptVec4& rhs) const);
SCRIPT_MethodOverload(Scale, "uniformScale", ScriptMatrix4, (f32 uniformScale));
SCRIPT_MethodOverload(Scale, "floatScale", ScriptMatrix4, (f32 xx, f32 yy, f32 zz));
SCRIPT_MethodOverload(Scale, "vecScale", ScriptMatrix4, ( ScriptVec3 const& sca ));

SCRIPT_MethodOverload(Translation, "vecTrans", ScriptMatrix4, ( ScriptVec3 const& pos ));
SCRIPT_MethodOverload(Translation, "floatTrans", ScriptMatrix4, (f32 xx, f32 yy, f32 zz));


SCRIPT_ClassEnd();

const ScriptMatrix4& ScriptMatrix4::GetIdentity( void )
{
  return mScriptMatrix4Identity;
}

ScriptMatrix4 ScriptMatrix4::Scale( f32 scal )
{
  return ScriptMatrix4(scal, 0.0f, 0.0f, 0.0f,
                       0.0f, scal, 0.0f, 0.0f,
                       0.0f, 0.0f, scal, 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f);
}

ScriptMatrix4 ScriptMatrix4::Scale( f32 xxxx, f32 yyyy, f32 zzzz )
{
  return ScriptMatrix4(xxxx, 0.0f, 0.0f, 0.0f,
                 0.0f, yyyy, 0.0f, 0.0f,
                 0.0f, 0.0f, zzzz, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

Grimelins::ScriptMatrix4 ScriptMatrix4::Scale( ScriptVec3 const& sc )
{
  return ScriptMatrix4(sc.x, 0.0f, 0.0f, 0.0f,
                 0.0f, sc.y, 0.0f, 0.0f,
                 0.0f, 0.0f, sc.z, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

ScriptMatrix4 ScriptMatrix4::Translation( f32 xxxx, f32 yyyy, f32 zzzz )
{
  return ScriptMatrix4(1.0f, 0.0f, 0.0f, xxxx,
                 0.0f, 1.0f, 0.0f, yyyy,
                 0.0f, 0.0f, 1.0f, zzzz,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

Grimelins::ScriptMatrix4 ScriptMatrix4::Translation( ScriptVec3 const& po )
{
  return ScriptMatrix4(1.0f, 0.0f, 0.0f, po.x,
                 0.0f, 1.0f, 0.0f, po.y,
                 0.0f, 0.0f, 1.0f, po.z,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

ScriptMatrix4 ScriptMatrix4::RotationX( f32 angle )
{
  float cosa = cosf(angle);
  float sina = sinf(angle);

  return ScriptMatrix4(1.0f, 0.0f,  0.0f, 0.0f,
                 0.0f, cosa, -sina, 0.0f,
                 0.0f, sina,  cosa, 0.0f,
                 0.0f, 0.0f,  0.0f, 1.0f);
}

ScriptMatrix4 ScriptMatrix4::RotationY( f32 angle )
{
  float cosa = cosf(angle);
  float sina = sinf(angle);

  return ScriptMatrix4( cosa, 0.0f, sina, 0.0f,
                  0.0f, 1.0f, 0.0f, 0.0f,
                 -sina, 0.0f, cosa, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f);
}

ScriptMatrix4 ScriptMatrix4::RotationZ( f32 angle )
{
  float cosa = cosf(angle);
  float sina = sinf(angle);

  return ScriptMatrix4(cosa, -sina, 0.0f, 0.0f,
                 sina,  cosa, 0.0f, 0.0f,
                 0.0f,  0.0f, 1.0f, 0.0f,
                 0.0f,  0.0f, 0.0f, 1.0f);
}

ScriptMatrix4 ScriptMatrix4::RotationAxis( const ScriptVec3& vec, f32 angle, const ScriptVec3& pos )
{
  const ScriptVec3& axis = vec.Normalized();
  float cosa = cosf(angle);
  float sina = sinf(angle);
  float cosm = 1.0f - cosa;

  return ScriptMatrix4( (axis.x * axis.x * cosm) + cosa, 
                  (axis.x * axis.y * cosm) - (axis.z * sina), 
                  (axis.x * axis.z * cosm) + (axis.y * sina),
                  0.0f,
                  (axis.y * axis.x * cosm) + (axis.z * sina),
                  (axis.y * axis.y * cosm) + cosa,
                  (axis.y * axis.z * cosm) - (axis.x * sina),
                  0.0f,
                  (axis.z * axis.x * cosm) - (axis.y * sina),
                  (axis.z * axis.y * cosm) + (axis.x * sina),
                  (axis.z * axis.z * cosm) + cosa,
                  0.0f, 
                  0.0f, 0.0f, 0.0f, 1.0f );

}

ScriptMatrix4 ScriptMatrix4::AlignVector( const ScriptVec3& ri, const ScriptVec3& up, const ScriptVec3& di )
{
  return ScriptMatrix4( ri.x, up.x, di.x, 0.0f,
                  ri.y, up.y, di.y, 0.0f,
                  ri.z, up.z, di.z, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f );
}

ScriptMatrix4 ScriptMatrix4::AlignVectorTranspose( const ScriptVec3& ri, const ScriptVec3& up, const ScriptVec3& di )
{
  return ScriptMatrix4( ri.x, ri.y, ri.z, 0.0f,
                  up.x, up.y, up.z, 0.0f,
                  di.x, di.y, di.z, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f );
}

ScriptMatrix4 ScriptMatrix4::AlignVectorCamera( const ScriptVec3& ri, const ScriptVec3& up, const ScriptVec3& di, const ScriptVec3& ey )
{
  return ScriptMatrix4(  ri.x,  ri.y,  ri.z, -ri.Dot(ey),
                   up.x,  up.y,  up.z, -up.Dot(ey),
                  -di.x, -di.y, -di.z, -di.Dot(ey),
                   0.0f,  0.0f,  0.0f,  1.0f );
}


ScriptMatrix4 ScriptMatrix4::LookAtMatrix( const ScriptVec3& eye, const ScriptVec3& pos, const ScriptVec3& up )
{
  ScriptVec3& dir = (eye - pos).Normalize();
  ScriptVec3& rig = up.Cross( dir ).Normalize();

    // correct the up-vector
  ScriptVec3& upf = dir.Cross( rig ).Normalize();

  return ScriptMatrix4( rig.x, rig.y, rig.z, -rig.Dot(eye),
                  upf.x, upf.y, upf.z, -upf.Dot(eye),
                  dir.x, dir.y, dir.z, -dir.Dot(eye),
                  0.0f,  0.0f,  0.0f,   1.0f );
}

  // Assumes half FOV and in radians
ScriptMatrix4 ScriptMatrix4::PerspectiveFOV( f32 aspect, f32 fov, f32 nearP, f32 farP )
{
  const f32 uppr = 1.0f / tanf(fov);
  const f32 lowr = aspect * uppr;
  const f32 denm = 1.0f / (farP - nearP);
  const f32 aaaa = (-nearP - farP) * denm;
  const f32 bbbb = (-2.0f * nearP * farP) * denm;

  return ScriptMatrix4(uppr, 0.0f,  0.0f, 0.0f,
                 0.0f, lowr,  0.0f, 0.0f,
                 0.0f, 0.0f,  aaaa, bbbb,
                 0.0f, 0.0f, -1.0f, 0.0f);
}

Grimelins::ScriptMatrix4 ScriptMatrix4::Orthographic( f32 width, f32 height, f32 nearP, f32 farP )
{
  const f32 uppr = 2.0f / width;
  const f32 lowr = 2.0f / height;
  const f32 aaaa = 1.0f / (nearP - farP);
  const f32 bbbb = nearP * aaaa;
  return ScriptMatrix4(uppr, 0.0f, 0.0f, 0.0f,
                 0.0f, lowr, 0.0f, 0.0f,
                 0.0f, 0.0f, aaaa, bbbb,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

// Constructors
 ScriptMatrix4::ScriptMatrix4(void) {
  std::memcpy( v, mScriptMatrix4Identity.v, sizeof(v) );
}

 ScriptMatrix4::ScriptMatrix4(ScriptMatrix4 const& rhs) {
  std::memcpy( v, rhs.v, sizeof(v) );
}

 ScriptMatrix4::ScriptMatrix4(f32 mm00, f32 mm01, f32 mm02, f32 mm03,
  f32 mm10, f32 mm11, f32 mm12, f32 mm13,
  f32 mm20, f32 mm21, f32 mm22, f32 mm23,
  f32 mm30, f32 mm31, f32 mm32, f32 mm33)
  : m00(mm00), m01(mm01), m02(mm02), m03(mm03),
  m10(mm10), m11(mm11), m12(mm12), m13(mm13),
  m20(mm20), m21(mm21), m22(mm22), m23(mm23),
  m30(mm30), m31(mm31), m32(mm32), m33(mm33) 
{
}

 ScriptMatrix4::ScriptMatrix4( const Matrix4& rhs ){
   std::memcpy( v, rhs.v, sizeof(v) );
 }

 ScriptVec4 ScriptMatrix4::operator*( ScriptVec4 const& rhs ) const
{
  return ScriptVec4(
    m00 * rhs.x + m01 * rhs.y + m02 * rhs.z + m03 * rhs.w, 
    m10 * rhs.x + m11 * rhs.y + m12 * rhs.z + m13 * rhs.w, 
    m20 * rhs.x + m21 * rhs.y + m22 * rhs.z + m23 * rhs.w, 
    m30 * rhs.x + m31 * rhs.y + m32 * rhs.z + m33 * rhs.w);
}

 ScriptVec3 ScriptMatrix4::MulVec( ScriptVec3 const& rhs ) const
{
  return ScriptVec3(
    m00 * rhs.x + m01 * rhs.y + m02 * rhs.z, 
    m10 * rhs.x + m11 * rhs.y + m12 * rhs.z, 
    m20 * rhs.x + m21 * rhs.y + m22 * rhs.z);
}

 ScriptVec3 ScriptMatrix4::MulPnt( ScriptVec3 const& rhs ) const
{
  return ScriptVec3(
    m00 * rhs.x + m01 * rhs.y + m02 * rhs.z + m03, 
    m10 * rhs.x + m11 * rhs.y + m12 * rhs.z + m13, 
    m20 * rhs.x + m21 * rhs.y + m22 * rhs.z + m23);
}

 ScriptMatrix4& ScriptMatrix4::operator=( ScriptMatrix4 const& rhs )
{
  if(this != &rhs)
    std::memcpy(v, rhs.v, sizeof(v));

  return (*this);
}

 ScriptMatrix4 ScriptMatrix4::operator+( ScriptMatrix4 const& rhs ) const
{
  return ScriptMatrix4(
    m00 + rhs.m00, m01 + rhs.m01, m02 + rhs.m02, m03 + rhs.m03,
    m10 + rhs.m10, m11 + rhs.m11, m12 + rhs.m12, m13 + rhs.m13,
    m20 + rhs.m20, m21 + rhs.m21, m22 + rhs.m22, m23 + rhs.m23,
    m30 + rhs.m30, m31 + rhs.m31, m32 + rhs.m32, m33 + rhs.m33);
}

 ScriptMatrix4 ScriptMatrix4::operator-(const ScriptMatrix4& rhs) const
{
  return ScriptMatrix4(
    m00 - rhs.m00, m01 - rhs.m01, m02 - rhs.m02, m03 - rhs.m03,
    m10 - rhs.m10, m11 - rhs.m11, m12 - rhs.m12, m13 - rhs.m13,
    m20 - rhs.m20, m21 - rhs.m21, m22 - rhs.m22, m23 - rhs.m23,
    m30 - rhs.m30, m31 - rhs.m31, m32 - rhs.m32, m33 - rhs.m33);
}

 ScriptMatrix4 ScriptMatrix4::operator*(const ScriptMatrix4& rhs) const
{
  return ScriptMatrix4(
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

 ScriptMatrix4& ScriptMatrix4::operator+=(const ScriptMatrix4& rhs) 
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

 ScriptMatrix4& ScriptMatrix4::operator-=(const ScriptMatrix4& rhs) 
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

 ScriptMatrix4& ScriptMatrix4::operator*=(const ScriptMatrix4& rhs) 
{
  return (*this) = (*this) * rhs;
}

 ScriptMatrix4 ScriptMatrix4::Transposed(void) const
{
  return ScriptMatrix4(m00, m10, m20, m30,
    m01, m11, m21, m31,
    m02, m12, m22, m32,
    m03, m13, m23, m33);
}


 ScriptMatrix4 ScriptMatrix4::Inverse(void) const
{
  ScriptMatrix4 inv(*this);
  f64 det;
  ScriptMatrix4 ret;
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

 ScriptVec3 ScriptMatrix4::GetTranslation() const
{
  return ScriptVec3(m03, m13, m23);
}

 ScriptMatrix4& ScriptMatrix4::SetTranspose(void)
{
  std::swap(m01, m10);
  std::swap(m02, m20);
  std::swap(m03, m30);
  std::swap(m12, m21);
  std::swap(m13, m31);
  std::swap(m23, m32);

  return *this;
}

 ScriptMatrix4& ScriptMatrix4::SetZero(void)
{
  std::memset( v, 0, sizeof(v) );
  return *this;
}

 ScriptMatrix4& ScriptMatrix4::SetIdentity(void)
{
  std::memcpy( v, mScriptMatrix4Identity.v, sizeof(v) );
  return *this;
}

 ScriptMatrix4& ScriptMatrix4::SetMatrix( f32 mm00, f32 mm01, f32 mm02, f32 mm03, 
                                f32 mm10, f32 mm11, f32 mm12, f32 mm13, 
                                f32 mm20, f32 mm21, f32 mm22, f32 mm23, 
                                f32 mm30, f32 mm31, f32 mm32, f32 mm33 )
 {
   *this = ScriptMatrix4( mm00, mm01, mm02, mm03,
                          mm10, mm11, mm12, mm13,
                          mm20, mm21, mm22, mm23,
                          mm30, mm31, mm32, mm33 );
   return *this;
 }
} // namespace Virtus     