/*! ====================================================================================================================
  @file flextype.cpp

  Codename Virtus

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>

  @date May.18.2011

  @brief 
    A flexible data type that cares very little for what type of value you store and in what context you use it; all
    together not very picky. The primary use is to have a type that can readily adapt to changing use context,
    especially when C++ must interface with loosely typed languages like Lua, operate on strings read from text files
    as though they were numerical data, and other game logic applications.

    An example of its flexibility is to assign a string from a text file "2,3,4,5" and assume it is a 4d vector, then 
    add it to a position in the engine (an actual vector type), then compute the resultant length (a vector operation)
    and assign the resulting length back to the FlexType (now a double) and save it back into some file (as a string).
    As you can see the flexibility can be quite handy, just remember it comes at the price of memory and speed!

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "flextype.h"


namespace Grimelins
{

// Stream insertion operator. This will dump all of the value states of the FlexType. If you instead desire to dump
// a single value, say the integer form simply typecast the FlexType to that type, which will then use the stream
// insertion of the that type instead.
std::ostream& operator<<(std::ostream & os, FlexType const& f)
{
#ifdef VIRTUS_FLEXTYPE_DEBUG_STREAM
  os << "\nstring: \"" << f.m_string << "\"\n";
  os << "   s32: " << f.m_int << "\n";
  os << "   u32: " << f.m_unsigned << "\n";
  os << "   f64: " << f.m_double << "\n";
  os << "  Vec4: " << f.m_vec4.x << ", " << f.m_vec4.y << ", " << f.m_vec4.z << ", " << f.m_vec4.w << "\n";
  if(f.isNumeric(false))
  {
    if(f.isNumeric(true))
      os << " Is absolutely numeric.";
    else
      os << " Is numeric, but contains some non-numeric string characters.";
  }
  else
    os << " Is not numeric what-so-ever, purely a string.";
  
  if(f.m_knownVectorLength)
    os << "Root data is a vector of size " << f.m_knownVectorLength << ".";
  else
    os << "Root data is not a vector.";
   os  << "\n\n";;
#else
  os << f.m_string;
#endif
  return os;
}

// Stream extraction operator. This will dump all of the value states of the FlexType. If you instead desire to dump
// a single value, say the integer form simply typecast the FlexType to that type, which will then use the stream
// insertion of the that type instead.
std::istream& operator>>(std::istream & is, FlexType& f)
{
  std::stringstream s;
  s << is;
  f.m_string = s.str();
  f.flex();
  return is;
}


FlexType::FlexType( std::string s)
:m_string(s)
{
  flex();
}

FlexType::FlexType( s32 i )
{
  std::stringstream source;
  source << i;
  m_string = source.str();
  flex();
}

FlexType::FlexType( u32 u )
{
  std::stringstream source;
  source << u;
  m_string = source.str();
  flex();
}

FlexType::FlexType( f64 d )
{
  std::stringstream source;
  source << d;
  m_string = source.str();
  flex();
}

FlexType::FlexType( vec2 const& v )
{
  std::stringstream source;
  source << v.x << "," << v.y;
  m_string = source.str();
  flex();
}

FlexType::FlexType( vec3 const& v )
{
  std::stringstream source;
  source << v.x << "," << v.y << "," << v.z;
  m_string = source.str();
  flex();
}

FlexType::FlexType( vec4 const& v )
{
  std::stringstream source;
  source << v.x << "," << v.y << "," << v.z << "," << v.w;
  m_string = source.str();
  flex();
}

FlexType::FlexType( FlexType const& f )
: m_string(f.m_string), 
  m_int(f.m_int),
  m_unsigned(f.m_unsigned), 
  m_double(f.m_double), 
  m_vec4(f.m_vec4)
{/*body of FlexType CTOR*/}

FlexType::~FlexType( void )
{

}
//conversion to C style character strings
FlexType::operator s8 const* (void) const
{
  return m_string.c_str();
}

