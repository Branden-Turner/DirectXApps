/*! ====================================================================================================================
  @file scriptGlobal.cpp

  Grimelins 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jan.31.2012

  @brief This is a class for global values that should be available to all scripts.
  For example, the global pointer to the player, or the boss.

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "scriptGlobal.h"
#include "factory.h"

namespace Grimelins
{
  SCRIPT_ClassStart(ScriptGlobal);
  SCRIPT_CTOR_Disabled();
  SCRIPT_MethodOverload(ERR_IF, "ErrorIf", void, (bool, Text const&));
  SCRIPT_Method(getPlayer1);
  SCRIPT_Method(setPlayer1);
  SCRIPT_Method(clearPlayer1);
  SCRIPT_Method(getBoss);
  SCRIPT_Method(setBoss);
  SCRIPT_Method(clearBoss);
  SCRIPT_Method(getGlobal);
  SCRIPT_Method(setGlobal);
  SCRIPT_Method(clearGlobal);
  SCRIPT_Method(isGamePaused);
  SCRIPT_Method(acos);
  SCRIPT_Method(cos);
  SCRIPT_Method(sin);
  SCRIPT_Method(pi);
  SCRIPT_Method(randf);
  SCRIPT_Method(randi);
  SCRIPT_Method(randfr);
  SCRIPT_Method(randir);
  SCRIPT_ClassEnd();
  ScriptGlobal* SCRIPTGLOBALS = NULL;

  ScriptGlobal::ScriptGlobal( void )
  : m_boss(-1), m_player1(-1), m_global(-1)
  {
    SCRIPTGLOBALS = this;
    SCRIPT_RegisterType(ScriptGlobal, Grimelins);
    SCRIPT_GlobalInstanceOfBoundType(this, "g");
  }

  ScriptGlobal::~ScriptGlobal( void )
  {
    ErrorIf(m_player1!=-1, "ScriptGlobal shut down with player pointer still active - if it was freed it should update this to prevent invalid pointers being accessed from script.");
    ErrorIf(m_boss!=-1,   "ScriptGlobal shut down with boss pointer still active - if it was freed it should update this to prevent invalid pointers being accessed from script.");
  }

  void ScriptGlobal::ERR_IF( bool expr, Text const& message )
  {
    ErrorIf(expr, "A script assert was encountered.\n Error message:\n %s", message.c_str());
  }

  Object* ScriptGlobal::getPlayer1( void )
  {
//     ErrorIf(gFactory->getObjectByID(m_player1)==NULL, "Script called getPlayer1, which did not exist!");
    return gFactory->getObjectByID(m_player1);
  }

  void ScriptGlobal::setPlayer1( Object& obj )
  {
    m_player1 = obj.getID();
  }

  void ScriptGlobal::clearPlayer1( void )
  {
    m_player1 = -1;
  }

  Object* ScriptGlobal::getBoss( void )
  {
    //     ErrorIf(gFactory->getObjectByID(m_boss)==NULL, "Script called getBoss, which did not exist!");
    return gFactory->getObjectByID(m_boss);
  }

  void ScriptGlobal::setBoss( Object& obj )
  {
    m_boss = obj.getID();
  }

  void ScriptGlobal::clearBoss( void )
  {
    m_boss = -1;
  }

  Object* ScriptGlobal::getGlobal( void )
  {
    return gFactory->getObjectByID(m_global);
  }

  void ScriptGlobal::setGlobal( Object& g )
  {
    m_global = g.getID();
  }
  void ScriptGlobal::clearGlobal(void)
  {
    m_global = -1;
  }

  float ScriptGlobal::acos( float val ) const
  {
    return acosf(val);
  }

  float ScriptGlobal::cos( float val ) const
  {
    return cosf(val);
  }

  float ScriptGlobal::sin( float val ) const
  {
    return sinf(val);
  }

  float ScriptGlobal::randf() const
  {
    return SCAST(float, rand());
  }

  int ScriptGlobal::randi() const
  {
    return rand();
  }

  float ScriptGlobal::randfr(float a, float b) const
  {
    return (rand() / (static_cast<float>(RAND_MAX) + 1.0f)) * (b - a) + a;
  }

  int ScriptGlobal::randir(int a, int b) const
  {
    return a + rand() / ( RAND_MAX / ( b - a ) + 1 );
  }

  float ScriptGlobal::pi( void ) const
  {
    return PI;
  }

  bool ScriptGlobal::isGamePaused( void )
  {
    return gCore->GamePaused;
  }


}//namespace Grimelins
