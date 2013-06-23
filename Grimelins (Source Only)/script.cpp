/*! ====================================================================================================================
  @file script.cpp

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.23.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "script.h"

namespace Grimelins
{



  Script::Script( void )
    :m_script(NULL), m_source(""), m_owner(default_owner)
  {

  }

  Script::Script( Script const& script )
    : m_script(new Sqrat::Script(gCore->Scripter)), m_source(script.m_source), m_owner(script.m_owner)
  {
    m_script->CompileFile(m_source);
  }

  Script::Script( std::string const& file, Text const& owner )
    : m_script(new Sqrat::Script(gCore->Scripter)), m_source(CONFIG_PATH_SCRIPT + file + ".nut"), m_owner(owner)
  {
    m_script->CompileFile(m_source);
  }

  Grimelins::Script& Script::operator=( Script const& rhs )
  {
    newSource(rhs.m_source);
    return *this;
  }

  Grimelins::Script& Script::operator=( std::string const& rhs )
  {
    newSource(CONFIG_PATH_SCRIPT + rhs + ".nut");
    return *this;
  }

  Script::~Script( void )
  {
    delete m_script;
  }

  /*! --------------------------------------------------------------------------------------------------------------------
    @brief      Causes the Script object to re-compile the source. This is useful if the source has changed at runtime.
       
    @date 23.10.2011
  
  --------------------------------------------------------------------------------------------------------------------- */
  void Script::recompile( void )
  {
      compile();
  }

  void Script::run( void )
  {
    ErrorIf(!m_script, "Script cannot be run.\n\nScript was default constructed. Empty script has nothing to run.\n");
    ErrorIf(!m_script->IsCompiled(), "Script cannot be run.\n\nScript was not successfully compiled, try using recompile() first.\n");
    try
    {
      gCore->Scripter.setContext(m_owner, m_source);
      m_script->Run();
    }
    catch (Sqrat::Exception e)
    {
      printDebug("Error in squirrel virtual machine running script '%s' for Script object.\n", m_source.c_str());
    }
  }

  void Script::newSource( std::string const& source )
  {
    delete m_script;
    m_script = new Sqrat::Script(gCore->Scripter);
    m_source = source;
    compile();
    ErrorIf(!m_script->IsCompiled(), "Script Failed to compile '%s'\n\nCheck the filepath and filename.\n If you are sure it's right, check Squirrel syntax in the file.\n", m_source.c_str());     
  }

  void Script::compile( void )
  {
    ErrorIf(!m_script, "Script cannot be complied.\n\nScript was default constructed. Empty script has nothing to compile.\n");
    try
    {
      m_script->CompileFile(m_source);
    }
    catch (Sqrat::Exception e)
    {
      printDebug("Error compiling file '%s' in squirrel virtual machine for Script object.\n", m_source.c_str());
      return;
    }
    ErrorIf(!m_script->IsCompiled(), "Script Failed to compile '%s'\n\nCheck the filepath and filename.\n If you are sure it's right, check Squirrel syntax in the file.\n", m_source.c_str());    
  }

  const Text Script::default_owner = "Someone Constructed this script without identifing themselves!";

};//Virtus namespace
