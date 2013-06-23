/*! ====================================================================================================================
  @file behavior.cpp

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.16.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "behavior.h"

// #define SCRIPT_DEBUG_FN

namespace Grimelins
{


    Behavior::Behavior( Text const& file )
      :m_script(CONFIG_PATH_SCRIPT + file + ".nut"), m_bytecode(NULL), m_id(file)
    {
      setup();
    }

    Behavior::~Behavior( void )
    {
      while(m_behaviors.size())
      {
        delete m_behaviors.begin()->second;
        m_behaviors.erase(m_behaviors.begin());
      }
      delete m_bytecode;
    }

    Grimelins::u1 Behavior::defines( BehaviorFnName const& fnName )
    {
      return m_behaviorsByName.find(fnName) != m_behaviorsByName.end();
    }

    BehaviorFn& Behavior::call( BehaviorFnName const& fnName )
    {
      // DEBUG HELP TOOL, John Dusseau
      #ifdef SCRIPT_DEBUG_FN

      if(id != "event_input_linear_debug" && id != "event_player1_cc_anim"
            && id != "event_input_level1" && fnName != "update" && fnName != "collide_env_p1")
      printDebug("%s.nut  on::%s\n", id.c_str(), fnName.c_str());
      #endif
      
      ErrorIf(!defines(fnName), "call() invoked on a behavior function that does not exist.\nBehavior: %s\nFunction: %s\n", m_script.c_str(), fnName.c_str());
      return *m_behaviorsByName[fnName];
    }

    BehaviorFn& Behavior::call( hstring const& msgName )
    {
      // DEBUG HELP TOOL, John Dusseau
      
      #ifdef SCRIPT_DEBUG_FN
      if(id != "event_input_linear_debug" && id != "event_player1_cc_anim"
            && id != "event_input_level1" && msgName != "update" && msgName != "collide_env_p1")
         printDebug("%s.nut on::%s\n", id.c_str(), msgName.str());
      #endif

      ErrorIf(m_behaviors.find(msgName)==m_behaviors.end(), "call() invoked on a behavior function that does not exist.\nBehavior: %s\nFunction: %s\n", m_script.c_str(), msgName.str());
      return *m_behaviors[msgName];
    }

    void Behavior::setup( void )
    {
      m_bytecode = new ByteCode(gCore->Scripter);
      ANT__Bar_Deinit(bar);
      ANT__Bar_Init(bar, ("Script: " + m_id).c_str(), false);
      ANT__Bar_AddButton(bar, "HIDE", ANT__Hide, ANT__PBar(bar));
      ANT__Bar_AddSeperatorAny(bar);

      try { m_bytecode->CompileFile(m_script); }
      catch (Sqrat::Exception e) { printDebug("Error compiling file '%s' in squirrel virtual machine for Script object.\n", m_script.c_str()); return;}

      ErrorIf(!m_bytecode->IsCompiled(), "Script Failed to compile '%s'\n\nCheck the filepath and filename.\n If you are sure it's right, check Squirrel syntax in the file.\n", m_script.c_str());    
      if(!m_bytecode->IsCompiled())
      {
        delete m_bytecode;
        m_bytecode = NULL;
        return;
      }

      try { m_bytecode->Run(); }
      catch (Sqrat::Exception e) { printDebug("Error in squirrel virtual machine running script '%s' for Script object.\n", m_script.c_str()); return;}

      Core::EventMap::iterator eIt = gCore->GameLogicEvents.begin();
      Core::EventMap::iterator endIt = gCore->GameLogicEvents.end();

      for(;eIt!=endIt;++eIt)
      {
        Sqrat::Function* fn = new BehaviorFn(Sqrat::RootTable(gCore->Scripter).GetSlot("on"), eIt->second.c_str());
        ErrorIf(!fn, "Script Object failed to allocate heap memory for a function");
        if(!fn->IsNull())
        {
          m_behaviors[eIt->first] = fn;
          m_behaviorsByName[eIt->second] = fn;
          ANT__Bar_AddButton_P(bar, eIt->second.c_str(), NULL, NULL, "group=Behaviors");
        }
        else
        {
          ANT__Bar_AddButton_P(bar, eIt->second.c_str(), NULL, NULL, "group=Unused");
          delete fn;
        }
      }
      ANT__Define(bar, "/Unused opened=false");
      ANT__Define(bar, "/Behaviors opened=true");
      gCore->Scripter.runString("on.clear();");
      ErrorIf(m_behaviors.empty(), "Behavior serialization yeilded NO scripted functions from file: %s\n", m_script.c_str());
    }

    Behavior::BehaviorFnMap const& Behavior::getDefinedFn( void ) const
    {
      return m_behaviors;
    }

    void Behavior::recompile( void )
    {
      while(m_behaviors.size())
      {
        delete m_behaviors.begin()->second;
        m_behaviors.erase(m_behaviors.begin());
      }
      delete m_bytecode;
      setup();
    }
};//Virtus namespace
