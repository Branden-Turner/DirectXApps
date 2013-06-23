/*! ====================================================================================================================
  @file vector3.inl

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    Vector3 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "quaternion.h"



namespace Grimelins {

VEC3INLINE Vector3::Vector3(void)
  : x(0.0f), y(0.0f), z(0.0f)
{
}

VEC3INLINE Vector3::Vector3(const Vector3& rhs)
  : x(rhs.x), y(rhs.y), z(rhs.z)
{
}


VEC3INLINE Vector3::Vector3( const ScriptVec3& rhs )
  : x(rhs.x), y(rhs.y), z(rhs.z)
{
}



VEC3INLINE Vector3::Vector3(f32 xx, f32 yy, f32 zz)
  : x(xx), y(yy), z(zz)
{
}

VEC3INLINE Vector3& Vector3::operator=(const Vector3& rhs)
{
  if( this != &rhs )
    std::memcpy( v, rhs.v, sizeof(v) );

  return *this;
}

VEC3INLINE Vector3 Vector3::operator+(f32 rhs) const
{
  return Vector3(x + rhs, y + rhs, z + rhs);
}

VEC3INLINE Vector3 Vector3::operator-(f32 rhs) const
{
  return Vector3(x - rhs, y - rhs, z - rhs);
}

VEC3INLINE Vector3 Vector3::operator*(f32 rhs) const
{
  return Vector3(x * rhs, y * rhs, z * rhs);
}

VEC3INLINE Vector3 Vector3::operator/(f32 rhs) const
{
  return Vector3(x / rhs, y / rhs, z / rhs);
}

VEC3INLINE Vector3& Vector3::operator+=(f32 rhs)
{
  x += rhs;
  y += rhs;
  z += rhs;
  return *this;
}

VEC3INLINE Vector3& Vector3::operator-=(f32 rhs)
{
  x -= rhs;
  y -= rhs;
  z -= rhs;
  return *this;
}

VEC3INLINE Vector3& Vector3::operator*=(f32 rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

VEC3INLINE Vector3& Vector3::operator/=(f32 rhs)
{
  x /= rhs;
  y /= rhs;
  z /= rhs;
  return *this;
}

VEC3INLINE Vector3 Vector3::operator+(const Vector3& rhs) const
{
  return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

VEC3INLINE Vector3 Vector3::operator-(const Vector3& rhs) const
{
  return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

VEC3INLINE Vector3 Vector3::operator*(const Vector3& rhs) const
{
  return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

VEC3INLINE Vector3 Vector3::operator/(const Vector3& rhs) const
{
  return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
}

VEC3INLINE Vector3& Vector3::operator+=(const Vector3& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

VEC3INLINE Vector3& Vector3::operator-=(const Vector3& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

VEC3INLINE Vector3& Vector3::operator*=(const Vector3& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  return *this;
}

VEC3INLINE Vector3& Vector3::operator/=(const Vector3& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  return *this;
}

VEC3INLINE bool Vector3::operator==(const Vector3& rhs)
{
  return fequal(x, rhs.x) && fequal(y, rhs.y) && fequal(z, rhs.z);
}

VEC3INLINE bool Vector3::operator!=(const Vector3& rhs)
{
  return !fequal(x, rhs.x) || !fequal(y, rhs.y) || !fequal(z, rhs.z);
}

VEC3INLINE f32& Vector3::operator[](u32 index)
{
  ErrorIf(index > 2, "Index Outside of Range: Vector3::operator[]");

  return v[index];
}

VEC3INLINE const f32& Vector3::operator[](u32 index) const
{
  ErrorIf(index > 2, "Index Outside of Range: Vector3::operator[]");

  return v[index];
}

VEC3INLINE Vector3 Vector3::operator-(void) const
{
  return Vector3(-x, -y, -z);
}

VEC3INLINE Vector3 Vector3::operator+(void) const
{
  return *this;
}

VEC3INLINE f32 Vector3::Length(void) const
{
  return sqrtf(x*x + y*y + z*z);
}

VEC3INLINE f32 Vector3::LengthSq(void) const
{
  return (x*x + y*y + z*z);
}

VEC3INLINE f32 Vector3::Magnitude(void) const
{
  return sqrtf(x*x + y*y + z*z);
}

VEC3INLINE f32 Vector3::MagnitudeSq(void) const
{
  return (x*x + y*y + z*z);
}

VEC3INLINE f32 Vector3::Dot(const Vector3& rhs) const
{
  return (x * rhs.x + y * rhs.y + z * rhs.z);
}

VEC3INLINE Vector3 Vector3::Cross(const Vector3& rhs) const
{
  return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

VEC3INLINE void Vector3::Zero(void)
{
  x = y = z = 0.0f;
}

VEC3INLINE Vector3& Vector3::Normalize( void )
{
  f32 len = Magnitude();

  // check if the length is equal to zero
  if(len != 0)
  {
    x /= len;
    y /= len;
    z /= len;
  }
  else
  {
    x = 0;
    y = 0;
    z = 0;
  }
    
  return *this;

}

VEC3INLINE Vector3 Vector3::Normalized( void ) const
{
  f32 len = Magnitude();

  return Vector3(x / len, y / len, z / len);
}

VEC3INLINE Vector3 Vector3::RotateWithQuat(const Quaternion& quat)
{
  // convert the vector into a quaternion
  Quaternion vectQuat(*this);

  // compute the quaternion rotation result
  Quaternion quatResult = (quat * vectQuat) * quat.inverse();

  // convert the result quaternion into a vector
  return quatResult.vector3();
}

VEC3INLINE bool Vector3::isParallelWith(const Vector3& vec) const
{
    f32 scalProd = Dot(vec);
    f32 a = std::abs(scalProd);
    f32 b = Length() * vec.Length();
    return approxEqual(a,b);
}
} /// namespace Virtus
