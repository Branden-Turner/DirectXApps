/**********************************************************************/
/*
  
  \file
    animquaternion.cpp
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This is a quaternion class that I'm writing for use in the animation 
    system as required by CS460.
    
*/
/**********************************************************************/
#include "precompiled.h"
#include "animquaternion.h"

namespace Grimelins
{

    AnimQuaternion::AnimQuaternion(): s(1.0f), x(0.0f), y(0.0f), z(0.0f)
    {}

    AnimQuaternion::AnimQuaternion( const Matrix4& mtx )
    {
      s = 0.5f * sqrt( (mtx.m00 + mtx.m11 + mtx.m22) * 0.25f );
      
      f32 div = 1 / (4.0f * s);

      x = div * (mtx.m21 - mtx.m12);
      y = div * (mtx.m02 - mtx.m20);
      z = div * (mtx.m10 - mtx.m01);
    }

    AnimQuaternion::AnimQuaternion( f32 _s, const vec3& vec ): s(_s), v(vec)
    {}

    AnimQuaternion::AnimQuaternion( f32 _s, f32 _x, f32 _y, f32 _z ): s(_s), x(_x), y(_y), z(_z)
    {}

    AnimQuaternion::AnimQuaternion( const vec3& axis, f32 angle )
    {
      f32 halfAngle = 0.5f * angle;
      f32 sinHalfAngle = sin(halfAngle);
      f32 cosHalfAngle = cos(halfAngle);

      x = axis.x * sinHalfAngle;
      y = axis.y * sinHalfAngle;
      z = axis.z * sinHalfAngle;
      s = cosHalfAngle;
    }

    f32& AnimQuaternion::operator[]( u32 index )
    {
      return a[index];
    }

    AnimQuaternion AnimQuaternion::operator+( const AnimQuaternion& rhs ) const
    {
      return AnimQuaternion(s + rhs.s, x + rhs.x, y + rhs.y, z + rhs.z);
    }

    AnimQuaternion AnimQuaternion::operator-( const AnimQuaternion& rhs ) const
    {
      return AnimQuaternion(s - rhs.s, x - rhs.x, y - rhs.y, z - rhs.z);
    }

    AnimQuaternion AnimQuaternion::operator*( const AnimQuaternion& rhs ) const
    {
      //[s1, v1] * [s2, v2] = [s1 * s2 - v1 dot v2, s1 * v2 + s2 * v1 + v1 X v2]
      return AnimQuaternion((s * rhs.s) - v.Dot(rhs.v) , rhs.v * s + v * rhs.s + v.Cross(rhs.v) );
    }

    AnimQuaternion AnimQuaternion::operator*( f32 scalar ) const
    {
      return AnimQuaternion( s * scalar, x * scalar, y * scalar, z * scalar);
    }

    vec3 AnimQuaternion::operator*( const vec3 &rhs ) const
    {
      vec3 vec = rhs;

      //Create a pure quaternion out of the vector
      AnimQuaternion vecQ(0.0f, vec.x, vec.y, vec.z);

      //r' = q * r * q^-1
      vec =  ((*this)  * vecQ * Inverse()).v;

      return vec;
    }

    AnimQuaternion& AnimQuaternion::operator+=( const AnimQuaternion& rhs )
    {
      s += rhs.s;
      x += rhs.x;
      y += rhs.y;
      z += rhs.z;
      return *this;
    }

    AnimQuaternion& AnimQuaternion::operator-=( const AnimQuaternion& rhs )
    {
      s -= rhs.s;
      x -= rhs.x;
      y -= rhs.y;
      z -= rhs.z;
      return *this;
    }

    AnimQuaternion& AnimQuaternion::operator*=( const AnimQuaternion& rhs )
    {
      vec3 v1 = v;
      f32 s1 = s;

      s = (s1 * rhs.s) - v1.Dot(rhs.v);
      v =  rhs.v * s1 +  v1 + v1.Cross(rhs.v) * rhs.s;

      return *this;
    }

    AnimQuaternion& AnimQuaternion::operator*=( f32 scalar )
    {
      s *= scalar;
      x *= scalar;
      y *= scalar;
      z *= scalar;
      return *this;
    }

    f32 AnimQuaternion::Dot( const AnimQuaternion& rhs ) const
    {
      return s * rhs.s + x * rhs.x + y * rhs.y + z * rhs.z;
    }


    f32 AnimQuaternion::SquareMag() const
    {
      return s * s + x * x + y * y + z * z;
    }


    f32 AnimQuaternion::Magnitude() const
    {
      return sqrt(s * s + x * x + y * y + z * z);
    }

    AnimQuaternion AnimQuaternion::Inverse() const
    {
      //q^-1 = 1/mag^2 * [s, -v]
      f32 sqrMag = SquareMag();

      if(sqrMag != 0.0f)
      {
        f32 det = 1.0f / sqrMag;  
        return AnimQuaternion(det * s, det * (-x), det * (-y), det * (-z));
      }

      return AnimQuaternion(*this);
    }

