/**********************************************************************/
/*
  
  \file
    quaternion.h
  
  \author
    branden.turner
    All content © 2012 DigiPen (USA) Corporation, all rights reserved.

  \brief
    quaternion class
    
*/
/**********************************************************************/
#ifndef QUAT_H
#define QUAT_H

namespace cs350
{
  struct Quaternion
  {
    //has a scalar and a vector representing i, j, and k imaginary components.
    union
    {
      struct
      {
        float x, y, z, s;
      };

      struct 
      {
        vec3 v;
        float s;
      };

      float a[4];
    };

    //Constructors
    Quaternion();
    Quaternion(const mat4& mtx);
    Quaternion(float _s, const vec3& vec);
    Quaternion(float s, float x, float y, float z);
    Quaternion(const vec3& axis, float angle);

    //To grab a part of the quaternion.
    float& operator [] (unsigned index);

    //Operations for addition, subtraction, and scalar multiplication of quaternions
    Quaternion operator+ (const Quaternion& rhs) const;
    Quaternion operator- (const Quaternion& rhs) const;
    Quaternion operator* (const Quaternion& rhs) const;
    Quaternion operator* (float scalar) const;
    vec3 operator* (const vec3 &rhs) const;
    Quaternion& operator+= (const Quaternion& rhs);
    Quaternion& operator-= (const Quaternion& rhs);
    Quaternion& operator*= (const Quaternion& rhs);
    Quaternion& operator*= (float scalar);

    //Dot products and other necessary functions for rotations.
    float Dot(const Quaternion& rhs) const;
    float SquareMag() const;
    float Magnitude() const;
    Quaternion Inverse() const;
    void SetInverse();
    void SetIdentity();
    void Normalize();

    //Linear algebra conversions
    void ToMatrix(mat4& mat);
    void Rotate(vec3& vec) const;

    //Interpolation stuff
    void AnimSlerp(const Quaternion &q1, const Quaternion &q2, float t);
    void Nlerp(const Quaternion &q1, const Quaternion &q2, float t);
  };

  Quaternion operator* (float scalar, const Quaternion &quat);
  vec3 operator* (const vec3 &lhs, const Quaternion &rhs);

  typedef Quaternion quat;
}

#endif 