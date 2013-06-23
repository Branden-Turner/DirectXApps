/*! ====================================================================================================================
  @file vector3.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    Vector3 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once


#ifndef VEC3INLINE
  #if(VIRTUS_CONFIG_DEBUG_MODE)
    #define VEC3INLINE inline
  #else
    #define VEC3INLINE __forceinline
  #endif
#endif

namespace Grimelins {

  class Quaternion;

struct Vector3
{
  union
  {
    struct { f32 x,y,z; };
    f32 v[3];
  };

  VEC3INLINE Vector3(void);
  VEC3INLINE Vector3(const Vector3& rhs);
  VEC3INLINE Vector3(const ScriptVec3& rhs);
  VEC3INLINE Vector3(f32 xx, f32 yy, f32 zz);

  VEC3INLINE Vector3& operator=(const Vector3& rhs);

  VEC3INLINE Vector3 operator+(f32 rhs) const;
  VEC3INLINE Vector3 operator-(f32 rhs) const;
  VEC3INLINE Vector3 operator*(f32 rhs) const;
  VEC3INLINE Vector3 operator/(f32 rhs) const;
  VEC3INLINE Vector3& operator+=(f32 rhs);
  VEC3INLINE Vector3& operator-=(f32 rhs);
  VEC3INLINE Vector3& operator*=(f32 rhs);
  VEC3INLINE Vector3& operator/=(f32 rhs);

  VEC3INLINE Vector3 operator+(const Vector3& rhs) const;
  VEC3INLINE Vector3 operator-(const Vector3& rhs) const;
  VEC3INLINE Vector3 operator*(const Vector3& rhs) const;
  VEC3INLINE Vector3 operator/(const Vector3& rhs) const;
  VEC3INLINE Vector3& operator+=(const Vector3& rhs);
  VEC3INLINE Vector3& operator-=(const Vector3& rhs);
  VEC3INLINE Vector3& operator*=(const Vector3& rhs);
  VEC3INLINE Vector3& operator/=(const Vector3& rhs);

  VEC3INLINE bool operator==(const Vector3& rhs);
  VEC3INLINE bool operator!=(const Vector3& rhs);

  VEC3INLINE f32& operator[](u32 index);
  VEC3INLINE const f32& operator[](u32 index) const;

  VEC3INLINE Vector3 operator-(void) const;
  VEC3INLINE Vector3 operator+(void) const;

  VEC3INLINE f32 Length(void) const;
  VEC3INLINE f32 LengthSq(void) const;
  VEC3INLINE f32 Magnitude(void) const;
  VEC3INLINE f32 MagnitudeSq(void) const;
  VEC3INLINE Vector3& Normalize(void);
  VEC3INLINE Vector3 Normalized(void) const;
  VEC3INLINE f32 Dot(const Vector3& rhs) const;
  VEC3INLINE Vector3 Cross(const Vector3& rhs) const;
  VEC3INLINE void Zero(void);
  VEC3INLINE Vector3 RotateWithQuat(const Quaternion& quat);
  VEC3INLINE bool isParallelWith(const Vector3& vec) const;
  friend std::ostream& operator<< (std::ostream& out, Vector3 const& v3 );
};

typedef Vector3 float3;
typedef Vector3 vec3;
std::ostream& operator<< (std::ostream& out, Vector3 const& v3 );

} // namespace Virtus

#include "vector3.inl"


