/*! ====================================================================================================================
  @file scriptEnum.cpp

  Codename Grimelins 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Sep.30.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "scriptEnum.h"

namespace Grimelins
{


  ScriptENUM::ScriptENUM( Text const& script_container )
    : m_container(script_container)
  {
  }

  void ScriptENUM::newEnum( Text const& name, s32 val )
  {
    ErrorIf(m_byName.find(name)!=m_byName.end(), "Script ENUM by this name already exists (%s).\n", name.c_str());
    ErrorIf(m_byGUID.find(val)!=m_byGUID.end(), "Script ENUM by this value already exists (%d).\n", val);
    m_byName[name] = val;
    m_byGUID[val] = name;
    m_enum.Const(name.c_str(), val);
  }

  int ScriptENUM::getEnum( Text const& name )
  {
    if(m_byName.find(name)!=m_byName.end())
      return m_byName[name];
    return 0;
  }

  Text ScriptENUM::getEnum( s32 const& val )
  {
    if(m_byGUID.find(val)!=m_byGUID.end())
      return m_byGUID[val];
    return "";
  }

  void ScriptENUM::bind( void )
  {
    Sqrat::ConstTable(gCore->Scripter).Enum(m_container.c_str(), m_enum);
  }
};//Virtus namespace