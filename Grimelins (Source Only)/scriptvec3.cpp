/*! ====================================================================================================================
  @file ScriptVec3.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.06.2011

  @brief 
    ScriptVec3 Math Library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "quaternion.h"
#include "scriptvec3.h"



namespace Grimelins {
  SCRIPT_ClassStart(ScriptVec3);
  SCRIPT_CTOR_Enabled();
  SCRIPT_PlainData(x);
  SCRIPT_PlainData(y);
  SCRIPT_PlainData(z);
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
  SCRIPT_Method(RotateWithQuat);
  SCRIPT_Method(isParallelWith);



  //Squirrel syntax : someVector = myVector.subF(3);  is equivalent to someVector = myVector - 3;
  SCRIPT_MethodOverload(operator-, "subF", ScriptVec3, (f32) const);
  SCRIPT_MethodOverload(operator+, "addF", ScriptVec3, (f32) const);
  SCRIPT_MethodOverload(operator*, "mulF", ScriptVec3, (f32) const);
  SCRIPT_MethodOverload(operator/, "divF", ScriptVec3, (f32) const);

  //Squirrel syntax : myVector.subWithF(3);   is equivalent to myVector -= 3;
  SCRIPT_MethodOverload(operator-=, "subWithF", ScriptVec3&, (f32) );
  SCRIPT_MethodOverload(operator+=, "addWithF", ScriptVec3&, (f32) );
  SCRIPT_MethodOverload(operator*=, "mulWithF", ScriptVec3&, (f32) );
  SCRIPT_MethodOverload(operator/=, "divWithF", ScriptVec3&, (f32) );

  //Squirrel syntax : someVector = myVector - anotherVector;  is equivalent to someVector = myVector - anotherVector;
  SCRIPT_MethodOverload(operator-, "_sub", ScriptVec3, (const ScriptVec3&) const);
  SCRIPT_MethodOverload(operator+, "_add", ScriptVec3, (const ScriptVec3&) const);
  SCRIPT_MethodOverload(operator*, "_mul", ScriptVec3, (const ScriptVec3&) const);
  SCRIPT_MethodOverload(operator/, "_div", ScriptVec3, (const ScriptVec3&) const);

  //Squirrel syntax : myVector.subWith(someVector);  is equivalent to myVector -= someVector;
  SCRIPT_MethodOverload(operator-=, "subWith", ScriptVec3&, (const ScriptVec3&) );
  SCRIPT_MethodOverload(operator+=, "addWith", ScriptVec3&, (const ScriptVec3&) );
  SCRIPT_MethodOverload(operator*=, "mulWith", ScriptVec3&, (const ScriptVec3&) );
  SCRIPT_MethodOverload(operator/=, "divWith", ScriptVec3&, (const ScriptVec3&) );
  
  //unary negation
  SCRIPT_MethodOverload(operator-, "_unm", ScriptVec3, (void) const);


  SCRIPT_ClassEnd();

 ScriptVec3::ScriptVec3(void)
  : x(0.0f), y(0.0f), z(0.0f)
{
}

 ScriptVec3::ScriptVec3(const ScriptVec3& rhs)
  : x(rhs.x), y(rhs.y), z(rhs.z)
{
}

 ScriptVec3::ScriptVec3(f32 xx, f32 yy, f32 zz)
  : x(xx), y(yy), z(zz)
{
}

 ScriptVec3::ScriptVec3( const vec3& rhs )
   : x(rhs.x), y(rhs.y), z(rhs.z)
 {
 }

 ScriptVec3& ScriptVec3::operator=(const ScriptVec3& rhs)
{
  if( this != &rhs )
    std::memcpy( v, rhs.v, sizeof(v) );

  return *this;
}

 ScriptVec3 ScriptVec3::operator+(f32 rhs) const
{
  return ScriptVec3(x + rhs, y + rhs, z + rhs);
}

 ScriptVec3 ScriptVec3::operator-(f32 rhs) const
{
  return ScriptVec3(x - rhs, y - rhs, z - rhs);
}

 ScriptVec3 ScriptVec3::operator*(f32 rhs) const
{
  return ScriptVec3(x * rhs, y * rhs, z * rhs);
}

 ScriptVec3 ScriptVec3::operator/(f32 rhs) const
{
  return ScriptVec3(x / rhs, y / rhs, z / rhs);
}

 ScriptVec3& ScriptVec3::operator+=(f32 rhs)
{
  x += rhs;
  y += rhs;
  z += rhs;
  return *this;
}

 ScriptVec3& ScriptVec3::operator-=(f32 rhs)
{
  x -= rhs;
  y -= rhs;
  z -= rhs;
  return *this;
}

 ScriptVec3& ScriptVec3::operator*=(f32 rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

 ScriptVec3& ScriptVec3::operator/=(f32 rhs)
{
  x /= rhs;
  y /= rhs;
  z /= rhs;
  return *this;
}

 ScriptVec3 ScriptVec3::operator+(const ScriptVec3& rhs) const
{
  return ScriptVec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

 ScriptVec3 ScriptVec3::operator-(const ScriptVec3& rhs) const
{
  return ScriptVec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

 ScriptVec3 ScriptVec3::operator*(const ScriptVec3& rhs) const
{
  return ScriptVec3(x * rhs.x, y * rhs.y, z * rhs.z);
}

 ScriptVec3 ScriptVec3::operator/(const ScriptVec3& rhs) const
{
  return ScriptVec3(x / rhs.x, y / rhs.y, z / rhs.z);
}

 ScriptVec3& ScriptVec3::operator+=(const ScriptVec3& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

 ScriptVec3& ScriptVec3::operator-=(const ScriptVec3& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

 ScriptVec3& ScriptVec3::operator*=(const ScriptVec3& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  return *this;
}

 ScriptVec3& ScriptVec3::operator/=(const ScriptVec3& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  return *this;
}

 f32& ScriptVec3::operator[](u32 index)
{
  ErrorIf(index > 2, "Index Outside of Range: ScriptVec3::operator[]");

  return v[index];
}

 const f32& ScriptVec3::operator[](u32 index) const
{
  ErrorIf(index > 2, "Index Outside of Range: ScriptVec3::operator[]");

  return v[index];
}

 ScriptVec3 ScriptVec3::operator-(void) const
{
  return ScriptVec3(-x, -y, -z);
}

 ScriptVec3 ScriptVec3::operator+(void) const
{
  return *this;
}

 f32 ScriptVec3::Length(void) const
{
  return sqrtf(x*x + y*y + z*z);
}

 f32 ScriptVec3::LengthSq(void) const
{
  return (x*x + y*y + z*z);
}

 f32 ScriptVec3::Magnitude(void) const
{
  return sqrtf(x*x + y*y + z*z);
}

 f32 ScriptVec3::MagnitudeSq(void) const
{
  return (x*x + y*y + z*z);
}

 f32 ScriptVec3::Dot(const ScriptVec3& rhs) const
{
  return (x * rhs.x + y * rhs.y + z * rhs.z);
}

 ScriptVec3 ScriptVec3::Cross(const ScriptVec3& rhs) const
{
  return ScriptVec3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

 void ScriptVec3::Zero(void)
{
  x = y = z = 0.0f;
}

 ScriptVec3& ScriptVec3::Normalize( void )
{
  f32 len = Magnitude();

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

 ScriptVec3 ScriptVec3::Normalized( void ) const
{
  f32 len = Magnitude();
  if(len == 0)
    return ScriptVec3();
  else
    return ScriptVec3(x / len, y / len, z / len);

}

void ScriptVec3::Set(f32 xx, f32 yy, f32 zz)
{
  x = xx; y = yy; z = zz;
}

 ScriptVec3 ScriptVec3::RotateWithQuat(const Quaternion& quat)
{
  // convert the vector into a quaternion
  vec3 rot;
  rot.x = x;
  rot.y = y;
  rot.z = z;
  Quaternion vectQuat(0,rot);

  // compute the quaternion rotation result
  Quaternion quatResult = (quat * vectQuat) * quat.inverse();

  // convert the result quaternion into a vector
  rot = quatResult.vector3();

  return ScriptVec3(rot.x, rot.y, rot.z);
}

 bool ScriptVec3::isParallelWith(const ScriptVec3& vec) const
{
    f32 scalProd = Dot(vec);
    f32 a = std::abs(scalProd);
    f32 b = Length() * vec.Length();
    f32 epsilon = .00001f;
    return ((a-b) < epsilon && (a-b) > -epsilon);
}

} /// namespace Virtus


