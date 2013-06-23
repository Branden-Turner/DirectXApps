/*! ====================================================================================================================
  @file compspeaker.h

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.17.2012

  @brief 
    Speaker component (the element that emits a sound and is picked up by a Listener Component)

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#include "component.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Code Segment

namespace Grimelins {

  SCRIPT_HeaderDefine(Speaker);


  class Speaker : public Component
  {
  public:
    Speaker(void);
    virtual ~Speaker(void);

  public:
    virtual void addRegistry(void);
    virtual void remRegistry(void);
    virtual Text getTypeName(void) const;
    virtual TypeID getTypeID(void) const;
    virtual void serialize( Scope& data, bool read );
    virtual void serialize( Component const* source);
    virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp);

  private: // Interface
    friend class Factory;

    static Component* Speaker::create(void);

  public:
    Vector3 getDir(void) const { return mDirection; }
    void setDir(Vector3 const& vec) { mDirection = vec; }
    
    ScriptVec3 getDirection(void) const { return mDirection; }
    void setDirection(ScriptVec3 const& vec) { mDirection = Vector3(vec); }

  private:
    Vector3 mDirection; // Sound emmiting direction
  };

} // namespace Grimelins