/**********************************************************************/
/*
  
  \file
    animquaternion.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This is a quaternion class that I'm writing for use in the animation 
    system as required by CS460.
    
*/
/**********************************************************************/
#ifndef VIRTUS_ANIMQUATERNION_H
#define VIRTUS_ANIMQUATERNION_H

namespace Grimelins
{

    //The notation I use for quaternions is CS460s, so [s, v], but the layout in memory is [v, s]
    class AnimQuaternion
    {

      public:

      //has a scalar and a vector representing i, j, and k imaginary components.
      union
      {
        struct
        {
          f32 x, y, z, s;
        };

        struct 
        {
          vec3 v;
          f32 s;
        };

        f32 a[4];
      };

      //No need for an assignment operator or copy constructor as it's just four floats.

      //Constructors
      AnimQuaternion();
      AnimQuaternion(const Matrix4& mtx);
      AnimQuaternion(f32 _s, const vec3& vec);
      AnimQuaternion(f32 s, f32 x, f32 y, f32 z);
      AnimQuaternion(const vec3& axis, f32 angle);

      //To grab a part of the quaternion.
      f32& operator [] (u32 index);

      //Operations for addition, subtraction, and scalar multiplication of quaternions
      AnimQuaternion operator+ (const AnimQuaternion& rhs) const;
      AnimQuaternion operator- (const AnimQuaternion& rhs) const;
      AnimQuaternion operator* (const AnimQuaternion& rhs) const;
      AnimQuaternion operator* (f32 scalar) const;
      vec3 operator* (const vec3 &rhs) const;
      AnimQuaternion& operator+= (const AnimQuaternion& rhs);
      AnimQuaternion& operator-= (const AnimQuaternion& rhs);
      AnimQuaternion& operator*= (const AnimQuaternion& rhs);
      AnimQuaternion& operator*= (f32 scalar);

      //Dot products and other necessary functions for rotations.
      f32 Dot(const AnimQuaternion& rhs) const;
      f32 SquareMag() const;
      f32 Magnitude() const;
      AnimQuaternion Inverse() const;
      void SetInverse();
      void SetIdentity();
      void Normalize();

      //Linear algebra conversions
      void ToMatrix(Matrix4& mat);
      void Rotate(vec3& vec) const;

      //Interpolation stuff
      void AnimSlerp(const AnimQuaternion &q1, const AnimQuaternion &q2, f32 t);
      void Nlerp(const AnimQuaternion &q1, const AnimQuaternion &q2, f32 t);
    };

    AnimQuaternion operator* (f32 scalar, const AnimQuaternion &quat);
    vec3 operator* (const vec3 &lhs, const AnimQuaternion &rhs);
}

#endif  VIRTUS_ANIMQUATERNION_H