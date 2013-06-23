/*! ====================================================================================================================
  @file filestream.inl

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jul.2.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

namespace Grimelins
{
  template <typename T>
  Text toText(T const& value)
  {
    std::stringstream ss;
    ss << value;
    return ss.str();
  }

  template <typename T>
  T FileStream::read( void )
  {
    T out;
    cleanStream();
    ErrorIf(!m_file.good(), "FileStream::<T>read failed. Unexpected end of file.");
    m_file >> out;
    return out;
  }

  template <>
  std::string FileStream::read<std::string>( void )
  {
    cleanStream();
    std::string out;
    ch c;
    do 
    { 
      ErrorIf(!m_file.good(), "FileStream::<string>read failed. Unexpected end of file.");
      c = m_file.get();
      if(c=='\n' || c==EOF) break;
      out += c;
    } while (m_file.good());

    stringTrim(out);
    return out;  
  }

  template <>
  vec2 FileStream::read<vec2>( void )
  {
    vec2 out;
    cleanStream();
    ErrorIf(!m_file.good(), "FileStream::<vec2>read failed. Unexpected end of file.");
    out.x = read<float>();
    streamSeekTo(',');
    out.y = read<float>();
    return out;  
  }

  template <>
  vec3 FileStream::read<vec3>( void )
  {
    vec3 out;
    cleanStream();
    ErrorIf(!m_file.good(), "FileStream::<vec3>read failed. Unexpected end of file.");
    out.x = read<float>();
    streamSeekTo(',');
    out.y = read<float>();
    streamSeekTo(',');
    out.z = read<float>();
    return out;
  }

  template <>
  vec4 FileStream::read<vec4>( void )
  {
    vec4 out;
    cleanStream();
    ErrorIf(!m_file.good(), "FileStream::<vec4>read failed. Unexpected end of file.");
    out.x = read<float>();
    streamSeekTo(',');
    out.y = read<float>();
    streamSeekTo(',');
    out.z = read<float>();
    streamSeekTo(',');
    out.w = read<float>();
    return out;
  }

  template <>
  Quat FileStream::read<Quat>( void )
  {
    Quat out;
    cleanStream();
    ErrorIf(!m_file.good(), "FileStream::<Quat>read failed. Unexpected end of file.");
    out.w = read<float>();
    streamSeekTo(',');
    out.x = read<float>();
    streamSeekTo(',');
    out.y = read<float>();
    streamSeekTo(',');
    out.z = read<float>();
    return out;
  }

};//namespace