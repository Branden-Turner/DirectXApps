/*! ====================================================================================================================
  @file component_transform.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jul.1.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#include "component.h"
namespace Grimelins
{

    SCRIPT_HeaderDefine(Transform);

    class Transform : public Component
    {
      friend class Object;
    public:
      Transform(void);
      virtual ~Transform(void);

    public://Virtual interface
      virtual void addRegistry( void );
      virtual void remRegistry( void );
      virtual Text getTypeName(void) const;
      virtual TypeID getTypeID(void) const;
      virtual void serialize( Scope& data, bool read );
      virtual void serialize( Component const* source);
      virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp);

      PROPERTY(vec3, scale);
      PROPERTY(Quat, rotation);
      PROPERTY(vec3, position);

      //SCRIPT special functions
      ScriptVec3 getPosition(void);
      ScriptVec3 getScale(void);
      void setPosition(ScriptVec3& pos);
      void setScale(ScriptVec3& scale);

    private:
      friend class Factory;
      friend class Physics;
      friend class Graphics;
      static Component* create(void);

    private:
      vec3 m_scale;
      Quat m_rotation;
      vec3 m_position;
    };
};//namespace