/**********************************************************************/
/*
  
  \file
    cmpplayer.cpp
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Components for players.  This will hold player data and state information
    about the main character.
*/
/**********************************************************************/
#include "precompiled.h"
#include "cmpplayer.h"
#include "object.h"
#include "dx9vertextype.h"
#include "dx9device.h"
#include "dx9buffers.h"
#include "dx9shaders.h"
#include "dx9resourcemanager.h"
#include "uisystem.h"
#include "graphics.h"
#include "factory.h"
#include "physics.h"
#include "cmpproj.h"
#include "compbody.h"
#include "sound.h"
#include "sysinput.h"


namespace Grimelins
{
  SCRIPT_ClassStart(PlayerComponent);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(getTypeName);
  SCRIPT_Method(getTypeID);
  SCRIPT_Method(TakeDamage);
  SCRIPT_Method(Catching);
  SCRIPT_Method(MoveTowardBoss);
  SCRIPT_Method(MoveAwayFromBoss);
  SCRIPT_Method(MoveAroundBossRight);
  SCRIPT_Method(MoveAroundBossLeft);
  SCRIPT_Method(Dodge);
  SCRIPT_ClassEnd();

  static const s32 playerHealth = 3;
  static const f32 catchSqDist = 25000;

  static Timer healthTimer(true);
  static Timer damageTimer(true);
  static Timer overlayPulse(true);
  static Timer catchTimer(true);
  static Timer dodgeTimer(false);
  static Timer canDodge(true);
  static Timer walkSoundTimer(true);
  static u1 overlaySwitch = false;


  PlayerComponent::PlayerComponent() : health(playerHealth), state(Empty), throwDir(1,0,0)
  {
    move = movePrev = notmoving;
  }

  PlayerComponent::~PlayerComponent()
  {
    remRegistry();
  }

  void PlayerComponent::Update( f32 dt )
  {
    //Update the look at vector to the boss's position
    UpdateLookAt();

    if(health < 3 && healthTimer.Get() > 4.0f)
    {
      ++health;
      SOUND->Play(HEAL);
      healthTimer.StartTimer();
    }

    DrawHealth();

    ModelComponent* model = parent()->getComponentT<ModelComponent>();

    //Dodge and eventually go back into the idle.
    if(move == dodging)
      UpdateDodge();
    else if(move == moving)
    {
      //Do we just cycle through the same animation or do we abruptly change?
      if(movePrev == moving)
        model->ChangeAnim(Text("cc_run"));
      else if(movePrev == notmoving)
        model->ChangeAnimNOW(Text("cc_run"));

      //You were moving, but if you don't move more, you stop
      movePrev = moving;
      move = notmoving;
    }
    else
    {
      //Do we just cycle through the same animation or do we abruptly change?
      if(movePrev == moving)
        model->ChangeAnimNOW(Text("cc_idle"));
      else if(movePrev == notmoving)
        model->ChangeAnim(Text("cc_idle"));

      //You weren't moving, and you'll continue to not move!
      movePrev = notmoving;
      move = notmoving;
    }
  }

  void PlayerComponent::TakeDamage( s32 damage )
  {
    if(damageTimer.Get() > 1.0f)
    {
      INPUT->setControllerVibration(0.7f, 0.7f, 1.0f);
      ModelComponent* model = parent()->getComponentT<ModelComponent>();
      model->ChangeAnimNOW(Text("cc_hit"));
      SOUND->Play(HURT);
      damageTimer.StartTimer();
      healthTimer.StartTimer();
      health -= damage;
      if(health <= 0)
      {
        GRAPHICS->mUISystem->ClearUIElemByType(HUD);
        Death();
      }
    }
  }


