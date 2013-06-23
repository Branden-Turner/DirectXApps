/*! ====================================================================================================================
  @file ScriptVec2.inl

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    ScriptVec2 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "scriptvec2.h"


namespace Grimelins {

SCRIPT_ClassStart(ScriptVec2);
SCRIPT_CTOR_Enabled();
SCRIPT_PlainData(x);
SCRIPT_PlainData(y);
SCRIPT_Method(Length);
SCRIPT_Method(LengthSq);
SCRIPT_Method(Magnitude);
SCRIPT_Method(MagnitudeSq);
SCRIPT_Method(Normalize);
SCRIPT_Method(Normalized);
SCRIPT_Method(Dot);
SCRIPT_Method(Set);
SCRIPT_Method(Zero);

//Squirrel syntax : someVector = myVector.subF(3);  is equivalent to someVector = myVector - 3;
SCRIPT_MethodOverload(operator-, "subF", ScriptVec2, (f32) const);
SCRIPT_MethodOverload(operator+, "addF", ScriptVec2, (f32) const);
SCRIPT_MethodOverload(operator*, "mulF", ScriptVec2, (f32) const);
SCRIPT_MethodOverload(operator/, "divF", ScriptVec2, (f32) const);

//Squirrel syntax : myVector.subWithF(3);   is equivalent to myVector -= 3;
SCRIPT_MethodOverload(operator-=, "subWithF", ScriptVec2&, (f32) );
SCRIPT_MethodOverload(operator+=, "addWithF", ScriptVec2&, (f32) );
SCRIPT_MethodOverload(operator*=, "mulWithF", ScriptVec2&, (f32) );
SCRIPT_MethodOverload(operator/=, "divWithF", ScriptVec2&, (f32) );

//Squirrel syntax : someVector = myVector - anotherVector;  is equivalent to someVector = myVector - anotherVector;
SCRIPT_MethodOverload(operator-, "_sub", ScriptVec2, (const ScriptVec2&) const);
SCRIPT_MethodOverload(operator+, "_add", ScriptVec2, (const ScriptVec2&) const);
SCRIPT_MethodOverload(operator*, "_mul", ScriptVec2, (const ScriptVec2&) const);
SCRIPT_MethodOverload(operator/, "_div", ScriptVec2, (const ScriptVec2&) const);

//Squirrel syntax : myVector.subWith(someVector);  is equivalent to myVector -= someVector;
SCRIPT_MethodOverload(operator-=, "subWith", ScriptVec2&, (const ScriptVec2&) );
SCRIPT_MethodOverload(operator+=, "addWith", ScriptVec2&, (const ScriptVec2&) );
SCRIPT_MethodOverload(operator*=, "mulWith", ScriptVec2&, (const ScriptVec2&) );
SCRIPT_MethodOverload(operator/=, "divWith", ScriptVec2&, (const ScriptVec2&) );

//unary negation
SCRIPT_MethodOverload(operator-, "_unm", ScriptVec2, (void) const);


//_add, _sub, _mul, _div, _unm, _modulo, _set, _get, _typeof, _nexti, _cmp, _call, _delslot,_tostring
SCRIPT_ClassEnd();

 ScriptVec2::ScriptVec2(void)
  : x(0.0f), y(0.0f)
{
}

 ScriptVec2::ScriptVec2(const ScriptVec2& rhs)
  : x(rhs.x), y(rhs.y)
{
}

 ScriptVec2::ScriptVec2(f32 xx, f32 yy)
  : x(xx), y(yy)
{
}

 ScriptVec2::ScriptVec2( const vec2& rhs )
   : x(rhs.x), y(rhs.y)
 {
 }

 ScriptVec2& ScriptVec2::operator=(const ScriptVec2& rhs)
{
  if( this != &rhs )
    std::memcpy( v, rhs.v, sizeof(v) );

  return *this;
}

 ScriptVec2 ScriptVec2::operator+(f32 rhs) const
{
  return ScriptVec2(x + rhs, y + rhs);
}

 ScriptVec2 ScriptVec2::operator-(f32 rhs) const
{
  return ScriptVec2(x - rhs, y - rhs);
}

 ScriptVec2 ScriptVec2::operator*(f32 rhs) const
{
  return ScriptVec2(x * rhs, y * rhs);
}

 ScriptVec2 ScriptVec2::operator/(f32 rhs) const
{
  return ScriptVec2(x / rhs, y / rhs);
}

 ScriptVec2& ScriptVec2::operator+=(f32 rhs)
{
  x += rhs;
  y += rhs;
  return *this;
}

 ScriptVec2& ScriptVec2::operator-=(f32 rhs)
{
  x -= rhs;
  y -= rhs;
  return *this;
}

 ScriptVec2& ScriptVec2::operator*=(f32 rhs)
{
  x *= rhs;
  y *= rhs;
  return *this;
}

 ScriptVec2& ScriptVec2::operator/=(f32 rhs)
{
  x /= rhs;
  y /= rhs;
  return *this;
}

 ScriptVec2 ScriptVec2::operator+(const ScriptVec2& rhs) const
{
  return ScriptVec2(x + rhs.x, y + rhs.y);
}

 ScriptVec2 ScriptVec2::operator-(const ScriptVec2& rhs) const
{
  return ScriptVec2(x - rhs.x, y - rhs.y);
}

 ScriptVec2 ScriptVec2::operator*(const ScriptVec2& rhs) const
{
  return ScriptVec2(x * rhs.x, y * rhs.y);
}

 ScriptVec2 ScriptVec2::operator/(const ScriptVec2& rhs) const
{
  return ScriptVec2(x / rhs.x, y / rhs.y);
}

 ScriptVec2& ScriptVec2::operator+=(const ScriptVec2& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

 ScriptVec2& ScriptVec2::operator-=(const ScriptVec2& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

 ScriptVec2& ScriptVec2::operator*=(const ScriptVec2& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  return *this;
}

 ScriptVec2& ScriptVec2::operator/=(const ScriptVec2& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  return *this;
}

 f32& ScriptVec2::operator[](u32 index)
{
  ErrorIf(index > 1, "Index Outside of Range: ScriptVec2::operator[]");

  return v[index];
}

 const f32& ScriptVec2::operator[](u32 index) const
{
  ErrorIf(index > 1, "Index Outside of Range: ScriptVec2::operator[]");

  return v[index];
}

 ScriptVec2 ScriptVec2::operator-(void) const
{
  return ScriptVec2(-x, -y);
}

 ScriptVec2 ScriptVec2::operator+(void) const
{
  return *this;
}

 f32 ScriptVec2::Length(void) const
{
  return sqrtf(x*x + y*y);
}

 f32 ScriptVec2::LengthSq(void) const
{
  return (x*x + y*y);
}

 f32 ScriptVec2::Magnitude(void) const
{
  return sqrtf(x*x + y*y);
}

 f32 ScriptVec2::MagnitudeSq(void) const
{
  return (x*x + y*y);
}

 ScriptVec2& ScriptVec2::Normalize( void )
{
  float len = Magnitude();

  if(len == 0)
    Zero();
  else
  {
    x /= len;
    y /= len;
  }

  return *this;
}

 ScriptVec2 ScriptVec2::Normalized( void ) const
{
  float len = Magnitude();
  if(len == 0)
    return ScriptVec2();
  else
    return ScriptVec2(x / len, y / len);
 }

void ScriptVec2::Set(f32 xx, f32 yy)
{
  x = xx; y = yy;
}

 f32 ScriptVec2::Dot(const ScriptVec2& rhs) const
{
  return (x * rhs.x + y * rhs.x);
}

 void ScriptVec2::Zero(void)
{
  std::memset( v, 0, sizeof(v) );
}

} /// namespace Virtus#include "precompiled.h"