/*! ====================================================================================================================
  @file vector4.inl

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    Vector4 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
namespace Grimelins {

VEC4INLINE Vector4::Vector4(void)
  : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

VEC4INLINE Vector4::Vector4(const Vector4& rhs)
  : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
{
}


VEC4INLINE Vector4::Vector4( const ScriptVec4& rhs )
  : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
{
}


VEC4INLINE Vector4::Vector4(f32 xx, f32 yy, f32 zz, f32 ww)
  : x(xx), y(yy), z(zz), w(ww)
{
}

VEC4INLINE Vector4& Vector4::operator=(const Vector4& rhs)
{
  if( this != &rhs )
    std::memcpy( v, rhs.v, sizeof(v) );

  return *this;
}

VEC4INLINE Vector4 Vector4::operator+(f32 rhs) const
{
  return Vector4(x + rhs, y + rhs, z + rhs, w + rhs);
}

VEC4INLINE Vector4 Vector4::operator-(f32 rhs) const
{
  return Vector4(x - rhs, y - rhs, z - rhs, w - rhs);
}

VEC4INLINE Vector4 Vector4::operator*(f32 rhs) const
{
  return Vector4(x * rhs, y * rhs, z * rhs, w * rhs);
}

VEC4INLINE Vector4 Vector4::operator/(f32 rhs) const
{
  return Vector4(x / rhs, y / rhs, z / rhs, w / rhs);
}

VEC4INLINE Vector4& Vector4::operator+=(f32 rhs)
{
  x += rhs;
  y += rhs;
  z += rhs;
  w += rhs;
  return *this;
}

VEC4INLINE Vector4& Vector4::operator-=(f32 rhs)
{
  x -= rhs;
  y -= rhs;
  z -= rhs;
  w -= rhs;
  return *this;
}

VEC4INLINE Vector4& Vector4::operator*=(f32 rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

VEC4INLINE Vector4& Vector4::operator/=(f32 rhs)
{
  x /= rhs;
  y /= rhs;
  z /= rhs;
  w /= rhs;
  return *this;
}

VEC4INLINE Vector4 Vector4::operator+(const Vector4& rhs) const
{
  return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

VEC4INLINE Vector4 Vector4::operator-(const Vector4& rhs) const
{
  return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

VEC4INLINE Vector4 Vector4::operator*(const Vector4& rhs) const
{
  return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
}

VEC4INLINE Vector4 Vector4::operator/(const Vector4& rhs) const
{
  return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
}

VEC4INLINE Vector4& Vector4::operator+=(const Vector4& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

VEC4INLINE Vector4& Vector4::operator-=(const Vector4& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

VEC4INLINE Vector4& Vector4::operator*=(const Vector4& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  w *= rhs.w;
  return *this;
}

VEC4INLINE Vector4& Vector4::operator/=(const Vector4& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  w /= rhs.w;
  return *this;
}

VEC4INLINE f32& Vector4::operator[](u32 index)
{
  ErrorIf(index > 3, "Index Outside of Range: Vector4::operator[]");

  return v[index];
}

VEC4INLINE const f32& Vector4::operator[](u32 index) const
{
  ErrorIf(index > 3, "Index Outside of Range: Vector4::operator[]");

  return v[index];
}

VEC4INLINE Vector4 Vector4::operator-(void) const
{
  return Vector4(-x, -y, -z, -w);
}

VEC4INLINE Vector4 Vector4::operator+(void) const
{
  return *this;
}

VEC4INLINE f32 Vector4::Length(void) const
{
  return sqrtf(x*x + y*y + z*z + w*w);
}

VEC4INLINE f32 Vector4::LengthSq(void) const
{
  return (x*x + y*y + z*z + w*w);
}

VEC4INLINE f32 Vector4::Magnitude(void) const
{
  return sqrtf(x*x + y*y + z*z + w*w);
}

VEC4INLINE f32 Vector4::MagnitudeSq(void) const
{
  return (x*x + y*y + z*z + w*w);
}

VEC4INLINE f32 Vector4::Dot(const Vector4& rhs) const
{
  return (x * rhs.x + y * rhs.x + z * rhs.z + w * rhs.w);
}

VEC4INLINE Vector4 Vector4::Cross(const Vector4& rhs) const
{
  return Vector4(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x, 0.0f);
}

VEC4INLINE Vector4& Vector4::Normalize( void )
{
  float len = Magnitude();

  x /= len;
  y /= len;
  z /= len;

  return *this;
}

VEC4INLINE Vector4 Vector4::Normalized( void ) const
{
  float len = Magnitude();
  return Vector4(x / len, y / len, z / len, w);
}

VEC4INLINE void Vector4::Zero(void)
{
  std::memset( v, 0, sizeof(v) );
}

} /// namespace Virtus