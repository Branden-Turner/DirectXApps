/*! ====================================================================================================================
  @file script.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.21.2011

  @brief   Script class encapsulates generic Squirrel scripts. These are all such scripts that are not the specialized
           Behavior-type scripts. Script is a procedurally executed .nut file without instance context.

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_SCRIPT_H
#define VIRTUS_SCRIPT_H

namespace Grimelins
{
  class Script
  {
  public:
    static const Text default_owner;
    Script(void);
    Script(Script const& script);
    Script(std::string const& file, Text const& owner);
    Script& operator=(Script const& rhs);
    Script& operator=(std::string const& rhs);

    ~Script(void);

    void recompile(void);
    void run(void);

  private://helper
    void newSource(std::string const& source);
    void compile(void);
  
  private://data
    Sqrat::Script* m_script;
    Text m_source;
    Text m_owner;

  };
};//Virtus namespace
#endif //VIRTUS_SCRIPT_H