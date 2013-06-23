/**********************************************************************/
/*
  
  \file
    matrix4.cpp
  
  \author
    branden.turner
    All content © 2012 DigiPen (USA) Corporation, all rights reserved.

  \brief
    4x4 Matrix class for use in this graphics framework
    
*/
/**********************************************************************/
#include "precompiled.h"

namespace cs350
{
  const Matrix4x4& Matrix4x4::I( void )
  {
    return IdentityMtx;
  }

  Matrix4x4 Matrix4x4::Scale( float scale )
  {
    return Matrix4x4(scale, 0.0f, 0.0f, 0.0f,
                     0.0f, scale, 0.0f, 0.0f,
                     0.0f, 0.0f, scale, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f);
  }

  Matrix4x4 Matrix4x4::Scale( float x, float y, float z )
  {
    return Matrix4x4(x,    0.0f, 0.0f, 0.0f,
                     0.0f, y,    0.0f, 0.0f,
                     0.0f, 0.0f, z,    0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f);
  }

  Matrix4x4 Matrix4x4::Scale( Vector3 const& sc )
  {
    return Matrix4x4(sc.x, 0.0f, 0.0f, 0.0f,
                     0.0f, sc.y, 0.0f, 0.0f,
                     0.0f, 0.0f, sc.z, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f);
  }

  Matrix4x4 Matrix4x4::Translation( float x, float y, float z )
  {
    return Matrix4x4(1.0f, 0.0f, 0.0f, x,
                     0.0f, 1.0f, 0.0f, y,
                     0.0f, 0.0f, 1.0f, z,
                     0.0f, 0.0f, 0.0f, 1.0f);
  }

  Matrix4x4 Matrix4x4::Translation( Vector3 const& trans )
  {
    return Matrix4x4(1.0f, 0.0f, 0.0f, trans.x,
                     0.0f, 1.0f, 0.0f, trans.y,
                     0.0f, 0.0f, 1.0f, trans.z,
                     0.0f, 0.0f, 0.0f, 1.0f);
  }

  Matrix4x4 Matrix4x4::Rotation( float angle, RotationAxis axis )
  {
    float cosa = cosf(angle);
    float sina = sinf(angle);

    switch(axis)
    {
      case X:
        return Matrix4x4(1.0f, 0.0f,  0.0f, 0.0f,
                         0.0f, cosa, -sina, 0.0f,
                         0.0f, sina,  cosa, 0.0f,
                         0.0f, 0.0f,  0.0f, 1.0f);
      break;

      case Y:
        return Matrix4x4( cosa, 0.0f, sina, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          -sina, 0.0f, cosa, 0.0f,
                          0.0f, 0.0f, 0.0f, 1.0f);
      break;

      case Z:
        return Matrix4x4(cosa, -sina, 0.0f, 0.0f,
                         sina,  cosa, 0.0f, 0.0f,
                         0.0f,  0.0f, 1.0f, 0.0f,
                         0.0f,  0.0f, 0.0f, 1.0f);
      break;

      default:
        return IdentityMtx;
      break;
    }
  }

