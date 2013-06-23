/*! ====================================================================================================================
  @file matrix4.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.16.2011

  @brief
    Matrix4x4 Library of Awesome

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

#include "precompiled.h"
#include "matrix4.h"

namespace Grimelins {

const Matrix4& Matrix4::GetIdentity( void )
{
  return mMatrix4Identity;
}

Matrix4 Matrix4::Scale( f32 scal )
{
  return Matrix4(scal, 0.0f, 0.0f, 0.0f,
                 0.0f, scal, 0.0f, 0.0f,
                 0.0f, 0.0f, scal, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::Scale( f32 xxxx, f32 yyyy, f32 zzzz )
{
  return Matrix4(xxxx, 0.0f, 0.0f, 0.0f,
                 0.0f, yyyy, 0.0f, 0.0f,
                 0.0f, 0.0f, zzzz, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

Grimelins::Matrix4 Matrix4::Scale( Vector3 const& sc )
{
  return Matrix4(sc.x, 0.0f, 0.0f, 0.0f,
                 0.0f, sc.y, 0.0f, 0.0f,
                 0.0f, 0.0f, sc.z, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::Translation( f32 xxxx, f32 yyyy, f32 zzzz )
{
  return Matrix4(1.0f, 0.0f, 0.0f, xxxx,
                 0.0f, 1.0f, 0.0f, yyyy,
                 0.0f, 0.0f, 1.0f, zzzz,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

Grimelins::Matrix4 Matrix4::Translation( Vector3 const& po )
{
  return Matrix4(1.0f, 0.0f, 0.0f, po.x,
                 0.0f, 1.0f, 0.0f, po.y,
                 0.0f, 0.0f, 1.0f, po.z,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::RotationX( f32 angle )
{
  float cosa = cosf(angle);
  float sina = sinf(angle);

  return Matrix4(1.0f, 0.0f,  0.0f, 0.0f,
                 0.0f, cosa, -sina, 0.0f,
                 0.0f, sina,  cosa, 0.0f,
                 0.0f, 0.0f,  0.0f, 1.0f);
}

Matrix4 Matrix4::RotationY( f32 angle )
{
  float cosa = cosf(angle);
  float sina = sinf(angle);

  return Matrix4( cosa, 0.0f, sina, 0.0f,
                  0.0f, 1.0f, 0.0f, 0.0f,
                 -sina, 0.0f, cosa, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::RotationZ( f32 angle )
{
  float cosa = cosf(angle);
  float sina = sinf(angle);

  return Matrix4(cosa, -sina, 0.0f, 0.0f,
                 sina,  cosa, 0.0f, 0.0f,
                 0.0f,  0.0f, 1.0f, 0.0f,
                 0.0f,  0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::RotationAxis( const Vector3& vec, f32 angle, const Vector3& pos )
{
  const Vector3& axis = vec.Normalized();
  float cosa = cosf(angle);
  float sina = sinf(angle);
  float cosm = 1.0f - cosa;

  return Matrix4( (axis.x * axis.x * cosm) + cosa, 
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

Matrix4 Matrix4::AlignVector( const Vector3& ri, const Vector3& up, const Vector3& di )
{
  return Matrix4( ri.x, up.x, di.x, 0.0f,
                  ri.y, up.y, di.y, 0.0f,
                  ri.z, up.z, di.z, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f );
}

Matrix4 Matrix4::AlignVectorTranspose( const Vector3& ri, const Vector3& up, const Vector3& di )
{
  return Matrix4( ri.x, ri.y, ri.z, 0.0f,
                  up.x, up.y, up.z, 0.0f,
                  di.x, di.y, di.z, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f );
}

Matrix4 Matrix4::AlignVectorCamera( const Vector3& ri, const Vector3& up, const Vector3& di, const Vector3& ey )
{
  return Matrix4(  ri.x,  ri.y,  ri.z, -ri.Dot(ey),
                   up.x,  up.y,  up.z, -up.Dot(ey),
                  -di.x, -di.y, -di.z, -di.Dot(ey),
                   0.0f,  0.0f,  0.0f,  1.0f );
}


Matrix4 Matrix4::LookAtMatrix( const Vector3& eye, const Vector3& pos, const Vector3& up )
{
  Vector3& dir = (eye - pos).Normalize();
  Vector3& rig = up.Cross( dir ).Normalize();

    // correct the up-vector
  Vector3& upf = dir.Cross( rig ).Normalize();

  return Matrix4( rig.x, rig.y, rig.z, -rig.Dot(eye),
                  upf.x, upf.y, upf.z, -upf.Dot(eye),
                  dir.x, dir.y, dir.z, -dir.Dot(eye),
                  0.0f,  0.0f,  0.0f,   1.0f );
}

  // Assumes half FOV and in radians
Matrix4 Matrix4::PerspectiveFOV( f32 aspect, f32 fov, f32 nearP, f32 farP )
{
  const f32 uppr = 1.0f / tanf(fov);
  const f32 lowr = aspect * uppr;
  const f32 denm = 1.0f / (farP - nearP);
  const f32 aaaa = (-nearP - farP) * denm;
  const f32 bbbb = (-2.0f * nearP * farP) * denm;

  return Matrix4(uppr, 0.0f,  0.0f, 0.0f,
                 0.0f, lowr,  0.0f, 0.0f,
                 0.0f, 0.0f,  aaaa, bbbb,
                 0.0f, 0.0f, -1.0f, 0.0f);
}

Grimelins::Matrix4 Matrix4::Orthographic( f32 width, f32 height, f32 nearP, f32 farP )
{
  const f32 uppr = 2.0f / width;
  const f32 lowr = 2.0f / height;
  const f32 aaaa = 1.0f / (nearP - farP);
  const f32 bbbb = nearP * aaaa;
  return Matrix4(uppr, 0.0f, 0.0f, 0.0f,
                 0.0f, lowr, 0.0f, 0.0f,
                 0.0f, 0.0f, aaaa, bbbb,
                 0.0f, 0.0f, 0.0f, 1.0f);
}

} // namespace Virtus