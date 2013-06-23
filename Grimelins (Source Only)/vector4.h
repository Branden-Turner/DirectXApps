/*! ====================================================================================================================
  @file vector4.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    Vector4 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_VECTOR4_H
#define VIRTUS_VECTOR4_H

#ifndef VEC4INLINE
  #if(VIRTUS_CONFIG_DEBUG_MODE)
    #define VEC4INLINE inline
  #else
    #define VEC4INLINE __forceinline
  #endif
#endif

namespace Grimelins {

struct Vector4
{
  union
  {
    struct { f32 x,y,z,w; };
    f32 v[4];
  };
  
  VEC4INLINE Vector4(void);
  VEC4INLINE Vector4(const Vector4& rhs);
  VEC4INLINE Vector4(const ScriptVec4& rhs);
  VEC4INLINE Vector4(f32 xx, f32 yy, f32 zz, f32 ww);

  VEC4INLINE Vector4& operator=(const Vector4& rhs);

  VEC4INLINE Vector4 operator+(f32 rhs) const;
  VEC4INLINE Vector4 operator-(f32 rhs) const;
  VEC4INLINE Vector4 operator*(f32 rhs) const;
  VEC4INLINE Vector4 operator/(f32 rhs) const;
  VEC4INLINE Vector4& operator+=(f32 rhs);
  VEC4INLINE Vector4& operator-=(f32 rhs);
  VEC4INLINE Vector4& operator*=(f32 rhs);
  VEC4INLINE Vector4& operator/=(f32 rhs);

  VEC4INLINE Vector4 operator+(const Vector4& rhs) const;
  VEC4INLINE Vector4 operator-(const Vector4& rhs) const;
  VEC4INLINE Vector4 operator*(const Vector4& rhs) const;
  VEC4INLINE Vector4 operator/(const Vector4& rhs) const;
  VEC4INLINE Vector4& operator+=(const Vector4& rhs);
  VEC4INLINE Vector4& operator-=(const Vector4& rhs);
  VEC4INLINE Vector4& operator*=(const Vector4& rhs);
  VEC4INLINE Vector4& operator/=(const Vector4& rhs);

  VEC4INLINE f32& operator[](u32 index);
  VEC4INLINE const f32& operator[](u32 index) const;

  VEC4INLINE Vector4 operator-(void) const;
  VEC4INLINE Vector4 operator+(void) const;

  VEC4INLINE f32 Length(void) const;
  VEC4INLINE f32 LengthSq(void) const;
  VEC4INLINE f32 Magnitude(void) const;
  VEC4INLINE f32 MagnitudeSq(void) const;
  VEC4INLINE Vector4& Normalize(void);
  VEC4INLINE Vector4 Normalized(void) const;
  VEC4INLINE f32 Dot(const Vector4& rhs) const;
  VEC4INLINE Vector4 Cross(const Vector4& rhs) const;
  VEC4INLINE void Zero(void);
  friend std::ostream& operator<< (std::ostream& out, Vector4 const& v4 );
};
std::ostream& operator<< (std::ostream& out, Vector4 const& v4 );

typedef Vector4 float4;
typedef Vector4 vec4;

} // namespace Virtus

#include "vector4.inl"

#endif // Include Guard