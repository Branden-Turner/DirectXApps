/*! ====================================================================================================================
  @file gfxparticlesystem.h

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @author Branden Turner
  @date Feb.25.2012

  @brief 
    A particle system manages a bunch of particles and coordinates them in a way to form a nice visual effect.

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#include "geometry.h"
#include "gfxparticle.h"
#include "appfiles.h"
#include "d3d9types.h"
#include "dx9device.h"


namespace Grimelins {

using namespace Application;

class ParticleSystem
{
  friend class ParticleManager;

public: // Declaration
  struct INIT_INFO
  {
    INIT_INFO(Text& texturefile):Texture(texturefile){};
    DX9Device*  Device;       /// Device used to allocate and render
    FilePath    Texture;      /// Path to the texture of the particle
    D3DBLEND    BlendMode;    /// Blend mode used when particles are layered
    Text        SystemName;   /// The name of the system, should be unique
    s32         NumParticles; /// Number of particles this system holds initially
    vec3        Position;     /// The position of this particle system
  };

public: // Autos
  ParticleSystem(INIT_INFO const* initInfo);
  virtual ~ParticleSystem(void);

public: // Virtual
    /// Updates positions of particles and updates/computes bounding volume of system.
    /// Returns true if particles are still being updated, false if no particles are alive.
  virtual bool Update(void) = 0;

    /// Sets up the shape for the particle system. If this function is not overloaded
    /// a default shape of square is used.
  virtual void InitShape(unsigned index);

    /// Resets a given particle to properties that are default to the particle system.
  virtual void ResetParticle(Particle* p) = 0;

  virtual bool IntegrateParticles();
  
  virtual u32 ID() const { return mID; };

  virtual void Hide() { mDraw = false; };
  virtual void Show() { mDraw = true; };
  virtual void SetPos(vec3& pos) { if(mPosition.x != pos.x || mPosition.z != pos.z) mPosition = pos; };

  static bool SortZ(ParticleVerts& lhs, ParticleVerts& rhs);

protected: // data
  
  ParticleSystem(){};
  DX9Device*                        mDevice;      /// Device used to allocate and render
  shrdptr<DX9Texture, NoDelete>     mTexture;     /// The texture the particle system is rendered to
  D3DBLEND                          mBlendMode;   /// The blend mode particles are layered with
  Text                              mSystemName;  /// Unique name of the system
  Particles                         mParticles;   /// Array of particles
  ParticleShapes                    mShapes;      /// Array of particle shapes (parallel to mParticles)
  s32                               mNumAlive;    /// Number of particles alive in this system
  AABB_V2                           mBound;       /// Bounding volume around all particles
  vec3                              mPosition;    /// Position of particle system
  u32                               mID;          /// ID of this particle system
  vec3                              mCPosition;   /// Camera space position
  u1                                mDraw;
};

class Explosion : public ParticleSystem
{
  public:
    Explosion(INIT_INFO const* initInfo);
    ~Explosion();
  public:
    bool Update();
    void ResetParticle(Particle* p);
};

class Dust : public ParticleSystem
{
  public:
    Dust(INIT_INFO const* initInfo);
    ~Dust();
  public:
    bool Update();
    void ResetParticle(Particle* p);
};

class LineEmitter : public ParticleSystem
{
  public:
    LineEmitter(INIT_INFO const* initInfo, vec3& p2);
    ~LineEmitter();
  public:
    bool Update();
    void ResetParticle(Particle* p);
};

class Sparks : public ParticleSystem
{
  public:
    Sparks(INIT_INFO const* initInfo, vec3& dir, bool grav);
    ~Sparks();
  public:
    bool Update();
    void ResetParticle(Particle* p);
  private:
    bool gravity;
};

class ConeEmitter : public ParticleSystem
{
  public:
    ConeEmitter(INIT_INFO const* initInfo, vec3& dir, f32 coneAngle);
    ~ConeEmitter();
  public:
    bool Update();
    void ResetParticle(Particle* p);
  private:
    vec3 direction;
    f32 cAngle;
};

class BillBoard : public ParticleSystem
{
  public:
    BillBoard(INIT_INFO const* initInfo, f32 scaleX, f32 scaleY, u1 bobs);
    ~BillBoard();
  public:
    bool Update();
    void ResetParticle(Particle* p);
    void MarkDead() { dead = true; };
    void SetPos(vec3& pos);
  private:   
    u1 dead;
    u1 bob;
    f32 bobLowerBound;
    f32 bobUpperBound;
    f32 bobSpeed;
    f32 startY;
};


} // namespace Grimelins