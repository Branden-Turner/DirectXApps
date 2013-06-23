/*! ====================================================================================================================
  @file ScriptVec2.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    ScriptVec2 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIME_SCRIPTV2_H
#define GRIME_SCRIPTV2_H

namespace Grimelins {

struct Vector2;
typedef Vector2 vec2;

SCRIPT_HeaderDefine(ScriptVec2);

class ScriptVec2
{
public:
  union
  {
    struct { f32 x,y; };
    f32 v[2];
  };
  
  ScriptVec2(void);
  ScriptVec2(const ScriptVec2& rhs);
  ScriptVec2(const vec2& rhs);
  ScriptVec2(f32 xx, f32 yy);

  ScriptVec2& operator=(const ScriptVec2& rhs);

  ScriptVec2 operator+(f32 rhs) const;
  ScriptVec2 operator-(f32 rhs) const;
  ScriptVec2 operator*(f32 rhs) const;
  ScriptVec2 operator/(f32 rhs) const;
  ScriptVec2& operator+=(f32 rhs);
  ScriptVec2& operator-=(f32 rhs);
  ScriptVec2& operator*=(f32 rhs);
  ScriptVec2& operator/=(f32 rhs);

  ScriptVec2 operator+(const ScriptVec2& rhs) const;
  ScriptVec2 operator-(const ScriptVec2& rhs) const;
  ScriptVec2 operator*(const ScriptVec2& rhs) const;
  ScriptVec2 operator/(const ScriptVec2& rhs) const;
  ScriptVec2& operator+=(const ScriptVec2& rhs);
  ScriptVec2& operator-=(const ScriptVec2& rhs);
  ScriptVec2& operator*=(const ScriptVec2& rhs);
  ScriptVec2& operator/=(const ScriptVec2& rhs);

  f32& operator[](u32 index);
  const f32& operator[](u32 index) const;

  ScriptVec2 operator-(void) const;
  ScriptVec2 operator+(void) const;

  f32 Length(void) const;
  f32 LengthSq(void) const;
  f32 Magnitude(void) const;
  f32 MagnitudeSq(void) const;
  ScriptVec2& Normalize( void );
  ScriptVec2 Normalized( void ) const;
  f32 Dot(const ScriptVec2& rhs) const;
  void Set(f32 xx, f32 yy);
  void Zero(void);
};

typedef ScriptVec2    svec2;

} // namespace Virtus


#endif // Include Guard