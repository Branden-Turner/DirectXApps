/*! ====================================================================================================================
  @file syspointers.h

  Codename Virtus

  @author Branden Turner <branden.turner@digipen.edu>
  @date Oct.14.2011

  @brief 
    System Pointers

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIME_SYSPOINTER_H
#define GRIME_SYSPOINTER_H

namespace Grimelins
{
  //Forward declarations for system pointers.
  class Input;
  class Physics;
  class Graphics;
  class Sound;
  class Factory;
  class Object;
  class UserInterface;
  class ScriptGlobal;

  extern Input*    INPUT;
  extern Physics*  PHYSICS;
  extern Graphics* GRAPHICS;
  extern Sound*    SOUND;
  extern Factory*  FACTORY;
  extern UserInterface*  USERINTERFACE;
  extern ScriptGlobal* SCRIPTGLOBALS;

}


#endif  GRIME_SYSPOINTER_H