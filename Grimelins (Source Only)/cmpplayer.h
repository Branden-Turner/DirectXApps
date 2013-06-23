/**********************************************************************/
/*
  
  \file
    cmpplayer.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Components for players.  This will hold player data and state information
    about the main character.
*/
/**********************************************************************/
#ifndef GRIMELIN_CMP_PLAYER_H
#define GRIMELIN_CMP_PLAYER_H

#include "component.h"

namespace Grimelins
{

  class ProjectileComponent;

  enum PlayerState
  {
    HasAmmo,
    Empty,
    Catching,
    typenum
  };

  enum MoveState
  {
    notmoving,
    moving,
    dodging, 
    movenum
  };

  class Body;

  SCRIPT_HeaderDefine(PlayerComponent);
  class PlayerComponent : public Component
  {
    public:
      void Update( f32 dt );
      PlayerComponent();
      virtual ~PlayerComponent();

    //Script Interface
    public:
      void TakeDamage( s32 damage );
      void Dodge(f32 dodgeSpeed);
      void MoveTowardBoss(f32 moveSpeed);
      void MoveAwayFromBoss(f32 movesSpeed);
      void MoveAroundBossRight(f32 moveSpeed);
      void MoveAroundBossLeft(f32 moveSpeed);
      void Catching();


    //Component interface
    public:
      virtual void addRegistry( void );
      virtual void remRegistry( void );
      virtual TypeID getTypeID(void) const;
      virtual Text getTypeName(void) const;
      virtual void serialize( Scope& data, bool read );
      virtual void serialize( Component const* source);
      virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp);

    //Factory creation interface
    friend class Factory;
    friend class Object;
    friend class ProjectileComponent;
    private:
      static Component* create(void);
      void Death();
      void DrawHealth();
      void ShootProjectile();
      void UpdatePlayerPos();
      void UpdateLookAt();
      void UpdateDodge();
      void BoundBody(Body& body);
      s32 health;
      s32 startHealth;
      PlayerState state;
      ProjectileComponent* ammo;
      vec3 bossLookAt;
      vec3 bossRight;
      f32 distFromBoss;
      f32 movespeed;
      vec3 moveDir;
      vec3 throwDir;
      MoveState move;
      MoveState movePrev;
  };
}
#endif