//conversion to string
FlexType::operator std::string(void) const
{
  return m_string;
}

//conversion to 32-bit signed integers
FlexType::operator s32(void) const
{
  return m_int;
}

//conversion to 32-bit unsigned integers
FlexType::operator u32(void) const
{
  return m_unsigned;
}

//conversion to 32-bit floating point
FlexType::operator f32(void) const
{
  return SCAST(f32, m_double);
}

//conversion to 64-bit floating point
FlexType::operator f64(void) const
{
  return m_double;
}

//conversion to 2d vectors
FlexType::operator vec2(void) const
{
  return vec2(m_vec4.x, m_vec4.y);
}

//conversion to 3d vectors
FlexType::operator vec3(void) const
{
  return vec3(m_vec4.x, m_vec4.y, m_vec4.z);
}

//conversion to 4d vectors
FlexType::operator vec4(void) const
{
  return m_vec4;
}

//conversion to boolean type
FlexType::operator u1(void) const
{
  return !m_string.empty() && m_string != "0";
}

//conversion to hstring type
FlexType::operator hstring (void) const
{
  return H(m_string.c_str());
}

FlexType& FlexType::operator=( FlexType const& rhs )
{
  m_knownVectorLength = rhs.m_knownVectorLength;
  m_string = rhs.m_string;
  m_int = rhs.m_int;
  m_unsigned = rhs.m_unsigned;
  m_double = rhs.m_double;
  m_vec4 = rhs.m_vec4;
  return *this;
}

FlexType& FlexType::operator=( std::string const& rhs )
{
  m_string = rhs;
  flex();
  return *this;
}

FlexType& FlexType::operator=( s8 const* rhs )
{
  m_string = rhs;
  flex();
  return *this;
}

FlexType& FlexType::operator=( s32 rhs )
{
  std::stringstream source;
  source << rhs;
  m_string = source.str();
  flex();
  return *this;
}

FlexType& FlexType::operator=( u32 rhs )
{
  std::stringstream source;
  source << rhs;
  m_string = source.str();
  flex();
  return *this;
}

FlexType& FlexType::operator=( f32 rhs )
{
  std::stringstream source;
  source << rhs;
  m_string = source.str();
  flex();
  return *this;
}

FlexType& FlexType::operator=( f64 rhs )
{
  std::stringstream source;
  source << rhs;
  m_string = source.str();
  flex();
  return *this;
}

FlexType& FlexType::operator=( vec2 const& rhs )
{
  std::stringstream source;
  source << rhs.x << "," << rhs.y;
  m_string = source.str();
  flex();
  return *this;
}

FlexType& FlexType::operator=( vec3 const& rhs )
{
  std::stringstream source;
  source << rhs.x << "," << rhs.y << "," << rhs.z;
  m_string = source.str();
  flex();
  return *this;
}

FlexType& FlexType::operator=( vec4 const& rhs )
{
  std::stringstream source;
  source << rhs.x << "," << rhs.y << "," << rhs.z << "," << rhs.w;
  m_string = source.str();
  flex();
  return *this;
}

FlexType& FlexType::operator+=( FlexType const& rhs )
{
  m_int += rhs.m_int;
  m_unsigned += rhs.m_unsigned;
  m_double += rhs.m_double;
  m_vec4 += rhs.m_vec4;
  setString();
  return *this;
}

FlexType& FlexType::operator-=( FlexType const& rhs )
{
  m_int -= rhs.m_int;
  m_unsigned -= rhs.m_unsigned;
  m_double -= rhs.m_double;
  m_vec4 -= rhs.m_vec4;
  setString();
  return *this;
}

FlexType& FlexType::operator*=( FlexType const& rhs )
{
  m_int *= rhs.m_int;
  m_unsigned *= rhs.m_unsigned;
  m_double *= rhs.m_double;
  m_vec4 *= rhs.m_vec4;
  setString();
  return *this;
}

