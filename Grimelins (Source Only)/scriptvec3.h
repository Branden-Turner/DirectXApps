/*! ====================================================================================================================
  @file ScriptVec3.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    ScriptVec3 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIME_SCRIPTV3_H
#define GRIME_SCRIPTV3_H

namespace Grimelins {

  class Quaternion;
  struct Vector3;
  typedef Vector3 vec3;

  SCRIPT_HeaderDefine(ScriptVec3);

class ScriptVec3
{
public:
  union
  {
    struct { f32 x,y,z; };
    f32 v[3];
  };

  ScriptVec3(void);
  ScriptVec3(const ScriptVec3& rhs);
  ScriptVec3(const vec3& rhs);
  ScriptVec3(f32 xx, f32 yy, f32 zz);

  ScriptVec3& operator=(const ScriptVec3& rhs);

  ScriptVec3 operator+(f32 rhs) const;
  ScriptVec3 operator-(f32 rhs) const;
  ScriptVec3 operator*(f32 rhs) const;
  ScriptVec3 operator/(f32 rhs) const;
  ScriptVec3& operator+=(f32 rhs);
  ScriptVec3& operator-=(f32 rhs);
  ScriptVec3& operator*=(f32 rhs);
  ScriptVec3& operator/=(f32 rhs);

  ScriptVec3 operator+(const ScriptVec3& rhs) const;
  ScriptVec3 operator-(const ScriptVec3& rhs) const;
  ScriptVec3 operator*(const ScriptVec3& rhs) const;
  ScriptVec3 operator/(const ScriptVec3& rhs) const;
  ScriptVec3& operator+=(const ScriptVec3& rhs);
  ScriptVec3& operator-=(const ScriptVec3& rhs);
  ScriptVec3& operator*=(const ScriptVec3& rhs);
  ScriptVec3& operator/=(const ScriptVec3& rhs);

  f32& operator[](u32 index);
  const f32& operator[](u32 index) const;

  ScriptVec3 operator-(void) const;
  ScriptVec3 operator+(void) const;

  f32 Length(void) const;
  f32 LengthSq(void) const;
  f32 Magnitude(void) const;
  f32 MagnitudeSq(void) const;
  ScriptVec3& Normalize(void);
  ScriptVec3 Normalized(void) const;
  f32 Dot(const ScriptVec3& rhs) const;
  ScriptVec3 Cross(const ScriptVec3& rhs) const;
  void Set(f32 xx, f32 yy, f32 zz);
  void Zero(void);
  ScriptVec3 RotateWithQuat(const Quaternion& quat);
  bool isParallelWith(const ScriptVec3& vec) const;
};

typedef ScriptVec3    svec3;

} // namespace Virtus

#endif // Include Guard


