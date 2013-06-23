/*! ====================================================================================================================
  @file vector2.inl

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    Vector2 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
namespace Grimelins {

VEC2INLINE Vector2::Vector2(void)
  : x(0.0f), y(0.0f)
{
}

VEC2INLINE Vector2::Vector2(const Vector2& rhs)
  : x(rhs.x), y(rhs.y)
{
}


VEC2INLINE Vector2::Vector2( const ScriptVec2& rhs )
  : x(rhs.x), y(rhs.y)
{
}


VEC2INLINE Vector2::Vector2(f32 xx, f32 yy)
  : x(xx), y(yy)
{
}

VEC2INLINE Vector2& Vector2::operator=(const Vector2& rhs)
{
  if( this != &rhs )
    std::memcpy( v, rhs.v, sizeof(v) );

  return *this;
}

VEC2INLINE Vector2 Vector2::operator+(f32 rhs) const
{
  return Vector2(x + rhs, y + rhs);
}

VEC2INLINE Vector2 Vector2::operator-(f32 rhs) const
{
  return Vector2(x - rhs, y - rhs);
}

VEC2INLINE Vector2 Vector2::operator*(f32 rhs) const
{
  return Vector2(x * rhs, y * rhs);
}

VEC2INLINE Vector2 Vector2::operator/(f32 rhs) const
{
  return Vector2(x / rhs, y / rhs);
}

VEC2INLINE Vector2& Vector2::operator+=(f32 rhs)
{
  x += rhs;
  y += rhs;
  return *this;
}

VEC2INLINE Vector2& Vector2::operator-=(f32 rhs)
{
  x -= rhs;
  y -= rhs;
  return *this;
}

VEC2INLINE Vector2& Vector2::operator*=(f32 rhs)
{
  x *= rhs;
  y *= rhs;
  return *this;
}

VEC2INLINE Vector2& Vector2::operator/=(f32 rhs)
{
  x /= rhs;
  y /= rhs;
  return *this;
}

VEC2INLINE Vector2 Vector2::operator+(const Vector2& rhs) const
{
  return Vector2(x + rhs.x, y + rhs.y);
}

VEC2INLINE Vector2 Vector2::operator-(const Vector2& rhs) const
{
  return Vector2(x - rhs.x, y - rhs.y);
}

VEC2INLINE Vector2 Vector2::operator*(const Vector2& rhs) const
{
  return Vector2(x * rhs.x, y * rhs.y);
}

VEC2INLINE Vector2 Vector2::operator/(const Vector2& rhs) const
{
  return Vector2(x / rhs.x, y / rhs.y);
}

VEC2INLINE Vector2& Vector2::operator+=(const Vector2& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

VEC2INLINE Vector2& Vector2::operator-=(const Vector2& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

VEC2INLINE Vector2& Vector2::operator*=(const Vector2& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  return *this;
}

VEC2INLINE Vector2& Vector2::operator/=(const Vector2& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  return *this;
}

VEC2INLINE f32& Vector2::operator[](u32 index)
{
  ErrorIf(index > 1, "Index Outside of Range: Vector2::operator[]");

  return v[index];
}

VEC2INLINE const f32& Vector2::operator[](u32 index) const
{
  ErrorIf(index > 1, "Index Outside of Range: Vector2::operator[]");

  return v[index];
}

VEC2INLINE Vector2 Vector2::operator-(void) const
{
  return Vector2(-x, -y);
}

VEC2INLINE Vector2 Vector2::operator+(void) const
{
  return *this;
}

VEC2INLINE f32 Vector2::Length(void) const
{
  return sqrtf(x*x + y*y);
}

VEC2INLINE f32 Vector2::LengthSq(void) const
{
  return (x*x + y*y);
}

VEC2INLINE f32 Vector2::Magnitude(void) const
{
  return sqrtf(x*x + y*y);
}

VEC2INLINE f32 Vector2::MagnitudeSq(void) const
{
  return (x*x + y*y);
}

VEC2INLINE Vector2& Vector2::Normalize( void )
{
  float len = Magnitude();

  x /= len;
  y /= len;

  return *this;
}

VEC2INLINE Vector2 Vector2::Normalized( void ) const
{
  float len = Magnitude();
  return Vector2(x / len, y / len);
}

VEC2INLINE f32 Vector2::Dot(const Vector2& rhs) const
{
  return (x * rhs.x + y * rhs.x);
}

VEC2INLINE void Vector2::Zero(void)
{
  std::memset( v, 0, sizeof(v) );
}

} /// namespace Virtus