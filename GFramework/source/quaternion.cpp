/**********************************************************************/
/*
  
  \file
    quaternion.cpp
  
  \author
    branden.turner
    All content © 2012 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This is a quaternion class that I'm writing for use in the animation 
    system as required by CS460.
    
*/
/**********************************************************************/
#include "precompiled.h"

namespace cs350
{

    Quaternion::Quaternion(): s(1.0f), x(0.0f), y(0.0f), z(0.0f)
    {}

    Quaternion::Quaternion( const mat4& mtx )
    {
      s = 0.5f * sqrt( (mtx.m00 + mtx.m11 + mtx.m22) * 0.25f );
      
      float div = 1 / (4.0f * s);

      x = div * (mtx.m21 - mtx.m12);
      y = div * (mtx.m02 - mtx.m20);
      z = div * (mtx.m10 - mtx.m01);
    }

    Quaternion::Quaternion( float _s, const vec3& vec ): s(_s), v(vec)
    {}

    Quaternion::Quaternion( float _s, float _x, float _y, float _z ): s(_s), x(_x), y(_y), z(_z)
    {}

    Quaternion::Quaternion( const vec3& axis, float angle )
    {
      float halfAngle = 0.5f * angle;
      float sinHalfAngle = sin(halfAngle);
      float cosHalfAngle = cos(halfAngle);

      x = axis.x * sinHalfAngle;
      y = axis.y * sinHalfAngle;
      z = axis.z * sinHalfAngle;
      s = cosHalfAngle;
    }

    float& Quaternion::operator[]( unsigned index )
    {
      return a[index];
    }

    Quaternion Quaternion::operator+( const Quaternion& rhs ) const
    {
      return Quaternion(s + rhs.s, x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Quaternion Quaternion::operator-( const Quaternion& rhs ) const
    {
      return Quaternion(s - rhs.s, x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Quaternion Quaternion::operator*( const Quaternion& rhs ) const
    {
      //[s1, v1] * [s2, v2] = [s1 * s2 - v1 dot v2, s1 * v2 + s2 * v1 + v1 X v2]
      return Quaternion((s * rhs.s) - v.Dot(rhs.v) , rhs.v * s + v * rhs.s + v.Cross(rhs.v) );
    }

    Quaternion Quaternion::operator*( float scalar ) const
    {
      return Quaternion( s * scalar, x * scalar, y * scalar, z * scalar);
    }

    vec3 Quaternion::operator*( const vec3 &rhs ) const
    {
      vec3 vec = rhs;

      //Create a pure quaternion out of the vector
      Quaternion vecQ(0.0f, vec.x, vec.y, vec.z);

      //r' = q * r * q^-1
      vec =  ((*this)  * vecQ * Inverse()).v;

      return vec;
    }

    Quaternion& Quaternion::operator+=( const Quaternion& rhs )
    {
      s += rhs.s;
      x += rhs.x;
      y += rhs.y;
      z += rhs.z;
      return *this;
    }

    Quaternion& Quaternion::operator-=( const Quaternion& rhs )
    {
      s -= rhs.s;
      x -= rhs.x;
      y -= rhs.y;
      z -= rhs.z;
      return *this;
    }

    Quaternion& Quaternion::operator*=( const Quaternion& rhs )
    {
      vec3 v1 = v;
      float s1 = s;

      s = (s1 * rhs.s) - v1.Dot(rhs.v);
      v =  rhs.v * s1 +  v1 + v1.Cross(rhs.v) * rhs.s;

      return *this;
    }

    Quaternion& Quaternion::operator*=( float scalar )
    {
      s *= scalar;
      x *= scalar;
      y *= scalar;
      z *= scalar;
      return *this;
    }

    float Quaternion::Dot( const Quaternion& rhs ) const
    {
      return s * rhs.s + x * rhs.x + y * rhs.y + z * rhs.z;
    }


    float Quaternion::SquareMag() const
    {
      return s * s + x * x + y * y + z * z;
    }


    float Quaternion::Magnitude() const
    {
      return sqrt(s * s + x * x + y * y + z * z);
    }

    Quaternion Quaternion::Inverse() const
    {
      //q^-1 = 1/mag^2 * [s, -v]
      float sqrMag = SquareMag();

      if(sqrMag != 0.0f)
      {
        float det = 1.0f / sqrMag;  
        return Quaternion(det * s, det * (-x), det * (-y), det * (-z));
      }

      return Quaternion(*this);
    }

    void Quaternion::SetInverse()
    {
      //q^-1 = 1/mag^2 * [s, -v]
      float sqrMag = SquareMag();

      if(sqrMag != 0.0f)
      {
        float det = 1.0f / sqrMag;  
        s *= det;
        x *= -det;
        y *= -det; 
        z *= -det;
      }
    }

    void Quaternion::SetIdentity()
    {
      s = 1; 
      x = y = z = 0.0f;
    }

    void Quaternion::Normalize()
    {
      float mag = Magnitude();

      if(mag != 0.0f)
      {
        float div = 1.0f / mag;
        s *= div;
        x *= div;
        y *= div;
        z *= div;
      }
    }

    void Quaternion::ToMatrix(mat4& mat)
    {
      //Make sure we're using unit quaternions (I can turn this check off when I'm using Nlerp)
//       if(SquareMag() > 1.01f)
//         Normalize();

      float xx = x * x;
      float yy = y * y;
      float zz = z * z;
      float xy = x * y;
      float xz = x * z;
      float yz = y * z;
      float sx = s * x;
      float sy = s * y;
      float sz = s * z;

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

    void Quaternion::Rotate( vec3& vec ) const
    {
      //Create a pure quaternion out of the vector
      Quaternion vecQ(0.0f, vec.x, vec.y, vec.z);

      //r' = q * r * q^-1
      vec =  ((*this)  * vecQ * Inverse()).v;
    }

    void Quaternion::AnimSlerp( const Quaternion &q1, const Quaternion &q2, float t )
    {
      //Math both from Graphics Gems III and CS460
      //Slerp looks like this: qt = sin(alpha * (1 - t)) / sin(alpha) * q1 + sin(alpha * t) / sin(alpha) * q2
      //Where alpha is the angle between the two quaternions, which is simply cos^-1(q1 dot q2)

      //The cosine of the angle between these two quaternions is simply the dot product
      float cosine = q1.Dot(q2); 

      float slerp_epsilon = 0.00001f;

      bool flip;

      //if the cosine is negative, we need to flip it, to make sure we're using the geodesic path
      //This function will work just fine without these checks, but this is the most robust solution.
      if (flip = (cosine < 0))
        cosine = -cosine;

      float theta, omega;

      //if 1 - cos of the angle is above a certain epsilon, we can do the interpolation
      if ((1.0 - cosine) > slerp_epsilon)
      {
        float alpha = acos (cosine);
        float sinAlpha = sin (alpha);
        theta = static_cast<float>(sin((1.0 - t) * alpha) / sinAlpha);
        omega = static_cast<float>(sin(t * alpha) / sinAlpha);
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

    void Quaternion::Nlerp(const Quaternion &q1, const Quaternion &q2, float t)
    {
      float g = 1.0f - t;

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

    Quaternion operator*( float scalar, const Quaternion& quat )
    {
      return quat * scalar;
    }

    vec3 operator*( const vec3 &lhs, const Quaternion &rhs )
    {
      return rhs * lhs;
    }
}