    void AnimQuaternion::SetInverse()
    {
      //q^-1 = 1/mag^2 * [s, -v]
      f32 sqrMag = SquareMag();

      if(sqrMag != 0.0f)
      {
        f32 det = 1.0f / sqrMag;  
        s *= det;
        x *= -det;
        y *= -det; 
        z *= -det;
      }
    }

    void AnimQuaternion::SetIdentity()
    {
      s = 1; 
      x = y = z = 0.0f;
    }

    void AnimQuaternion::Normalize()
    {
      f32 mag = Magnitude();

      if(mag != 0.0f)
      {
        f32 div = 1.0f / mag;
        s *= div;
        x *= div;
        y *= div;
        z *= div;
      }
    }

    void AnimQuaternion::ToMatrix(Matrix4& mat)
    {
      //Make sure we're using unit quaternions (I can turn this check off when I'm using Nlerp)
//       if(SquareMag() > 1.01f)
//         Normalize();

      f32 xx = x * x;
      f32 yy = y * y;
      f32 zz = z * z;
      f32 xy = x * y;
      f32 xz = x * z;
      f32 yz = y * z;
      f32 sx = s * x;
      f32 sy = s * y;
      f32 sz = s * z;

      //I'm building this matrix in the transpose order for now, 
      //if we change things and animation breaks, THIS IS WHY
      mat.m00 = 1 - 2 * (yy  + zz);
      mat.m10 = 2 * (xy - sz);
      mat.m20 = 2 * (xz + sy);
      mat.m30 = 0;
      mat.m01 = 2 * (xy + sz);
      mat.m11 = 1 - 2 * (xx + zz);
      mat.m21 = 2 * (yz - sx);
      mat.m31 = 0;
      mat.m02 = 2 * (xz - sy);
      mat.m12 = 2 * (yz + sx);
      mat.m22 = 1 - 2 * (xx + yy);
      mat.m32 = mat.m03 = mat.m13 = mat.m23 = 0;
      mat.m33 = 1;

//    Non transposed order
//    (1 - 2 * (yy  + zz), 2 * (xy - sz),      2 * (xz + sy),     0,
//     2 * (xy + sz),      1 - 2 * (xx + zz),  2 * (yz - sx),     0,
//     2 * (xz - sy),      2 * (yz + sx),      1 - 2 * (xx + yy), 0,
//     0,                  0,                  0,                 1);
    }

    void AnimQuaternion::Rotate( vec3& vec ) const
    {
      //Create a pure quaternion out of the vector
      AnimQuaternion vecQ(0.0f, vec.x, vec.y, vec.z);

      //r' = q * r * q^-1
      vec =  ((*this)  * vecQ * Inverse()).v;
    }

    void AnimQuaternion::AnimSlerp( const AnimQuaternion &q1, const AnimQuaternion &q2, f32 t )
    {
      //Math both from Graphics Gems III and CS460
      //Slerp looks like this: qt = sin(alpha * (1 - t)) / sin(alpha) * q1 + sin(alpha * t) / sin(alpha) * q2
      //Where alpha is the angle between the two quaternions, which is simply cos^-1(q1 dot q2)

      //The cosine of the angle between these two quaternions is simply the dot product
      f32 cosine = q1.Dot(q2); 

      f32 slerp_epsilon = 0.00001f;

      bool flip;

      //if the cosine is negative, we need to flip it, to make sure we're using the geodesic path
      //This function will work just fine without these checks, but this is the most robust solution.
      if (flip = (cosine < 0))
        cosine = -cosine;

      f32 theta, omega;

      //if 1 - cos of the angle is above a certain epsilon, we can do the interpolation
      if ((1.0 - cosine) > slerp_epsilon)
      {
        f32 alpha = acos (cosine);
        f32 sinAlpha = sin (alpha);
        theta = static_cast<f32>(sin((1.0 - t) * alpha) / sinAlpha);
        omega = static_cast<f32>(sin(t * alpha) / sinAlpha);
      }
      //Otherwise just linearly interpolate.
      else
      {
        theta = (1.0f - t);
        omega = t;
      }

      if (flip) omega = -omega;

      x = (theta * q1.x + omega * q2.x);
      y = (theta * q1.y + omega * q2.y);
      z = (theta * q1.z + omega * q2.z);
      s = (theta * q1.s + omega * q2.s);
    }

    void AnimQuaternion::Nlerp(const AnimQuaternion &q1, const AnimQuaternion &q2, f32 t)
    {
      f32 g = 1.0f - t;

      //See if we need to invert a quaternion to maintain a geodesic path
      if(q1.Dot(q2) < 0.0f)
          g = -g;

      //interpolate the normalize
      x = g * q1.x + t * q2.x;
      y = g * q1.y + t * q2.y;
      z = g * q1.z + t * q2.z;
      s = g * q1.s + t * q2.s;
      Normalize();
    }

    AnimQuaternion operator*( f32 scalar, const AnimQuaternion& quat )
    {
      return quat * scalar;
    }

    Grimelins::vec3 operator*( const vec3 &lhs, const AnimQuaternion &rhs )
    {
      return rhs * lhs;
    }
}