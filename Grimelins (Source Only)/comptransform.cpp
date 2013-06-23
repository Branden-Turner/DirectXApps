/*! ====================================================================================================================
  @file component_transform.cpp

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jul.1.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "comptransform.h"

namespace Grimelins
{
  /////// SCRIPT BINDS
  //// Transform
  SCRIPT_ClassStart(Transform);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(getTypeName);
  SCRIPT_Method(getTypeID);
  //SCRIPT specialized functions
  SCRIPT_Method(getPosition);
  SCRIPT_Method(getScale);
  SCRIPT_Method(setPosition);
  SCRIPT_Method(setScale);
  SCRIPT_ClassEnd();

    Component* Transform::create(void)
    {
      return SCAST(Component*, new Transform());
    }

    Transform::Transform( void )
    {
    }

    Transform::~Transform( void )
    {
      remRegistry();
    }

    Text Transform::getTypeName( void ) const
    {
      return "Transform";
    }

    void Transform::addRegistry( void )
    {
      /*no system cares... SYSTEM->vRegisterComponent(getTypeID(), this);*/
    }

    void Transform::remRegistry( void )
    {
      /*no system cares... SYSTEM->vRegisterComponent(getTypeID(), this);*/
    }

    TypeID Transform::getTypeID( void ) const
    {
      return TYPE_ID(Transform);
    }

    void Transform::serialize( Scope& data, bool read )
    {
      SERIAL_BeginSerializing(Transform);

      SERIAL_Var(vec3, "position", m_position);
      SERIAL_Var(vec3, "scale", m_scale);
      SERIAL_Var(Quat, "rotation", m_rotation);
    }

    void Transform::serialize( Component const* source )
    {
      Transform const& t = *SCAST(Transform const*, source);
      m_position = t.m_position;
      m_scale = t.m_scale;
      m_rotation = t.m_rotation;
    }

    u1 Transform::handleEvent( hstring const& message, RawStore const& args, MessageType type, f32 timestamp )
    {
      MSG_StartHandling();
      MSG_DoneHandling();
    }

    GET(Transform, scale)
    {
      return m_scale;
    }

    SET(Transform, scale)
    {
      m_scale = value;
      ErrorIf(!scale.x && !scale.y && !scale.z, "Transform::Scale was assigned an illigal value (0,0,0).");
    }

    GET(Transform, rotation)
    {
      return m_rotation;
    }

    SET(Transform, rotation)
    {
      m_rotation = value;
    } 

    GET(Transform, position)
    {
      return m_position;
    }

    SET(Transform, position)
    {
      m_position = value;
    }

    Grimelins::ScriptVec3 Transform::getPosition( void )
    {
      return ScriptVec3(m_position);
    }

    Grimelins::ScriptVec3 Transform::getScale( void )
    {
      return ScriptVec3(m_scale);
    }

    void Transform::setPosition( ScriptVec3& pos )
    {
      m_position = vec3(pos);
    }

    void Transform::setScale( ScriptVec3& scale )
    {
      m_scale = vec3(scale);
    }
}//namespace
