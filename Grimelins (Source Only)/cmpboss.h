/**********************************************************************/
/*
  
  \file
    cmpboss.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This will control the boss.
*/
/**********************************************************************/
#ifndef GRIMELIN_CMP_BOSS_H
#define GRIMELIN_CMP_BOSS_H

#include "component.h"

namespace Grimelins
{
  enum BirdBossState
  {
    idle,
    shooting,
    breathing,
    bashing,
    hurt,
    death,
    birdStateNum
  };

  class Object;
  class Body;

  SCRIPT_HeaderDefine(BossComponent);
  class BossComponent : public Component
  {
    public:
      void Update( f32 dt );
      BossComponent();
      virtual ~BossComponent();

    //Script interface, this will get taken out in future builds
    public:

      void TakeDamage(s32 damage);

      //Component interface
    public:
      virtual void addRegistry(void);
      virtual void remRegistry(void);
      virtual TypeID getTypeID(void) const;
      virtual Text getTypeName(void) const;
      virtual void serialize( Scope& data, bool read );
      virtual void serialize( Component const* source);
      virtual u1 handleEvent(hstring const& message, CmdArgs const& args, MessageType type, f32 timestamp);

    //Factory creation interface
    friend class Factory;
    friend class Object;
    private:
      static Component* create(void);
      void DrawHealth();
      void Death();
      void ShootAtPlayer();
      void HeadBash();
      void BreathAttack();
      void DoAttack();
      void SpawnRocks();
      void UpdatePos();
      void BoundBody( Body& body );
      
      s32 health;
      s32 startHealth;
      s32 attackChoice;
      vec3 headPos;
      BirdBossState state;
      Object* bosshead;
      Object* beam;
      vec3 spawnSpot;
      vec3 retSpot;
      vec3 moveDir;
      vec3 curSpot;
      f32 moveSpeed;
      u1 vulnerable;
      u1 idling;
      u1 back;
      u1 startPosSet;
      u1 hurtSmash;
  };
}

#endif