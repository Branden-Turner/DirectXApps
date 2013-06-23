/*! ====================================================================================================================
  @file compspeaker.cpp

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.17.2012

  @brief 
    Speaker component

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "syssound.h"
#include "compspeaker.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Code Segment

namespace Grimelins {


SCRIPT_ClassStart(Speaker);
SCRIPT_CTOR_Disabled();
SCRIPT_Method(getTypeName);
SCRIPT_Method(getTypeID);
SCRIPT_Method(getDirection);
SCRIPT_Method(setDirection);
//SCRIPT specialized functions
//none
SCRIPT_ClassEnd();


Component* Speaker::create(void)
{
  return SCAST(Component*, new Speaker());
}


//////////////////////////////////////////////////////////////////////////
// Public Autos

Speaker::Speaker(void)
{

}

Speaker::~Speaker(void)
{
  remRegistry();
}

//////////////////////////////////////////////////////////////////////////
// Virtual Interface

void Speaker::addRegistry(void)
{
  SOUND->addSpeaker(this);
}

void Speaker::remRegistry(void)
{
  SOUND->removeSpeaker(this);
}

Text Speaker::getTypeName(void) const
{
  return "Speaker";
}

TypeID Speaker::getTypeID(void) const
{
  return TYPE_ID(Speaker);
}

void Speaker::serialize( Scope& data, bool read )
{
  SERIAL_BeginSerializing(Speaker);
  SERIAL_Var(vec3, "direction", mDirection);
}

void Speaker::serialize( Component const* source)
{
  Speaker const* s = checked_cast<Speaker const*>(source);
  mDirection = s->mDirection;
}

u1 Speaker::handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp)
{
  return false;
}



} // namespace Listener