  Matrix4x4 Matrix4x4::Rotation( float angle, const Vector3& vec )
  {
    const Vector3& axis = vec.Normalized();
    float cosa = cosf(angle);
    float sina = sinf(angle);
    float cosm = 1.0f - cosa;

    return Matrix4x4( (axis.x * axis.x * cosm) + cosa, 
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

  Matrix4x4 Matrix4x4::OrientationMtx( const Orientation& orient )
  {
    if(!orient.transpose)
      return Matrix4x4( orient.right.x, orient.up.x, orient.direction.x, 0.0f,
                        orient.right.y, orient.up.y, orient.direction.y, 0.0f,
                        orient.right.z, orient.up.z, orient.direction.z, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f );
    else
      return Matrix4x4( orient.right.x,         orient.right.y,     orient.right.z, 0.0f,
                        orient.up.x,               orient.up.y,        orient.up.z, 0.0f,
                        orient.direction.x, orient.direction.y, orient.direction.z, 0.0f,
                        0.0f,                             0.0f,               0.0f, 1.0f );
  }

  Matrix4x4 Matrix4x4::AlignVectorCamera( const Vector3& ri, const Vector3& up, const Vector3& di, const Vector3& ey )
  {
    return Matrix4x4(  ri.x,  ri.y,  ri.z, -ri.Dot(ey),
                       up.x,  up.y,  up.z, -up.Dot(ey),
                       -di.x, -di.y, -di.z, -di.Dot(ey),
                       0.0f,  0.0f,  0.0f,  1.0f );
  }


  Matrix4x4 Matrix4x4::LookAtMatrix( const Vector3& eye, const Vector3& pos, const Vector3& up )
  {
    Vector3& dir = (eye - pos).Normalize();
    return LookAtMatrixDir(eye, dir, up);

  }

  Matrix4x4 Matrix4x4::LookAtMatrixDir( const Vector3& eye, Vector3 const& dir, Vector3 const& up )
  {
    Vector3& rig = up.Cross( -dir ).Normalize();

    // correct the up-vector
    Vector3& upf = dir.Cross( rig ).Normalize();

    mat4 negTrans = mat4::Translation(-eye);

    return Matrix4x4( rig.x, rig.y, rig.z, 0.0f,
                      upf.x, upf.y, upf.z, 0.0f,
                      -dir.x, -dir.y, -dir.z,  0.0f,
                      0.0f,  0.0f,  0.0f,   1.0f ) * negTrans;
  }

  Matrix4x4 Matrix4x4::PerspectiveProjFOV( float aspect, float fov, float nearP, float farP )
  {
    const float uppr = 1.0f / tanf(fov);
    const float lowr = aspect * uppr;

    // normalizes our coordinates
    const float denm = 1.0f / (farP - nearP);
    const float aaaa = (-nearP - farP) * denm;
    const float bbbb = (-2.0f * nearP * farP) * denm;

    return Matrix4x4(uppr, 0.0f,  0.0f, 0.0f,
                     0.0f, lowr,  0.0f, 0.0f,
                     0.0f, 0.0f,  aaaa, bbbb,
                     0.0f, 0.0f, -1.0f, 0.0f);
  }

  Matrix4x4 Matrix4x4::OrthographicProj( float width, float height, float nearP, float farP )
  {
    const float uppr = 2.0f / width;
    const float lowr = 2.0f / height;

    //solution to projection 
    const float aaaa = 1.0f / (nearP - farP);
    const float bbbb = nearP * aaaa;

    return Matrix4x4(uppr, 0.0f, 0.0f, 0.0f,
                     0.0f, lowr, 0.0f, 0.0f,
                     0.0f, 0.0f, aaaa, bbbb,
                     0.0f, 0.0f, 0.0f, 1.0f);
  }

  Matrix4x4 Matrix4x4::CrossProdMtx( const Vector3& represent ) const
  {
    Matrix4x4 mat;
    mat.mtx[0][0] = 0;
    mat.mtx[1][1] = 0;
    mat.mtx[0][1] = -represent.z;
    mat.mtx[0][2] =  represent.y;
    mat.mtx[1][0] =  represent.z;
    mat.mtx[1][2] = -represent.x;
    mat.mtx[2][0] = -represent.y;
    mat.mtx[2][1] =  represent.x;
    mat.mtx[2][2] =  0;
    mat.mtx[3][3] =  0;
    return mat;
  }

  Matrix4x4 Matrix4x4::TensorProdMtx( const Vector3& lhs, const Vector3& rhs ) const
  {
    Matrix4x4 mat;
    mat.mtx[0][0] = lhs.x * rhs.x;
    mat.mtx[0][1] = lhs.x * rhs.y;
    mat.mtx[0][2] = lhs.x * rhs.z;

    mat.mtx[1][0] = lhs.y * rhs.x;
    mat.mtx[1][1] = lhs.y * rhs.y;
    mat.mtx[1][2] = lhs.y * rhs.z;

    mat.mtx[2][0] =  lhs.z * rhs.x;
    mat.mtx[2][1] =  lhs.z * rhs.y;
    mat.mtx[2][2] =  lhs.z * rhs.z;

    mat.mtx[3][3] =  0;
    return mat;
  }



}