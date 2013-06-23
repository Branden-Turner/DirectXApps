/*! ====================================================================================================================
  @file filestream.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jul.2.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once

namespace Grimelins
{
// #define streamRead(type) read<type>(void)

  class FileStream
  {
  public:
    FileStream(std::string const& file="");
    ~FileStream(void);

    void newStream(std::string const& file);
    void cleanStream(void);
    void streamSeekTo(ch const& c);
    void stringTrim(std::string s);
    bool inScope( ch const& openScope ='{', ch const& closeScope='}');
    std::string readSubstring(ch const& delimiter=':');
    bool hasData(void);

    bool peekLineFor(ch const& delim);

    template <typename T>
    T read(void);

    // ** string
    template <>
    std::string read<std::string>(void);

    // ** Vector2
    template <>
    vec2 read<vec2>(void);

    // ** Vector3
    template <>
    vec3 read<vec3>(void);

    // ** Vector4
    template <>
    vec4 read<vec4>(void);

    // ** Quaternion
    template <>
    Quat read<Quat>(void);

  private:
    std::ifstream m_file;
  };

};//namespace
#include "filestream.inl"
