/*! ====================================================================================================================
  @file gfxparticlemanager.h

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @author Branden Turner
  @date Feb.25.2012

  @brief 
    Manages particle systems

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#include "gfxparticlesystem.h"
#include "dx9device.h"

namespace Grimelins {


class ParticleManager
{
public:
  void Init(DX9Device* device);
  void Shutdown(void);

  void Add(ParticleSystem* system);
  void Remove(ParticleSystem* system);
  bool DoesExist(ParticleSystem* system);

  void Update(void);
  void Render(DX9Device* device);

  void KillBB( u32 id );
  void SetBBPos( u32 id, vec3& pos );
 
private:
  void FillVertexBuffer( u32& numParticlesRender, ParticleSystem* sys );
  static bool SortZ( ParticleSystem* lhs, ParticleSystem* rhs );
  typedef std::vector<ParticleSystem*> Systems;

  Systems mSystems;
  u32 mMaxParticles;
  LPDIRECT3DVERTEXBUFFER9		m_pVertexBuffer;
  Systems mRemovedSystems;
};


} // namespace Grimelins