FlexType& FlexType::operator/=( FlexType const& rhs )
{
  m_int /= rhs.m_int;
  m_unsigned /= rhs.m_unsigned;
  m_double /= rhs.m_double;
  m_vec4 /= rhs.m_vec4;
  setString();
  return *this;
}

FlexType& FlexType::operator%=( u32 rhs )
{
  m_int %= rhs;
  m_unsigned %= rhs;

  //NON-IDEAL conversions, made to ensure value consistency conversions
  m_double = SCAST(f64, m_int);
  m_vec4.x = m_vec4.y = m_vec4.z = m_vec4.w = SCAST(f32, m_int);
  m_knownVectorLength = 0;//keeping a vector after this operation is silly
  setString();
  return *this;
}

FlexType FlexType::operator+( FlexType const& rhs ) const
{
  return FlexType(*this)+=rhs;
}

FlexType FlexType::operator-( FlexType const& rhs ) const
{
  return FlexType(*this)-=rhs;
}

FlexType FlexType::operator*( FlexType const& rhs ) const
{
  return FlexType(*this)*=rhs;
}

FlexType FlexType::operator/( FlexType const& rhs ) const
{
  return FlexType(*this)/=rhs;
}

FlexType FlexType::operator%( u32 rhs ) const
{
  return FlexType(*this)%=rhs;
}

u1 FlexType::operator<( FlexType const& rhs ) const
{
  //if either is not strictly a number, compare string sizes
  //if either string contains a decimal point, compare with double
  //if either string begins with a minus sign, compare with int
  //otherwise compare with unsigned

  if(!isNumeric(true) || !rhs.isNumeric(true)) 
    return m_string.size() < rhs.m_string.size();
  else if(m_string.find_first_of('.') != std::string::npos || rhs.m_string.find_first_of('.') != std::string::npos)
    return m_double < rhs.m_double;
  else if(m_string.substr(0,1) == "-" || rhs.m_string.substr(0,1) == "-")
    return m_int < rhs.m_int;
  else
    return m_unsigned < rhs.m_unsigned;
}

u1 FlexType::operator>( FlexType const& rhs ) const
{
  return rhs<*this;
}

u1 FlexType::operator==( FlexType const& rhs ) const
{
  return m_string == rhs.m_string;
}

u1 FlexType::operator!=( FlexType const& rhs ) const
{
  return !(*this==rhs);
}

u1 FlexType::operator<=( FlexType const& rhs ) const
{
  return *this==rhs || *this<rhs;
}

u1 FlexType::operator>=( FlexType const& rhs ) const
{
  return *this==rhs || *this>rhs;
}

u1 FlexType::isNumeric( u1 strict ) const
{
  if(strict)
  {
    //if string contains any character that is not either a digit, minus sign, decimal point, or comma (when also is a vector)
    for(size_t i = 0; i < m_string.size(); ++i)
      if(!std::isdigit(m_string[i]) && m_string[i] != '-' && (m_string[i] == ',' && !isVector()) && m_string[i] != '.')
        return false;
  }
  if(m_double || m_string == "0")
      return true;
  return false;
}
u1 FlexType::isVector( u32 size ) const
{
  if(size)
    return m_knownVectorLength==size;
  else
    return m_knownVectorLength!=0;
}

u32 FlexType::vectorSize( void ) const
{
  return m_knownVectorLength;
}