  void PlayerComponent::DrawHealth()
  {
    static DX9Texture* healthimg = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "health.png");
    static DX9Texture* healthoutimg = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "healthout.png");
    static UIElement healthElem;
    static vec2 firstPos(-0.035f,0.075f);
    static vec2 dimensions(0.1f,0.15f);

    static DX9Texture* overlayimg = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "overlay.png");
    static DX9Texture* overlayimg2 = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "overlay2.png");
    static UIElement overlayElem;

    //Pulse red when you have low health
    if(health < 2)
    {
      if(overlayPulse.Get() > 0.5f)
      {
        overlayPulse.StartTimer();
        overlaySwitch = !overlaySwitch;
      }

      if(overlaySwitch)
        overlayElem.tex = overlayimg;
      else
        overlayElem.tex = overlayimg2;
      
      overlayElem.pos = vec2(0.5f, 0.5f);
      overlayElem.dim = vec2(1.0f, 1.0f);
      overlayElem.type = HUD;

      GRAPHICS->mUISystem->AddUIElement(overlayElem);
    }

    //Draw the health based on how much health you have remaining.
    healthElem.tex = healthimg;
    healthElem.pos = firstPos;
    healthElem.dim = dimensions;
    healthElem.type = HUD;

    for(s32 i = 0; i < startHealth; ++i)
    {
      if(i > health - 1)
        healthElem.tex = healthoutimg;

      healthElem.pos.x += 0.075f;
      GRAPHICS->mUISystem->AddUIElement(healthElem);
    }
  }

  void PlayerComponent::Death()
  {
    ModelComponent* model = parent()->getComponentT<ModelComponent>();
    model->ChangeAnimNOW(Text("cc_hit"));
    gCore->Events.Post("game_over", MSG_PRIORITY_TOP);
  }

  void PlayerComponent::addRegistry( void )
  {
    FACTORY->vRegisterComponent(getTypeID(), this);
  }

  void PlayerComponent::remRegistry( void )
  {
    FACTORY->vUnregisterComponent(getTypeID(), this);
  }

  TypeID PlayerComponent::getTypeID( void ) const
  {
    return gCore->getTypeID<PlayerComponent>();
  }

  Text PlayerComponent::getTypeName( void ) const
  {
    return Text("PlayerComponent");
  }

  void PlayerComponent::serialize( Scope& data, bool read )
  {
    SERIAL_BeginSerializing(PlayerComponent);
    SERIAL_Var(s32, "health", health);
    startHealth = health;
  }

  void PlayerComponent::serialize( Component const* source )
  {
    PlayerComponent const* rhs = SCAST(PlayerComponent const*, source);
    startHealth = rhs->startHealth;
    health = rhs->health;
    state = rhs->state;
  }

  u1 PlayerComponent::handleEvent( hstring const& message, CmdArgs const& args, MessageType type, f32 timestamp )
  {
    return false;
  }

  Component* PlayerComponent::create( void )
  {
    return SCAST(Component*, new PlayerComponent());
  }

  void PlayerComponent::Catching()
  {
    if(state == HasAmmo)
      ShootProjectile();

    if(catchTimer.Get() > 0.5f)
    {
      catchTimer.StartTimer();
      Body* playerBody = parent()->getComponentT<Body>();
      Body* projBody = PHYSICS->getBodyWithinDistOfType(playerBody, Projectile, catchSqDist);

      if(projBody)
      {
        ProjectileComponent* proj = projBody->parent()->getComponentT<ProjectileComponent>();
        if(proj->getState() == shotByBoss)
        {
          state = HasAmmo;
          ammo = proj;
          proj->Catch(playerBody);
          SOUND->Play(CATCH);
          INPUT->setControllerVibration(0.25f, 0.25f, 0.25f);
        }
      }
    }

  }

  void PlayerComponent::ShootProjectile()
  {
    if(ammo)
    {
      ammo->Shoot(throwDir, 1500.0f, 5.0f);
      SOUND->Play(THROW);
      INPUT->setControllerVibration(0.25f, 0.25f, 0.25f);
    }
    state = Empty;
  }

  void PlayerComponent::Dodge( f32 dodgeSpeed )
  {
    if(move != dodging && canDodge.Get() > 0.7f)
    {
      //Play dodge animation
      ModelComponent* model = parent()->getComponentT<ModelComponent>();
      moveDir = model->GetDirection();
      model->ChangeAnimNOW(Text("cc_dodge"));
      SOUND->Play(JUMP);
      move = dodging;
      movespeed = dodgeSpeed;
      INPUT->setControllerVibration(0.25f, 0.25f, 0.25f);
      dodgeTimer.StartTimer();
      canDodge.StartTimer();
    }
  }

  void PlayerComponent::MoveTowardBoss( f32 moveSpeed )
  {
    //Rewrite using new script global boss/player pointers  see scriptGlobal.h
//     if(move == dodging) return;
// 
//     movespeed = moveSpeed;
// 
//     moveDir = bossLookAt;
// 
//     //The throwing direction is at the boss if you're moving toward him
//     vec3 bossPos = BOSS->getComponentT<Body>()->getPos();
//     ModelComponent* model = parent()->getComponentT<ModelComponent>();
//     throwDir = (bossPos - model->GetPos()).Normalize();
// 
//     move = moving; 
// 
//     UpdatePlayerPos();
  }


  void PlayerComponent::MoveAwayFromBoss( f32 movesSpeed )
  {
    if(move == dodging) return;

    movespeed = movesSpeed;

    moveDir = -bossLookAt;
    throwDir = moveDir;

    move = moving;

    UpdatePlayerPos();
  }


  void PlayerComponent::MoveAroundBossRight( f32 moveSpeed )
  {
    if(move == dodging) return;

    movespeed = moveSpeed;

    moveDir = bossRight;
    throwDir = moveDir;

    move = moving;

    UpdatePlayerPos();
  }

  void PlayerComponent::MoveAroundBossLeft( f32 moveSpeed )
  {
    if(move == dodging) return;

    movespeed = moveSpeed;

    moveDir = -bossRight;
    throwDir = moveDir;

    move = moving;

    UpdatePlayerPos();
  }

  void PlayerComponent::UpdateLookAt()
  {
    //Rewrite using new script global boss/player pointers  see scriptGlobal.h
//     ModelComponent* model = parent()->getComponentT<ModelComponent>();
// 
//     //Orbit the boss's original position no matter what.  We can change this if we need to.
//     static vec3 bossPos = BOSS->getComponentT<Body>()->getPos();
//     bossPos.y = model->GetPos().y;
// 
//     bossLookAt = bossPos - model->GetPos();
//     bossLookAt.Normalize();
// 
//     bossRight = bossLookAt.Cross(model->GetUp());
  }

  void PlayerComponent::UpdateDodge()
  {
    if(dodgeTimer.Get() < 0.58f)
      UpdatePlayerPos();
    else
    {
      SOUND->Play(LAND);
      INPUT->setControllerVibration(0.25f, 0.25f, 0.25f);
      move = notmoving;
    }
  }

  static const f32 xNeg = -250.0f;
  static const f32 xPos =  300.0f;
  static const f32 zNeg = -440.0f;
  static const f32 zPos =  440.0f;

  void PlayerComponent::BoundBody( Body& body )
  {
    vec3 bodyPos = body.getPos();
    if(bodyPos.x < xNeg) bodyPos.x = xNeg;
    else if(bodyPos.x > xPos) bodyPos.x = xPos;

    if(bodyPos.z < zNeg) bodyPos.z = zNeg;
    else if(bodyPos.z > zPos) bodyPos.z = zPos;

    body.setPos(bodyPos);
  }

  void PlayerComponent::UpdatePlayerPos()
  {
    if(move == moving && walkSoundTimer.Get() > 0.5f)
    {
      SOUND->Play(WALK);
      walkSoundTimer.StartTimer();
    }

    Body& body = *parent()->getComponentT<Body>();

    ModelComponent& model = *parent()->getComponentT<ModelComponent>();

    model.SetDirection(moveDir);

    body.setPos(body.getPos() + moveDir * movespeed * GRAPHICS->thisFrameDt); 

    BoundBody(body);
  }
}
