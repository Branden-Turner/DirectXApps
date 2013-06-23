/*! ====================================================================================================================
  @file ScriptVec4.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @author Branden Turner (adapting Joe's work to work with script interface)
  @date Jun.06.2011

  @brief 
    ScriptVec4 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "scriptvec4.h"

namespace Grimelins {

  SCRIPT_ClassStart(ScriptVec4);
  SCRIPT_CTOR_Enabled();
  SCRIPT_PlainData(x);
  SCRIPT_PlainData(y);
  SCRIPT_PlainData(z);
  SCRIPT_PlainData(w);
  SCRIPT_Method(Length);
  SCRIPT_Method(LengthSq);
  SCRIPT_Method(Magnitude);
  SCRIPT_Method(MagnitudeSq);
  SCRIPT_Method(Normalize);
  SCRIPT_Method(Normalized);
  SCRIPT_Method(Dot);
  SCRIPT_Method(Cross);  
  SCRIPT_Method(Set);
  SCRIPT_Method(Zero);


  //Squirrel syntax : someVector = myVector.subF(3);  is equivalent to someVector = myVector - 3;
  SCRIPT_MethodOverload(operator-, "subF", ScriptVec4, (f32) const);
  SCRIPT_MethodOverload(operator+, "addF", ScriptVec4, (f32) const);
  SCRIPT_MethodOverload(operator*, "mulF", ScriptVec4, (f32) const);
  SCRIPT_MethodOverload(operator/, "divF", ScriptVec4, (f32) const);

  //Squirrel syntax : myVector.subWithF(3);   is equivalent to myVector -= 3;
  SCRIPT_MethodOverload(operator-=, "subWithF", ScriptVec4&, (f32) );
  SCRIPT_MethodOverload(operator+=, "addWithF", ScriptVec4&, (f32) );
  SCRIPT_MethodOverload(operator*=, "mulWithF", ScriptVec4&, (f32) );
  SCRIPT_MethodOverload(operator/=, "divWithF", ScriptVec4&, (f32) );

  //Squirrel syntax : someVector = myVector - anotherVector;  is equivalent to someVector = myVector - anotherVector;
  SCRIPT_MethodOverload(operator-, "_sub", ScriptVec4, (const ScriptVec4&) const);
  SCRIPT_MethodOverload(operator+, "_add", ScriptVec4, (const ScriptVec4&) const);
  SCRIPT_MethodOverload(operator*, "_mul", ScriptVec4, (const ScriptVec4&) const);
  SCRIPT_MethodOverload(operator/, "_div", ScriptVec4, (const ScriptVec4&) const);

  //Squirrel syntax : myVector.subWith(someVector);  is equivalent to myVector -= someVector;
  SCRIPT_MethodOverload(operator-=, "subWith", ScriptVec4&, (const ScriptVec4&) );
  SCRIPT_MethodOverload(operator+=, "addWith", ScriptVec4&, (const ScriptVec4&) );
  SCRIPT_MethodOverload(operator*=, "mulWith", ScriptVec4&, (const ScriptVec4&) );
  SCRIPT_MethodOverload(operator/=, "divWith", ScriptVec4&, (const ScriptVec4&) );

  //unary negation
  SCRIPT_MethodOverload(operator-, "_unm", ScriptVec4, (void) const);

  SCRIPT_ClassEnd();

  ScriptVec4::ScriptVec4(void)
    : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
  {
  }

  ScriptVec4::ScriptVec4(const ScriptVec4& rhs)
    : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
  {
  }

  ScriptVec4::ScriptVec4(f32 xx, f32 yy, f32 zz, f32 ww)
    : x(xx), y(yy), z(zz), w(ww)
  {
  }

  ScriptVec4::ScriptVec4( const vec4& rhs )
    : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
  {
  }

  ScriptVec4& ScriptVec4::operator=(const ScriptVec4& rhs)
  {
    if( this != &rhs )
      std::memcpy( v, rhs.v, sizeof(v) );

    return *this;
  }

  ScriptVec4 ScriptVec4::operator+(f32 rhs) const
  {
    return ScriptVec4(x + rhs, y + rhs, z + rhs, w + rhs);
  }

  ScriptVec4 ScriptVec4::operator-(f32 rhs) const
  {
    return ScriptVec4(x - rhs, y - rhs, z - rhs, w - rhs);
  }

  ScriptVec4 ScriptVec4::operator*(f32 rhs) const
  {
    return ScriptVec4(x * rhs, y * rhs, z * rhs, w * rhs);
  }

  ScriptVec4 ScriptVec4::operator/(f32 rhs) const
  {
    return ScriptVec4(x / rhs, y / rhs, z / rhs, w / rhs);
  }

  ScriptVec4& ScriptVec4::operator+=(f32 rhs)
  {
    x += rhs;
    y += rhs;
    z += rhs;
    w += rhs;
    return *this;
  }

  ScriptVec4& ScriptVec4::operator-=(f32 rhs)
  {
    x -= rhs;
    y -= rhs;
    z -= rhs;
    w -= rhs;
    return *this;
  }

  ScriptVec4& ScriptVec4::operator*=(f32 rhs)
  {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;
    return *this;
  }

  ScriptVec4& ScriptVec4::operator/=(f32 rhs)
  {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    w /= rhs;
    return *this;
  }

  ScriptVec4 ScriptVec4::operator+(const ScriptVec4& rhs) const
  {
    return ScriptVec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
  }

  ScriptVec4 ScriptVec4::operator-(const ScriptVec4& rhs) const
  {
    return ScriptVec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
  }

  ScriptVec4 ScriptVec4::operator*(const ScriptVec4& rhs) const
  {
    return ScriptVec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
  }

  ScriptVec4 ScriptVec4::operator/(const ScriptVec4& rhs) const
  {
    return ScriptVec4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
  }

  ScriptVec4& ScriptVec4::operator+=(const ScriptVec4& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
  }

  ScriptVec4& ScriptVec4::operator-=(const ScriptVec4& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
  }

  ScriptVec4& ScriptVec4::operator*=(const ScriptVec4& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
  }

  ScriptVec4& ScriptVec4::operator/=(const ScriptVec4& rhs)
  {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
  }

  f32& ScriptVec4::operator[](u32 index)
  {
    ErrorIf(index > 3, "Index Outside of Range: ScriptVec4::operator[]");

    return v[index];
  }

  const f32& ScriptVec4::operator[](u32 index) const
  {
    ErrorIf(index > 3, "Index Outside of Range: ScriptVec4::operator[]");

    return v[index];
  }

  ScriptVec4 ScriptVec4::operator-(void) const
  {
    return ScriptVec4(-x, -y, -z, -w);
  }

  ScriptVec4 ScriptVec4::operator+(void) const
  {
    return *this;
  }

  f32 ScriptVec4::Length(void) const
  {
    return sqrtf(x*x + y*y + z*z + w*w);
  }

  f32 ScriptVec4::LengthSq(void) const
  {
    return (x*x + y*y + z*z + w*w);
  }

  f32 ScriptVec4::Magnitude(void) const
  {
    return sqrtf(x*x + y*y + z*z + w*w);
  }

  f32 ScriptVec4::MagnitudeSq(void) const
  {
    return (x*x + y*y + z*z + w*w);
  }

  f32 ScriptVec4::Dot(const ScriptVec4& rhs) const
  {
    return (x * rhs.x + y * rhs.x + z * rhs.z + w * rhs.w);
  }

  ScriptVec4 ScriptVec4::Cross(const ScriptVec4& rhs) const
  {
    return ScriptVec4(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x, 0.0f);
  }

  ScriptVec4& ScriptVec4::Normalize( void )
  {
    float len = Magnitude();

    if(len == 0)
      Zero();
    else
    {
      x /= len;
      y /= len;
      z /= len;
    }

    return *this;
  }

  ScriptVec4 ScriptVec4::Normalized( void ) const
  {
    float len = Magnitude();
    if(len == 0)
      return ScriptVec4();
    else
      return ScriptVec4(x / len, y / len, z / len, 0.0f);
  }

  void ScriptVec4::Set(f32 xx, f32 yy, f32 zz, f32 ww)
  {
    x = xx; y = yy; z = zz; w = ww;
  }

  void ScriptVec4::Zero(void)
  {
    std::memset( v, 0, sizeof(v) );
  }

} /// namespace Virtus