/*! ====================================================================================================================
  @file hstring.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.18.2011

  @brief 
    Compile time hash optimizations

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "hstring.h"

namespace Grimelins
{
#if 1
// #if VIRTUS_CONFIG_DEBUG_MODE || VIRTUS_CONFIG_PROFILE_MODE

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Debug Version

  hstring::hstring(void)
  {

  }

  hstring::hstring(const char* str) : m_String(str)
  {

  }

  hstring::hstring(std::string const& str) : m_String(str)
  {

  }

  hstring::hstring(hstring const& rhs) : m_String(rhs.m_String)
  {

  }

  hstring::~hstring(void)
  {

  }

  hstring& hstring::operator=(const char* str)
  {
    m_String = str;
    return *this;
  }

  hstring& hstring::operator=(hstring const& rhs)
  {
    m_String = rhs.m_String;
    return *this;
  }

  bool hstring::operator==(hstring const& rhs) const
  {
    return m_String == rhs.m_String;
  }

  bool hstring::operator!=(hstring const& rhs) const
  {
    return m_String != rhs.m_String;
  }

  bool hstring::operator<(hstring const& rhs) const
  {
    return m_String < rhs.m_String;
  }

  bool hstring::operator>(hstring const& rhs) const
  {
    return m_String > rhs.m_String;
  }

  bool hstring::empty(void) const
  {
    return m_String.empty();
  }

  ch const* const hstring::str(void) const
  {
    return m_String.c_str();
  }

#else

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Release Version

  hstring::hstring(void) : m_Hash(-1)
  {

  }

  hstring::hstring(s32 str) : m_Hash(str)
  {

  }

  hstring::hstring(hstring const& rhs) : m_Hash(rhs.m_Hash)
  {

  }

  hstring::hstring(ch const* str) : m_Hash(CONSTHASH(str))
  {
      
  }

  hstring::hstring(string const& str) : m_Hash(CONSTHASH(str.c_str()))
  {

  }

  hstring::~hstring(void)
  {

  }

  hstring& hstring::operator=(s32 str)
  {
    m_Hash = str;
    return *this;
  }

  hstring& hstring::operator=(hstring const& rhs)
  {
    m_Hash = rhs.m_Hash;
    return *this;
  }

  bool hstring::operator==(hstring const& rhs) const
  {
    return m_Hash == rhs.m_Hash;
  }

  bool hstring::operator!=(hstring const& rhs) const
  {
    return m_Hash != rhs.m_Hash;
  }

  bool hstring::operator<(hstring const& rhs) const
  {
    return m_Hash < rhs.m_Hash;
  }

  bool hstring::operator>(hstring const& rhs) const
  {
    return m_Hash > rhs.m_Hash;
  }

  bool hstring::empty(void) const
  {
    return m_Hash == -1;
  }

  ch const* const hstring::str(void) const
  {
    // Convert hash to hex string
    _itoa(m_Hash, m_CStr, 16);
    return m_CStr;
  }


#endif
} // namespace virtus