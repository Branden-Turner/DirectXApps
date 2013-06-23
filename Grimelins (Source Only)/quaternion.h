/*! ====================================================================================================================
  @file quaternion.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sept.09.2011

  @brief 
    Quaternion Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_QUATERNION_H
#define VIRTUS_QUATERNION_H

namespace Grimelins {

  struct Vector3;
  struct Vector4;
  struct Matrix4;

  SCRIPT_HeaderDefine(Quaternion);

  class Quaternion
  {
    public:
    union
    {
      struct  
      {
        f32 w, x, y, z;
      };
      struct  
      {
        f32 w;
        Vector3 v3;
      };
      f32 v[4];
    };

    // default constructor
    Quaternion(void);
    // construct quaternion from real w, and imaginary x,y,z
    Quaternion(f32 _w, f32 _x, f32 _y, f32 _z);
    // construct quaternion from angle-axis
    Quaternion(f32 _w, const Vector3 &_v);
    Quaternion(const vec3& axis, f32 angle);

    Quaternion(const vec3& vec);
    // construct quaternion from rotation matrix
    Quaternion(const Matrix4 &mat);
    // convert quaternion to matrix
    Matrix4 matrix() const;
    ScriptMatrix4 scriptMatrix4() const;

    // return the vector
    Vector3 vector3()const;
    ScriptVec3 scriptVec3()const;

    void ScriptRotate(f32 angle, const ScriptVec3 &axis );
    void Rotate(f32 angle, f32 xx, f32 yy, f32 zz );
      

    // convert quaternion to angle-axis
    void angleAxis(f32 &angle, Vector3 &axis) const;
    void angleAxis(f32 &angle, ScriptVec3 &axis) const;

    // set quaternion to zero
    void zeroQuatern();
    // set identity
    void identity();
    // add quaternions
    Quaternion operator+(const Quaternion &a) const;
    // subtract quaternions
    Quaternion operator-(const Quaternion &a) const;
    // multiply quaternions
    Quaternion operator*(const Quaternion &a) const;
    //scalar multiply
    Quaternion operator*(f32 s) const;

    // scalar divide
    Quaternion operator/(f32 s) const;
    // add and assign
    Quaternion& operator+=(const Quaternion &a);
    // subtract and assign
    Quaternion& operator-=(const Quaternion &a);
    // dot product 
    Quaternion dot(const Quaternion &q);
    // calculate conjugate of quaternion
    Quaternion conjugate();
    // calculate length of quaternion
    f32 length() const;
    // calculate the norm
    f32 norm() const;
    // normalize
    void normalize();
    // calculate inverse of quaternion
    Quaternion inverse() const;
    friend std::ostream& operator<< (std::ostream& out, Quaternion const& q );
  };

  typedef Quaternion Quat;

  std::ostream& operator<< (std::ostream& out, Quaternion const& q );

  // scalar multiply
  Quaternion operator*(f32 s, const Quaternion &q);

  // slerp
  Quaternion slerp(const Quaternion &a, const Quaternion &b, f32 t);

}// namespace Virtus

#endif 