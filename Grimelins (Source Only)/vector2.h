/*! ====================================================================================================================
  @file vector2.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    Vector2 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_VECTOR2_H
#define VIRTUS_VECTOR2_H

#ifndef VEC2INLINE
  #if(VIRTUS_CONFIG_DEBUG_MODE)
    #define VEC2INLINE inline
  #else
    #define VEC2INLINE __forceinline
  #endif
#endif

namespace Grimelins {

struct Vector2
{
  union
  {
    struct { f32 x,y; };
    f32 v[2];
  };

  VEC2INLINE Vector2(void);
  VEC2INLINE Vector2(const Vector2& rhs);
  VEC2INLINE Vector2(const ScriptVec2& rhs);
  VEC2INLINE Vector2(f32 xx, f32 yy);

  VEC2INLINE Vector2& operator=(const Vector2& rhs);

  VEC2INLINE Vector2 operator+(f32 rhs) const;
  VEC2INLINE Vector2 operator-(f32 rhs) const;
  VEC2INLINE Vector2 operator*(f32 rhs) const;
  VEC2INLINE Vector2 operator/(f32 rhs) const;
  VEC2INLINE Vector2& operator+=(f32 rhs);
  VEC2INLINE Vector2& operator-=(f32 rhs);
  VEC2INLINE Vector2& operator*=(f32 rhs);
  VEC2INLINE Vector2& operator/=(f32 rhs);

  VEC2INLINE Vector2 operator+(const Vector2& rhs) const;
  VEC2INLINE Vector2 operator-(const Vector2& rhs) const;
  VEC2INLINE Vector2 operator*(const Vector2& rhs) const;
  VEC2INLINE Vector2 operator/(const Vector2& rhs) const;
  VEC2INLINE Vector2& operator+=(const Vector2& rhs);
  VEC2INLINE Vector2& operator-=(const Vector2& rhs);
  VEC2INLINE Vector2& operator*=(const Vector2& rhs);
  VEC2INLINE Vector2& operator/=(const Vector2& rhs);

  VEC2INLINE f32& operator[](u32 index);
  VEC2INLINE const f32& operator[](u32 index) const;

  VEC2INLINE Vector2 operator-(void) const;
  VEC2INLINE Vector2 operator+(void) const;

  VEC2INLINE f32 Length(void) const;
  VEC2INLINE f32 LengthSq(void) const;
  VEC2INLINE f32 Magnitude(void) const;
  VEC2INLINE f32 MagnitudeSq(void) const;
  VEC2INLINE Vector2& Normalize( void );
  VEC2INLINE Vector2 Normalized( void ) const;
  VEC2INLINE f32 Dot(const Vector2& rhs) const;
  VEC2INLINE void Zero(void);
  friend std::ostream& operator<< (std::ostream& out, Vector2 const& v2 );

};
std::ostream& operator<< (std::ostream& out, Vector2 const& v2 );
typedef Vector2 float2;
typedef Vector2 vec2;

} // namespace Virtus

#include "vector2.inl"


#endif // Include Guard