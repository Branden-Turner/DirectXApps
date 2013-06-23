/*! ====================================================================================================================
  @file ScriptVec4.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @author Branden Turner (adapting Joe's work to work with script interface)
  @date Jun.06.2011

  @brief 
    ScriptVec4 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIME_SCRIPTV4_H
#define GRIME_SCRIPTV4_H

namespace Grimelins {

  struct Vector4;
  typedef Vector4 vec4;

  SCRIPT_HeaderDefine(ScriptVec4);

class ScriptVec4
{
public:
  union
  {
    struct { f32 x,y,z,w; };
    f32 v[4];
  };

  ScriptVec4(void);
  ScriptVec4(const ScriptVec4& rhs);
  ScriptVec4(const vec4& rhs);
  ScriptVec4(f32 xx, f32 yy, f32 zz, f32 ww);

  ScriptVec4& operator=(const ScriptVec4& rhs);

  ScriptVec4 operator+(f32 rhs) const;
  ScriptVec4 operator-(f32 rhs) const;
  ScriptVec4 operator*(f32 rhs) const;
  ScriptVec4 operator/(f32 rhs) const;
  ScriptVec4& operator+=(f32 rhs);
  ScriptVec4& operator-=(f32 rhs);
  ScriptVec4& operator*=(f32 rhs);
  ScriptVec4& operator/=(f32 rhs);

  ScriptVec4 operator+(const ScriptVec4& rhs) const;
  ScriptVec4 operator-(const ScriptVec4& rhs) const;
  ScriptVec4 operator*(const ScriptVec4& rhs) const;
  ScriptVec4 operator/(const ScriptVec4& rhs) const;
  ScriptVec4& operator+=(const ScriptVec4& rhs);
  ScriptVec4& operator-=(const ScriptVec4& rhs);
  ScriptVec4& operator*=(const ScriptVec4& rhs);
  ScriptVec4& operator/=(const ScriptVec4& rhs);

  f32& operator[](u32 index);
  const f32& operator[](u32 index) const;

  ScriptVec4 operator-(void) const;
  ScriptVec4 operator+(void) const;

  f32 Length(void) const;
  f32 LengthSq(void) const;
  f32 Magnitude(void) const;
  f32 MagnitudeSq(void) const;
  ScriptVec4& Normalize(void);
  ScriptVec4 Normalized(void) const;
  f32 Dot(const ScriptVec4& rhs) const;
  ScriptVec4 Cross(const ScriptVec4& rhs) const;
  void Set(f32 xx, f32 yy, f32 zz, f32 ww);
  void Zero(void);
};

typedef ScriptVec4    svec4;

} // namespace Virtus

#endif // Include Guard