void FlexType::flex( void )
{

  m_int = atoi(m_string.c_str());
  m_unsigned = strtoul(m_string.c_str(),NULL,0);
  m_double = atof(m_string.c_str());

  //Vector types
  size_t xComma = m_string.find_first_of(',');
  m_vec4.x = SCAST(f32, m_double); 
  if(xComma == std::string::npos)
  {
    m_knownVectorLength = 0;
    m_vec4.y = m_vec4.z = m_vec4.w = m_vec4.x;
  }
  else
  {

    size_t yComma = m_string.find_first_of(',',xComma+1);
    std::string Y = m_string.substr(xComma+1,yComma);
    m_vec4.y = SCAST(f32, atof(Y.c_str()));

    if(yComma == std::string::npos)
    {
      m_knownVectorLength = 2;
      m_vec4.z = m_vec4.w= 0;
    }
    else
    {
      size_t zComma = m_string.find_first_of(',',yComma+1);
      std::string Z = m_string.substr(yComma+1,zComma);   
      m_vec4.z = SCAST(f32, atof(Z.c_str()));

      if(zComma == std::string::npos)
      {
        m_knownVectorLength = 3;
        m_vec4.w = 0;
      }
      else
      {
        m_knownVectorLength = 4;
        std::string W = m_string.substr(zComma+1);
        m_vec4.w = SCAST(f32, atof(W.c_str()));
      }
    }
  }
}

void FlexType::setString( void )
{
  std::stringstream source;

  if(m_knownVectorLength)
  {
    source << m_vec4.x << "," << m_vec4.y;
    if(m_knownVectorLength > 2)
    {
      source << "," << m_vec4.z;
      if(m_knownVectorLength > 3)
        source << "," << m_vec4.w;
    }
  }
  else
    source << m_double;
  m_string = source.str();
}




///////////////////////
// TEST DRIVER STUFF //
///////////////////////
// void test(void)
// {
// 
// 
////String test
//   flexType fs1(std::string("-1"));
//   flexType fs2(std::string("-1.1Fortune Bound! HA#'"));
//   flexType fs3(std::string("-234.75,234.25,-124"));
//   flexType fs4(std::string("-190.23,-12743.2,-150.75,437.12"));



////Type test
//   std::string s1 = "-+-+";
//   std::string s2 = "-12331String with numbers";
//   int i = -124;
//   unsigned u = 215124;
//   unsigned long ul = 99999999999999;
//   float f = 1.25f;
//   double d = 2.303028347234;
//   Vector2 v2(2.1, 2.5);
//   Vector3 v3(3.234,3.523,3.212);
//   Vector4 v4(-4.2512,4.234,4.523,4.212);
// 
//   flexType f1(i), f2(u), f3(ul), f4(f), f5(d), f6(v2), f7(v3), f8(v4), f9(s1), f0(s2);
//   std::cout << f1 << f2 << f3 << f4 << f5 << f6 << f7 << f8 << f9 << f0;

////Modulo test
//   flexType f1(10);
//   flexType f2(204);
// 
//   flexType f3(f2%f1);
//   
//   flexType fs3;
//   fs3 += fs1;
//   fs3 += fs2;


//// += test
//   flexType f1(2);
//   flexType f2(50.2342);
//   flexType f3("try it");
//   flexType f4(Vector2(2,2));
//   flexType f5(Vector3(3,3,3));
//   flexType f6(Vector4(4,4,4,4));
//   f2+=f1;
//   f3+=f1;
//   f4+=f1;
//   f5+=f1;
//   f6+=f1;


//   string s1 = "-+-+";
//   string s2 = "-12331String with numbers";
//   s32 i = -124;
//   u32 u = 215124;
//   u32 ul = 99999999999999;
//   f32 f = 1.25f;
//   f64 d = 2.303028347234;
//   vec2 v2(2.1, 2.5);
//   vec3 v3(3.234,3.523,3.212);
//   vec4 v4(-4.2512,4.234,4.523,4.212);
// 
//   flexType f1(i);
//   std::cout << f1;
// 
//   f1 = u;
//   std::cout << f1;
// 
//   f1 = ul;
//   std::cout << f1;
// 
//   f1 = f;
//   std::cout << f1;
// 
//   f1 = d;
//   std::cout << f1;
// 
//   f1 = v2;
//   std::cout << f1;
// 
//   f1 = "1.5,1.8";
//   std::cout << f1;
// 
//   f1 = "1.5,,,,,";
//   std::cout << f1;
// }

} //namespace
