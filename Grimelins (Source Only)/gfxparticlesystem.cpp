/*! ====================================================================================================================
  @file gfxparticlesystem.cpp

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @author Branden Turner
  @date Feb.25.2012

  @brief 
    Particle system base class implementation

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "gfxparticlesystem.h"
#include "dx9resourcemanager.h"
#include "graphics.h"
#include "camera.h"
#include "scriptGlobal.h"

namespace Grimelins {

  static u32 bbID = 0;

//////////////////////////////////////////////////////////////////////////
// Autos




ParticleSystem::ParticleSystem(INIT_INFO const* initInfo)
  : mDevice(initInfo->Device),
    mBlendMode(initInfo->BlendMode),
    mSystemName(initInfo->SystemName),
    mNumAlive(0),
    mPosition(initInfo->Position)
{
  ErrorIf(mDevice == nullptr, "Need device to allocate texture!");
  ErrorIf(mSystemName.size() == 0, "Need a unique particle system id!");
  
  if (initInfo->Texture.Exists())
    mTexture = mDevice->GetRM()->GetTextureFromFile(initInfo->Texture.RelPath());
  else Error("Can not load particle system, %s texture is missing.", initInfo->Texture.FileName().c_str());

  //The ID of any non billboarded quad is simply unsigned max
  mID = (u32)-1;
  mDraw = true;
}

ParticleSystem::~ParticleSystem(void)
{
  // Implemented as virtual for children
}

//////////////////////////////////////////////////////////////////////////
// Defaults

void ParticleSystem::InitShape(unsigned index)
{
  ErrorIf(index < 0U || index > mShapes.size(), "Out of bound index!");

  // Compute camera space position of particle
  ScriptMatrix4 view = GRAPHICS->getSceneCamera()->getMatrix();
  Vector3 csPos = view.MulPnt(mParticles[index].mPosition);

  mCPosition = csPos;

  // Set up billboard vertex positions
  mShapes[index].vertex[0].pos  = csPos + Vector3( -mParticles[index].mScale.x, mParticles[index].mScale.y, 0);
  mShapes[index].vertex[1].pos  = csPos + Vector3( -mParticles[index].mScale.x, -mParticles[index].mScale.y, 0);
  mShapes[index].vertex[2].pos  = csPos + Vector3( mParticles[index].mScale.x, mParticles[index].mScale.y, 0);
  mShapes[index].vertex[3].pos  = csPos + Vector3( mParticles[index].mScale.x, -mParticles[index].mScale.y, 0);

  // Set up texture coords
  mShapes[index].vertex[0].tex = vec2( 0.0f,  0.0f);
  mShapes[index].vertex[1].tex = vec2( 0.0f,  1.0f);
  mShapes[index].vertex[2].tex = vec2( 1.0f,  0.0f);
  mShapes[index].vertex[3].tex = vec2( 1.0f,  1.0f);
}

bool ParticleSystem::IntegrateParticles()
{
  static u32 numDeadParticles = 0;
  float minX, minY, minZ, maxX, maxY, maxZ;

  minX = minY = minZ = FLT_MAX;
  maxX = maxY = maxZ = -FLT_MAX;

  //iterate through all particles and integrate them
  for(unsigned i = 0; i < mParticles.size(); ++i)
  {
    Particle& part = mParticles[i];

    //Update the particle if it has energy
    if(part.mEnergy > 0)
    {
      part.mPosition = part.mLastPosition + part.mVelocity * GRAPHICS->getDT();
      part.mLastPosition = part.mPosition;

      //Computing bounding volume min/max
      if(part.mPosition.x < minX)
        minX = part.mPosition.x;
      else if(part.mPosition.x > maxX)
        maxX = part.mPosition.x;

      if(part.mPosition.y < minY)
        minY = part.mPosition.y;
      else if(part.mPosition.y > maxY)
        maxY = part.mPosition.y;

      if(part.mPosition.z < minZ)
        minZ = part.mPosition.z;
      else if(part.mPosition.z > maxZ)
        maxZ = part.mPosition.z;

      --part.mEnergy;

      //kill any dead particles
      if(part.mEnergy <= 0)
      {
        --mNumAlive;
        
        //Swap out the dead particle with the end and delete it.
        Particles::iterator swap = --mParticles.end();
        Particles::iterator it   = mParticles.begin() + i;
        *it = *swap;
        ++numDeadParticles;
      }
      else
        InitShape(i);
    }
  }

  for(u32 i = 0; i < numDeadParticles; ++ i)
    mParticles.pop_back();

  numDeadParticles = 0;

  //recompute the bounding volume of the system
  vec3 min(minX, minY, minZ);

  float h, w, d;

  h = abs(maxY - minY);
  w = abs(maxX - minX);
  d = abs(maxZ - minZ);

  mBound.Extend = vec3( w * 0.5f, h * 0.5f, d * 0.5f);
  mBound.Center = min + mBound.Extend;

  //kill the system if necessary
  if(mNumAlive <= 0)
    return false;

  return true;
}

bool ParticleSystem::SortZ( ParticleVerts& lhs, ParticleVerts& rhs )
{
  return lhs.vertex[0].pos.z < rhs.vertex[0].pos.z;
}




Explosion::Explosion( INIT_INFO const* initInfo ) : ParticleSystem(initInfo)
{
  //Allocate shapes and particle info
  mNumAlive = initInfo->NumParticles;
  mParticles.resize(mNumAlive);
  mShapes.resize(mNumAlive);

  //Go through all particles
  for(s32 i = 0; i < mNumAlive; ++i)
  {
    //Start them at my position
    mParticles[i].mLastPosition = mParticles[i].mPosition = mPosition;

    //Give them a random velocity/direction
    mParticles[i].mVelocity.x = SCRIPTGLOBALS->randfr(-500.0f, 400.0f);
    mParticles[i].mVelocity.y = SCRIPTGLOBALS->randfr(-500.0f, 300.0f);
    mParticles[i].mVelocity.z = SCRIPTGLOBALS->randfr(-500.0f, 350.0f);
    mParticles[i].mScale.x    = mParticles[i].mScale.y     = SCRIPTGLOBALS->randfr(10.0f, 17.0f);

    //init their shapes
    InitShape((u32)i);

    //Give them a random energy between 15 and 30
    mParticles[i].mEnergy = SCRIPTGLOBALS->randir(15, 25);
  }
}


Explosion::~Explosion()
{
}

bool Explosion::Update()
{
  return IntegrateParticles();
}

void Explosion::ResetParticle( Particle* p )
{
  //Explosions are so fast that we don't recycle particles
}


Dust::Dust( INIT_INFO const* initInfo ) : ParticleSystem(initInfo)
{
  //Allocate shapes and particle info
  mNumAlive = initInfo->NumParticles;
  mParticles.resize(mNumAlive);
  mShapes.resize(mNumAlive);

  //Go through all particles
  for(s32 i = 0; i < mNumAlive; ++i)
  {
    //Start them at my position
    mParticles[i].mLastPosition = mParticles[i].mPosition = mPosition;

    //Give them a random velocity/direction in the xz plane
    mParticles[i].mVelocity.x = SCRIPTGLOBALS->randfr(-200.0f, 200.0f);
    mParticles[i].mVelocity.y = 20.0f;
    mParticles[i].mVelocity.z = SCRIPTGLOBALS->randfr(-200.0f, 200.0f);
    mParticles[i].mScale.x    = mParticles[i].mScale.y      = SCRIPTGLOBALS->randfr(2.0f, 8.0f);

    //init their shapes
    InitShape((u32)i);

    //Give them a random energy between 35 and 45
    mParticles[i].mEnergy = SCRIPTGLOBALS->randir(45, 60);
  }
}

Dust::~Dust()
{

}

bool Dust::Update()
{
  f32 sqRadius = 5000.0f;
  bool ret;
  ret = IntegrateParticles();

  for(s32 i = 0; i < mNumAlive; ++i)
  {
    
    vec3 radCheck = (mPosition - mParticles[i].mPosition);

    if(radCheck.LengthSq() >= sqRadius)
      ResetParticle(&mParticles[i]);
  }

  return ret;
}

void Dust::ResetParticle( Particle* p )
{
  //Set the particle back to the center of the dust cloud
  p->mLastPosition = p->mPosition = mPosition;
}


LineEmitter::LineEmitter( INIT_INFO const* initInfo, vec3& p2 ) : ParticleSystem(initInfo)
{
  //Allocate shapes and particle info
  mNumAlive = initInfo->NumParticles;
  mParticles.resize(mNumAlive);
  mShapes.resize(mNumAlive);

  //Go through all particles
  for(s32 i = 0; i < mNumAlive; ++i)
  {
    //Start them at a random point on this line
    mParticles[i].mLastPosition = mParticles[i].mPosition = mPosition;

    //Send them off in whatever direction we choose
    mParticles[i].mVelocity.x = SCRIPTGLOBALS->randfr(-200.0f, 200.0f);
    mParticles[i].mVelocity.y = SCRIPTGLOBALS->randfr(-200.0f, 200.0f);
    mParticles[i].mVelocity.z = SCRIPTGLOBALS->randfr(-200.0f, 200.0f);
    mParticles[i].mScale.x    = mParticles[i].mScale.y       = SCRIPTGLOBALS->randfr(2.0f, 8.0f);

    //init their shapes
    InitShape((u32)i);

    //Give them a random energy between 35 and 45
    mParticles[i].mEnergy = SCRIPTGLOBALS->randir(45, 60);
  }
}

LineEmitter::~LineEmitter()
{
}

bool LineEmitter::Update()
{
  static f32 sqRadius = 200.0f;
  bool ret;
  ret = IntegrateParticles();

  for(s32 i = 0; i < mNumAlive; ++i)
  {

    vec3 radCheck = (mPosition - mParticles[i].mPosition);

    if(radCheck.LengthSq() >= sqRadius)
      ResetParticle(&mParticles[i]);
  }

  return ret;
}

void LineEmitter::ResetParticle( Particle* p )
{
  //Set the particle back to the center of the dust cloud
  p->mLastPosition = p->mPosition = mPosition;
}


Sparks::Sparks( INIT_INFO const* initInfo, vec3& dir, bool grav ) : ParticleSystem(initInfo)
{
  //Allocate shapes and particle info
  mNumAlive = initInfo->NumParticles;
  mParticles.resize(mNumAlive);
  mShapes.resize(mNumAlive);
  gravity = grav;

  //Go through all particles
  for(s32 i = 0; i < mNumAlive; ++i)
  {
    //Start them at my position
    mParticles[i].mLastPosition = mParticles[i].mPosition = mPosition;

    //Send them off in whatever direction we choose
    mParticles[i].mVelocity   = dir * SCRIPTGLOBALS->randfr(20.0f, 60.0f);
    mParticles[i].mScale.x    = mParticles[i].mScale.y = SCRIPTGLOBALS->randfr(2.0f, 5.0f);

    //init their shapes
    InitShape((u32)i);

    //Give them a random energy between 35 and 45
    mParticles[i].mEnergy = SCRIPTGLOBALS->randir(10, 15);
  }
}

Sparks::~Sparks()
{
}


bool Sparks::Update()
{
  static f32 gravSpeed = 800.0f;

  bool ret = IntegrateParticles();

  if(gravity)
  {
    for (s32 i = 0; i < mNumAlive; ++i)
    {
      mParticles[i].mVelocity.y -= gravSpeed * GRAPHICS->getDT();
    }
  }

  return ret;
}

void Sparks::ResetParticle( Particle* p )
{
}


ConeEmitter::ConeEmitter( INIT_INFO const* initInfo, vec3& dir, f32 coneAngle ) : ParticleSystem(initInfo)
{
  //Allocate shapes and particle info
  mNumAlive = initInfo->NumParticles;
  mParticles.resize(mNumAlive);
  mShapes.resize(mNumAlive);
  cAngle = coneAngle;

  //Go through all particles
  for(s32 i = 0; i < mNumAlive; ++i)
  {
    //Start them at my position
    mParticles[i].mLastPosition = mParticles[i].mPosition = mPosition;

    //Send them off in whatever direction we choose
    mParticles[i].mVelocity   = dir;
    mParticles[i].mScale.x    = mParticles[i].mScale.y       = SCRIPTGLOBALS->randfr(2.0f, 8.0f);

    //init their shapes
    InitShape((u32)i);

    //Give them a random energy between 35 and 45
    mParticles[i].mEnergy = SCRIPTGLOBALS->randir(45, 60);
  }
}

ConeEmitter::~ConeEmitter()
{
}

bool ConeEmitter::Update()
{
  bool ret = IntegrateParticles();

  //If outside the cone, reset

  return ret;
}

void ConeEmitter::ResetParticle( Particle* p )
{
  //Set the particle back to the center of the dust cloud
  p->mLastPosition = p->mPosition = mPosition;
}

static f32 bSpeed = 140.0f;
static f32 bL     = -15.0f;
static f32 bU     = 15.0f;

BillBoard::BillBoard( INIT_INFO const* initInfo, f32 scaleX, f32 scaleY, u1 bobs ) : ParticleSystem(initInfo)
{
  //Allocate shapes and particle info
  mNumAlive = initInfo->NumParticles;
  mParticles.resize(mNumAlive);
  mShapes.resize(mNumAlive);

  startY = mPosition.y;

  //Go through all particles
  for(s32 i = 0; i < mNumAlive; ++i)
  {
    //Start them at my position
    mParticles[i].mLastPosition = mParticles[i].mPosition = mPosition;

    //Set up the scale of the billboard
    mParticles[i].mScale      = vec2(scaleX, scaleY);

    //init their shapes
    InitShape((u32)i);

    mID = bbID++;

    //Give each billboard no energy, so that it's not updated except in camera space
    mParticles[i].mEnergy = 0;
  }

  dead = false;
  bob = bobs;

  bobUpperBound = bU;
  bobLowerBound = bL;
  bobSpeed = bSpeed;
  mDraw = false;
}

BillBoard::~BillBoard()
{
}


bool BillBoard::Update()
{
  if(bob)
  {
    if(mPosition.y >= startY + bobUpperBound)
      bobSpeed = -bSpeed;
    else if(mPosition.y <= startY + bobLowerBound)
      bobSpeed = bSpeed;

    mPosition.y += bobSpeed * GRAPHICS->getDT();

    for(s32 i = 0; i < mNumAlive; ++i)
      mParticles[i].mLastPosition = mParticles[i].mPosition = mPosition;
  }

  for(u32 i = 0; i < mParticles.size(); ++i)
    InitShape(i);

  if(dead)
    return false;

  return true;
}

void BillBoard::ResetParticle( Particle* p )
{
  //We don't really need to reset a billboard yet.
}

void BillBoard::SetPos( vec3& pos )
{
  if(mPosition.x != pos.x || mPosition.z != pos.z)
  {  
    mPosition = pos;
    startY = pos.y;
  }
}

} // namespace Grimelins