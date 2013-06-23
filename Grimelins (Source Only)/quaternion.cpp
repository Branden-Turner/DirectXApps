/*! ====================================================================================================================
  @file Quaternion.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.9.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "quaternion.h"

namespace Grimelins {

  SCRIPT_ClassStart(Quaternion);
  SCRIPT_CTOR_Enabled();
  SCRIPT_PlainData(x);
  SCRIPT_PlainData(y);
  SCRIPT_PlainData(z);
  SCRIPT_PlainData(w);
  SCRIPT_Method(length);
  SCRIPT_Method(dot);
  SCRIPT_Method(conjugate);
  SCRIPT_Method(norm);
  SCRIPT_Method(normalize);
  SCRIPT_Method(dot);
  SCRIPT_Method(inverse);
  SCRIPT_Method(zeroQuatern);
  SCRIPT_Method(identity);
  SCRIPT_Method(scriptVec3);
  SCRIPT_Method(scriptMatrix4);
  SCRIPT_Method(ScriptRotate);
  SCRIPT_Method(Rotate);

  SCRIPT_MethodOverload(operator-, "_sub", Quaternion, (const Quaternion&) const);
  SCRIPT_MethodOverload(operator+, "_add", Quaternion, (const Quaternion&) const);
  SCRIPT_MethodOverload(operator*, "_mul", Quaternion, (const Quaternion&) const);
  SCRIPT_MethodOverload(operator/, "_div", Quaternion, (f32) const);

  SCRIPT_MethodOverload(operator-=, "subWith", Quaternion&, (const Quaternion&));
  SCRIPT_MethodOverload(operator+=, "addWith", Quaternion&, (const Quaternion&));

  SCRIPT_MethodOverload(operator*, "mulF", Quaternion, (f32) const );
  SCRIPT_MethodOverload(angleAxis, "angleAxis", void, (f32 &angle, ScriptVec3 &axis) const);

  SCRIPT_ClassEnd();

  Quaternion::Quaternion() : w(0.0f), x(0.0f), y(0.0f), z(0.0f)
  {

  }

  Quaternion::Quaternion(f32 _w, f32 _x, f32 _y, f32 _z)
  {
    w = _w;
    x = _x; 
    y = _y; 
    z = _z;
  }

  Quaternion::Quaternion( const vec3& vec ) : w(0.0f), v3(vec)
  {
  }
  

  Quaternion::Quaternion( f32 _w, const Vector3 &_v ) : w(_w), v3(_v)
  {
  }

  Quaternion::Quaternion( const vec3& axis, f32 angle )
  {
    // half angles
    const f32 a = angle * .5f;
    const f32 s = sin(a);
    const f32 c = cos(a);

    w = c;
    x = axis.x * s;
    y = axis.y * s;
    z = axis.z * s;
  }

  void Quaternion::Rotate(f32 angle, f32 xx, f32 yy, f32 zz )
  {
    // half angles
    const f32 a = angle * .5f;
    const f32 s = sin(a);
    const f32 c = cos(a);

    w = c;
    x = xx * s;
    y = yy * s;
    z = zz * s;

  }

  void Quaternion::ScriptRotate(f32 angle, const ScriptVec3 &axis )
  {
     Rotate(angle, axis.x, axis.y, axis.z);
  }

  Quaternion::Quaternion(const Matrix4 &mat)
  {
    const f32 trace = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
    if(trace > 0)
    {
      // if |w| > .5, choose w > .5
      f32 root = sqrt(trace + 1.0f); // 2w
      w = .5f * root; 
      root = .5f / root; // 1/(4w)
      x = (mat.m[2][1] - mat.m[1][2]) * root;
      y = (mat.m[0][2] - mat.m[2][0]) * root;
      z = (mat.m[1][0] - mat.m[0][1]) * root;
    }
    else
    {
      // if |w| <= .5
      static int next[3] = {2,3,1};

      int i = 1;
      if(mat.m[1][1] > mat.m[0][0])
        i = 2;
      if(mat.m[2][2] > mat.m[i][i]) // find out if this initializes a whole matrix....
        i = 3;
      int j = next[i];
      int k = next[j];

      f32 root = sqrt(mat.m[i][i] - mat.m[j][j] - mat.m[k][k] + 1.0f);
      f32 *q[3] = {&x, &y, &z};
      *q[i] = .5f * root;
      root = .5f / root;
      w = (mat.m[k][j] - mat.m[j][k]) * root;
      *q[j] = (mat.m[j][i] + mat.m[i][j]) * root;
      *q[k] = (mat.m[k][i] + mat.m[i][k]) * root;
    }
  }


  Matrix4 Quaternion::matrix() const
  {
    f32 xx = x * x;
    f32 yy = y * y;
    f32 zz = z * z;
    f32 xy = x * y;
    f32 xz = x * z;
    f32 yz = y * z;
    f32 wx = w * x;
    f32 wy = w * y;
    f32 wz = w * z;

    Matrix4 mat;

    //I'm building this matrix in the transpose order for now, 
    //if we change things and rotations break, this is why
    mat.m00 = 1 - 2 * (yy  + zz);
    mat.m10 = 2 * (xy - wz);
    mat.m20 = 2 * (xz + wy);
    mat.m30 = 0;
    mat.m01 = 2 * (xy + wz);
    mat.m11 = 1 - 2 * (xx + zz);
    mat.m21 = 2 * (yz - wx);
    mat.m31 = 0;
    mat.m02 = 2 * (xz - wy);
    mat.m12 = 2 * (yz + wx);
    mat.m22 = 1 - 2 * (xx + yy);
    mat.m32 = mat.m03 = mat.m13 = mat.m23 = 0;
    mat.m33 = 1;

    return mat;

    //    Non transposed order
    //    (1 - 2 * (yy  + zz), 2 * (xy - wz),      2 * (xz + wy),     0,
    //     2 * (xy + wz),      1 - 2 * (xx + zz),  2 * (yz - wx),     0,
    //     2 * (xz - wy),      2 * (yz + wx),      1 - 2 * (xx + yy), 0,
    //     0,                  0,                  0,                 1);
  }

  Vector3 Quaternion::vector3() const
  {
    return Vector3(x,y,z);
  }

  void Quaternion::angleAxis(f32 &angle, Vector3 &axis) const
  {
    const f32 squareLength = x*x + y*y + z*z;
    if(squareLength > epsilonSquared)
    {
      angle = 2.0f * (f32) acos(w);
      const f32 inv = 1.0f / (f32) pow(squareLength, .5f);
      axis.x = x * inv;
      axis.y = y * inv;
      axis.z = z * inv;
    }
    else
    {
      angle = 0.0f;
      axis.x = 1.0f;
      axis.y = 0.0f;
      axis.z = 0.0f;
    }
  }

  void Quaternion::angleAxis( f32 &angle, ScriptVec3 &axis ) const
  {
    angleAxis(angle, vec3(axis));
  }

  void Quaternion::zeroQuatern()
  {
    w = 0;
    x = 0;
    y = 0;
    z = 0;
  }

  void Quaternion::identity()
  {
    w = 1;
    x = 0;
    y = 0;
    z = 0;
  }

  Quaternion Quaternion::operator+(const Quaternion &a) const
  {
    return Quaternion(w + a.w, x + a.x, y + a.y, z + a.z);
  }

  Quaternion Quaternion::operator-(const Quaternion &a) const
  {
    return Quaternion(w - a.w, x - a.x, y - a.y, z - a.z);
  }

  Quaternion Quaternion::operator*(const Quaternion &a) const
  {
    return Quaternion((w * a.w) - v3.Dot(a.v3), a.v3 * w + v3 * a.w + v3.Cross(a.v3) );
  }

  Quaternion Quaternion::operator*(f32 s) const
  {
    return Quaternion(w * s, x * s, y * s, z * s);
  }

  Quaternion operator*(f32 s, const Quaternion &q)
  {
    return q * s;
  }

  Quaternion Quaternion::operator/(f32 s) const
  {
    if(s == 0)
      return Quaternion(0,0,0,0);
    return Quaternion(w/s, x/s, y/s, z/s);
  }

  Quaternion& Quaternion::operator+=(const Quaternion &a)
  {
    w += a.w;
    x += a.x;
    y += a.y;
    z += a.z;
    return *this;
  }

  Quaternion& Quaternion::operator-=(const Quaternion &a)
  {
    w -= a.w;
    x -= a.x;
    y -= a.y;
    z -= a.z;
    return *this;
  }

  Quaternion Quaternion::dot(const Quaternion &q)
  {
    return Quaternion(w * q.w + x * q.x + y * q.y + z * q.z, 0,0,0);
  }

  Quaternion Quaternion::conjugate()
  {
    return Quaternion(w, -x, -y, -z);
  }

  f32 Quaternion::length() const
  {
    return sqrt(w*w + x*x + y*y + z*z);
  }

  f32 Quaternion::norm() const
  {
    return (w*w + x*x + y*y + z*z);
  }

  void Quaternion::normalize()
  {
    const f32 len = this->length();
    if(len == 0)
    {
      w = 1;
      x = 0;
      y = 0;
      z = 0;
    }
    else
    {
      f32 inv = 1.0f / len;
      x = x * inv;
      y = y * inv;
      z = z * inv;
      w = w * inv;
    }
  }

  Quaternion Quaternion::inverse() const
  {
    const f32 n = norm();
    if(n == 0)
      return Quaternion(*this);
    else
      return Quaternion(w/n, -x/n, -y/n, -z/n);
  }

  Grimelins::ScriptVec3 Quaternion::scriptVec3() const
  {
    return ScriptVec3(x,y,z);
  }

  Grimelins::ScriptMatrix4 Quaternion::scriptMatrix4() const
  {
    f32 xx = x * x;
    f32 yy = y * y;
    f32 zz = z * z;
    f32 xy = x * y;
    f32 xz = x * z;
    f32 yz = y * z;
    f32 wx = w * x;
    f32 wy = w * y;
    f32 wz = w * z;

    ScriptMatrix4 mat;

    //I'm building this matrix in the transpose order for now, 
    //if we change things and rotations break, this is why
    mat.m00 = 1 - 2 * (yy  + zz);
    mat.m10 = 2 * (xy - wz);
    mat.m20 = 2 * (xz + wy);
    mat.m30 = 0;
    mat.m01 = 2 * (xy + wz);
    mat.m11 = 1 - 2 * (xx + zz);
    mat.m21 = 2 * (yz - wx);
    mat.m31 = 0;
    mat.m02 = 2 * (xz - wy);
    mat.m12 = 2 * (yz + wx);
    mat.m22 = 1 - 2 * (xx + yy);
    mat.m32 = mat.m03 = mat.m13 = mat.m23 = 0;
    mat.m33 = 1;

    return mat;

    //    Non transposed order
    //    (1 - 2 * (yy  + zz), 2 * (xy - wz),      2 * (xz + wy),     0,
    //     2 * (xy + wz),      1 - 2 * (xx + zz),  2 * (yz - wx),     0,
    //     2 * (xz - wy),      2 * (yz + wx),      1 - 2 * (xx + yy), 0,
    //     0,                  0,                  0,                 1);
  }


  Quaternion slerp(const Quaternion &a, const Quaternion &b, f32 t)
  {
    //t E [0:1]
    if(t < 0 || t > 1)
      return Quaternion(0,0,0,0);

    f32 flip = 1;
    f32 cosine = a.w * b.w + a.x + b.x + a.y * b.y + a.z * b.z;

    if (cosine < 0)
    {
      cosine = -cosine;
      flip = -1;
    }

    if((1-cosine) < epsilon)
      return (a * (1-t) + b * (t*flip));

    f32 theta = (f32)acos(cosine);
    f32 sine = (f32)sin(theta);
    f32 beta = (f32)sin((1-t) * theta) / sine;
    f32 alpha = (f32)sin(t * theta) / sine * flip;

    return (a * beta + b * alpha);
  }

} // namespace Virtus