/*! ====================================================================================================================
  @file gfxparticlemanager.cpp

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @author Branden Turner
  @date Feb.26.2012

  @brief 
    Particle manager implementation

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "gfxparticlemanager.h"
#include "dx9device.h"
#include "dx9texturetype.h"
#include "graphics.h"

namespace Grimelins {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Particle Manager

//////////////////////////////////////////////////////////////////////////
// Public Autos

  

void ParticleManager::Init(DX9Device* device)
{
  mSystems.reserve(32);
  mMaxParticles = 30000;
  HR(device->GetDevice()->CreateVertexBuffer( 
                                          mMaxParticles * sizeof(Vertex_XYZ_UV) * 6, 
                                          D3DUSAGE_WRITEONLY, 
                                          0,
                                          D3DPOOL_MANAGED, 
                                          &m_pVertexBuffer, NULL ));
}

void ParticleManager::Shutdown(void)
{
  SafeRelease(m_pVertexBuffer);
  DeleteContainer(mRemovedSystems);
}

void ParticleManager::Add(ParticleSystem* system)
{
  ErrorIf(std::find(mSystems.begin(), mSystems.end(), system) != mSystems.end(), "Duplicate Particle System Addition!");
  mSystems.push_back(system);
}

void ParticleManager::Remove(ParticleSystem* system)
{
  Systems::iterator it = std::find(mSystems.begin(), mSystems.end(), system);

  if (it != mSystems.end())
  {
    Systems::iterator swap = --mSystems.end();
    *it = *swap;
    mSystems.erase(swap, mSystems.end());
    mRemovedSystems.push_back(system);
  }
}

bool ParticleManager::DoesExist(ParticleSystem* system)
{
  return std::find(mSystems.begin(), mSystems.end(), system) != mSystems.end();
}

void ParticleManager::Update(void)
{
  Systems::iterator system = mSystems.begin();
  Systems::iterator endSystem = mSystems.end();

  std::vector<ParticleSystem*> deadSystems;

  for (; system != endSystem; ++system)
  {
    if(!(*system)->Update())
      deadSystems.push_back(*system);
  }

  for(unsigned i = 0; i < deadSystems.size(); ++i)
    Remove(deadSystems[i]);

  deadSystems.clear();
}

void ParticleManager::FillVertexBuffer( u32& numParticlesRender, ParticleSystem* sys )
{
  Vertex_XYZ_UV *pVertices;
  ParticleShapes& particleBillBoards = sys->mShapes;

  std::sort(particleBillBoards.begin(), particleBillBoards.end(), ParticleSystem::SortZ);

  // Lock the vertex buffer (4 verts per shape)
  if( FAILED( m_pVertexBuffer->Lock( 0, particleBillBoards.size() * sizeof(Vertex_XYZ_UV) * 6,	(void**) &pVertices, D3DLOCK_DISCARD)))
  {
    return;
  }

  // Update the vertices using the billboard buffer (6 verts at a time)
  for (UINT i = 0; i < particleBillBoards.size(); i++)
  {
    pVertices[0].pos = particleBillBoards[i].vertex[0].pos;
    pVertices[0].tex = particleBillBoards[i].vertex[0].tex;

    pVertices[1].pos = particleBillBoards[i].vertex[1].pos;
    pVertices[1].tex = particleBillBoards[i].vertex[1].tex;

    pVertices[2].pos = particleBillBoards[i].vertex[2].pos;
    pVertices[2].tex = particleBillBoards[i].vertex[2].tex;


    pVertices[3].pos = particleBillBoards[i].vertex[1].pos;
    pVertices[3].tex = particleBillBoards[i].vertex[1].tex;


    pVertices[4].pos = particleBillBoards[i].vertex[3].pos;
    pVertices[4].tex = particleBillBoards[i].vertex[3].tex;


    pVertices[5].pos = particleBillBoards[i].vertex[2].pos;
    pVertices[5].tex = particleBillBoards[i].vertex[2].tex;



    pVertices += 6;
    numParticlesRender++;
  }

  // Unlock the vertex buffer, we're done modifying vertices
  m_pVertexBuffer->Unlock();
}

bool ParticleManager::SortZ(ParticleSystem* lhs, ParticleSystem* rhs)
{
  return lhs->mCPosition.z < rhs->mCPosition.z;
}

void ParticleManager::Render(DX9Device* device)
{
  //Render particles with other alpha blended objects
  GRAPHICS->mModelMgr->SetTransluscentRenderState(device->GetDevice());

  std::sort(mSystems.begin(), mSystems.end(), ParticleManager::SortZ);

  device->SetShader(GRAPHICS->particlevs);
  device->SetShader(GRAPHICS->particleps);
  device->SetDecl(Vertex_XYZ_UV::Decl);

  device->SetVSData(&GRAPHICS->mDefaultShader.Projection, 4, 0);

  Systems::iterator system = mSystems.begin();
  Systems::iterator endSystem = mSystems.end();

  for (; system != endSystem; ++system)
  {
    ParticleSystem* sys = *system;

    //If the system is shown, draw it.
    if(sys->mDraw)
    {
      device->SetTexture(sys->mTexture.GetPtr(), 0);

      u32 numParticlesToRender = 0;
      FillVertexBuffer(numParticlesToRender, sys);

      // Render any particles
      if( numParticlesToRender )
      {
        device->GetDevice()->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof(Vertex_XYZ_UV) );
        device->DrawTris(numParticlesToRender * 2, 0);
      }
    }
  }
}

void ParticleManager::KillBB( u32 id )
{
  Systems::iterator system = mSystems.begin();
  Systems::iterator endSystem = mSystems.end();

  //Find the right ID and kill it.
  for (; system != endSystem; ++system)
  {
    if((*system)->ID() == id)
    {
      BillBoard* bb = SCAST(BillBoard*,*system);
      bb->MarkDead();
      break;
    }
  }
}

void ParticleManager::SetBBPos( u32 id, vec3& pos )
{
  Systems::iterator system = mSystems.begin();
  Systems::iterator endSystem = mSystems.end();

  //Find the right ID and set the position
  for (; system != endSystem; ++system)
  {
    if((*system)->ID() == id)
    {
      BillBoard* bb = SCAST(BillBoard*,*system);
      bb->mPosition = pos;
      break;
    }
  }
}




} // namespace Grimelins