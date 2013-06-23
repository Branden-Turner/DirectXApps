/*! ====================================================================================================================
  @file flextype.h

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

    // Use example:
    int i = 5;
    flexType f(i);                        // Construct a flexType from an integer
    f %= 2;                               // perform modulus operator on the data
    f *= 4.753;                           // multiply the flexType by a double
    f = Vector4(2,3,4,5) * f;             // assign a vector4 to the flexType after scaling it by the former value.
    f = <Vector4>(f).magnitude();         // assign the magnitude of the resultant vector back to the flexType
    ostream os << static_cast<double>(f); // insert the double value of the flexType into an ostream
    fstream fs << static_cast<string>(f); // insert the string value of the flexType into a fstream
    sstream ss << f;                      // insert the full value list of the flexType into a string stream



                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_FLEXTYPE_H
#define VIRTUS_FLEXTYPE_H

//Comment out to make the FlexType output to streams as a string
// when VIRTUS_FLEXTYPE_DEBUG_STREAM is defined all values will be
// put into the stream.

#define VIRTUS_FLEXTYPE_DEBUG_STREAM

namespace Grimelins
{
class FlexType
{

private://DATA MEMBERS
  std::string m_string;
  s32 m_int;
  u32 m_unsigned;
  f64 m_double;
  vec4 m_vec4;
  u32 m_knownVectorLength;//the vector size that can be reasonably guaranteed. Non-vectors are 0

public: //CONSTRUCTORS
  FlexType(std::string s = "");
  FlexType(s32 i);
  FlexType(u32 u);
  FlexType(f64 d);
  FlexType(vec2 const& v);
  FlexType(vec3 const& v);
  FlexType(vec4 const& v);
  FlexType(FlexType const& f);

  virtual ~FlexType(void);

public://CONVERSION OPERATORS
  operator ch const*(void) const;
  operator std::string(void) const;
  operator s32(void) const;
  operator u32(void) const;
  operator f32(void) const;
  operator f64(void) const;
  operator vec2(void) const;
  operator vec3(void) const;
  operator vec4(void) const;
  operator u1(void) const;
  operator hstring(void) const;

public: //ASSIGNMENT OPERATORS
  FlexType& operator=(FlexType const& rhs);
  FlexType& operator=(std::string const& rhs);
  FlexType& operator=(ch const* rhs);
  FlexType& operator=(s32 rhs);
  FlexType& operator=(u32 rhs);
  FlexType& operator=(f32 rhs);
  FlexType& operator=(f64 rhs);
  FlexType& operator=(vec2 const& rhs);
  FlexType& operator=(vec3 const& rhs);
  FlexType& operator=(vec4 const& rhs);

public: //MATHAMATICAL OPERATORS
  FlexType& operator+=(FlexType const& rhs);
  FlexType& operator-=(FlexType const& rhs);
  FlexType& operator*=(FlexType const& rhs);
  FlexType& operator/=(FlexType const& rhs);
  FlexType& operator%=(u32 rhs);//no other type makes sense except u32

  FlexType operator+(FlexType const& rhs) const;
  FlexType operator-(FlexType const& rhs) const;
  FlexType operator*(FlexType const& rhs) const;
  FlexType operator/(FlexType const& rhs) const;
  FlexType operator%(u32 rhs) const;//no other type makes sense except u32

public: //COMPARISON OPERATORS
  u1 operator<(FlexType const& rhs) const;
  u1 operator>(FlexType const& rhs) const;
  u1 operator==(FlexType const& rhs) const;
  u1 operator!=(FlexType const& rhs) const;
  u1 operator<=(FlexType const& rhs) const;
  u1 operator>=(FlexType const& rhs) const;

public: //OTHER METHODS
  /// Test for valid numerical data in the FlexType. If strict is enabled, also requires that the string not contain non-numerical data.
  u1 isNumeric(u1 strict = false) const;
  /// Returns whether or not the basis data is believed to be of type vector. Size constrains this test to vectors of a given length (2, 3, or 4).
  u1 isVector(u32 size = 0) const;
  /// Returns the vector size that can be logically guaranteed.
  u32 vectorSize(void) const;

  friend std::ostream& operator<<(std::ostream & os, FlexType const& f);
  friend std::istream& operator>>(std::istream & is, FlexType& f);

private://HELPERS
  void flex(void);
  void setString(void);

};

}//namespace

#endif
