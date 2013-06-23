/**********************************************************************/
/*
  
  \file
    cmpproj.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This will be a projectile moving through the scene.
*/
/**********************************************************************/
#ifndef GRIMELIN_CMP_PROJECTILE_H
#define GRIMELIN_CMP_PROJECTILE_H

#include "component.h"

namespace Grimelins
{

  enum ProjectileState
  {
    notShot,
    caughtByPlayer,
    shotByBoss,
    shotByPlayer,
    stateNum
  };

  class ProjectileComponent : public Component
  {
    public:
      void Update( f32 dt );
      void Shoot(vec3& direction, f32 speed, f32 pitchSpeed_ = 0.0f);
      void Catch(Body* playerBody);
      ProjectileState getState() const {return state;};
      ProjectileComponent();
      virtual ~ProjectileComponent();

    //Component interface
    public:
      virtual void addRegistry( void );
      virtual void remRegistry( void );
      virtual TypeID getTypeID(void) const;
      virtual Text getTypeName(void) const;
      virtual void serialize( Scope& data, bool read );
      virtual void serialize( Component const* source);
      virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp) {return false;};

    //Factory creation interface
    friend class Factory;
    friend class Object;
    private:
      static Component* create(void);
      vec3 dirVector;
      f32 speed;
      u1 shot;
      ProjectileState state;
      Body* player;
      f32 pitchSpeed;
  };
}

#endif