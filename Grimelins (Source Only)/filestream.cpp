/*! ====================================================================================================================
  @file filestream.cpp

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jul.2.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "filestream.h"

namespace Grimelins
{


  FileStream::FileStream( std::string const& file )
  {
    if(file!="")
      newStream(file);
  }

  FileStream::~FileStream( void )
  {
    if(m_file.is_open())
      m_file.close();
  }

  void FileStream::newStream( std::string const& file )
  {
    if(m_file.is_open())
      m_file.close();

    m_file.open(file.c_str());
    ErrorIf(!m_file.is_open(), "FileStream::newStream file not found \"%s\".", file.c_str());
    ErrorIf(!m_file.good(), "FileStream::newStream failed! There was no data in the file stream.");
  }

  void FileStream::cleanStream( void )
  {
    ch c;
    if(!m_file.good())
      return;

    do 
    {
      c = m_file.get();

    } while ((c == ' '  || c == '\t' || c == '\n') && c != EOF);
    m_file.unget();
  }

  void FileStream::streamSeekTo( ch const& c )
  {
    ch cur;
    do 
    {
      ErrorIf(!m_file.good(), "FileStream::dumpCharacter failed! There was no data in the file stream.");
      cur = m_file.get();
      ErrorIf(cur=='\n', "FileStream::dumpCharacter failed! A character identifier was expected, but an end of line was encountered.");
    } while (cur != c);

  }

  void FileStream::stringTrim( std::string s )
  {
    size_t lastChr = 1 + s.find_last_not_of(" \t\n");
    if(lastChr != s.size() && lastChr != 0)
      s = s.substr(0, lastChr);
  }

  bool FileStream::inScope( ch const& openScope, ch const& closeScope)
  {
    cleanStream();
    ch next = m_file.get();
    if(next == openScope)
      next = m_file.get();
    if(next != closeScope && next != EOF)
    {
      m_file.putback(next);
      return true;
    }
    return false;
  }

  std::string FileStream::readSubstring( ch const& delimiter)
  {
    cleanStream();
    std::string out;
    ch c;
    do 
    { 
      ErrorIf(!m_file.good(), "FileStream::segementRead failed. Unexpected end of file.");
      c = m_file.get();
      if(c=='\n' || c==delimiter) break;
      out += c;
    } while (m_file.good());
    stringTrim(out);
    return out;  
  }

  bool FileStream::hasData( void )
  {
    if(!m_file.good()) return false;
    cleanStream();
    return m_file.good();
  }

  bool FileStream::peekLineFor( ch const& delim )
  {
    std::string out;
    ch c;
    do 
    { 
      ErrorIf(!m_file.good(), "FileStream::peekLineFor failed. Unexpected end of file.");
      out += c = m_file.get();
      if(c=='\n') 
      {
        while(out.size())
        {
          m_file.putback(out[out.size()-1]);
          out = out.substr(0, out.size()-1);
        }
        return false;
      }
      if(c==delim)
      {
        while(out.size())
        {
          m_file.putback(out[out.size()-1]);
          out = out.substr(0, out.size()-1);
        }
        return true;
      }
    } while (m_file.good());
    return false;  
  }
};//namespace