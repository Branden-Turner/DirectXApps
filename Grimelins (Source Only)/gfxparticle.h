/*! ====================================================================================================================
  @file gfxparticle.h

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.25.2012

  @brief 
    Particle definition

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once

#include "dx9vertextype.h"

namespace Grimelins {

struct Particle
{
  vec3  mPosition;        // Position after most current integration step
  vec3  mLastPosition;    // Position at last integration step
  vec3  mVelocity;        // Velocity of particle
  vec4  mColor;           // Color of particle
  s32   mEnergy;          // Energy as a function of time (you could call this "Age")
  vec2   mScale;           // The size of the particle
};

typedef std::vector<Particle> Particles;

struct ParticleVerts
{
  Vertex_XYZ_UV vertex[4]; // The 4 positions of the quad verteces (measured from Particle position with scale along x,y)
};

typedef std::vector<ParticleVerts> ParticleShapes;

} // namespace Grimelins