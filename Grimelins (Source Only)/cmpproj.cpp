/**********************************************************************/
/*
  
  \file
    cmpproj.cpp
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This will be a projectile that will move through the scene.
*/
/**********************************************************************/
#include "precompiled.h"
#include "compbody.h"
#include "cmpproj.h"
#include "cmpgfx.h"
#include "physics.h"
#include "cmpplayer.h"

namespace Grimelins
{

  const static f32 KillHeight = 600.0f;
  const static f32 KillWidth =  800.0f;

  Component* ProjectileComponent::create( void )
  {
    return SCAST(Component*, new ProjectileComponent());
  }

  ProjectileComponent::ProjectileComponent()
  {  
  }

  ProjectileComponent::~ProjectileComponent()
  {

    //Rewrite using new script global boss/player pointers  see scriptGlobal.h
    //////////////////////////////////////////////////////////////////////////

//     //If you're attached to a player, let them know.
//     if(player)
//     {
//       PlayerComponent* playerCmp =  PLAYER->getComponentT<PlayerComponent>();
//       playerCmp->ammo = NULL;
//     }

    remRegistry();
  }

  void ProjectileComponent::addRegistry( void )
  {
    PHYSICS->vRegisterComponent(getTypeID(), this);
  }

  void ProjectileComponent::remRegistry( void )
  {
    PHYSICS->vUnregisterComponent(getTypeID(), this);
  }

  TypeID ProjectileComponent::getTypeID( void ) const
  {
    return gCore->getTypeID<ProjectileComponent>();
  }

  Text ProjectileComponent::getTypeName( void ) const
  {
    return Text("ProjectileComponent");
  }

  void ProjectileComponent::serialize( Scope& data, bool read )
  {
    SERIAL_BeginSerializing(ProjectileComponent);
    SERIAL_Var(f32, "speed", speed);
    SERIAL_Var(vec3, "dirVector", dirVector);
    SERIAL_Var(u1, "shot", shot);
    player = NULL;
  }

  void ProjectileComponent::serialize( Component const* source )
  {
    ProjectileComponent const* rhs = SCAST(ProjectileComponent const*, source);
    speed = rhs->speed;
    dirVector = rhs->dirVector;
    player = NULL;
  }

  static u1 CheckKillZone(const vec3& vec)
  {
    return (vec.y < -KillHeight || vec.y > KillHeight ||
            vec.x > KillWidth   || vec.x < -KillWidth ||
            vec.z > 500.0f      || vec.z < -500.0f);
  }

  void ProjectileComponent::Update( f32 dt )
  {
    if(!shot && !player) return;

    Body* body = parent()->getComponentT<Body>();

    if(state == shotByPlayer || body->getPos().x < 340.0f)
      body->setIsCollisionEnabled(true);
    else if(state != shotByPlayer)
      body->setIsCollisionEnabled(false);

    if(player)
    {
      ModelComponent* model = player->parent()->getComponentT<ModelComponent>();
      body->setPos(player->getPos() + vec3(0, 10, 0) + model->GetDirection() * 60);
    }
    else
    {
      ModelComponent* model = parent()->getComponentT<ModelComponent>();
      if(model)
        model->Pitch(pitchSpeed);
      
      body->m_current.m_position += dirVector * speed * dt;
    }

    if(CheckKillZone(body->getPos() ) )
      parent()->destroy();
  }

  void ProjectileComponent::Shoot(vec3& direction, f32 speed_, f32 pitchSpeed_)
  {
    dirVector = direction;
    speed = speed_;
    pitchSpeed = pitchSpeed_;
    dirVector.Normalize();
    shot = true;

    if(player)
    {
      player = NULL;
      state = shotByPlayer;
    }
    else
      state = shotByBoss;
  }

  void ProjectileComponent::Catch( Body* playerBody )
  {
    shot = false;
    player = playerBody;
    ModelComponent* pmodel = player->parent()->getComponentT<ModelComponent>();
    ModelComponent* model = parent()->getComponentT<ModelComponent>();

    vec3& up = model->GetUp();
    up = vec3(0,1,0);
    model->SetDirection(pmodel->GetDirection());
    state = caughtByPlayer;
  }

}