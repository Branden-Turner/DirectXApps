/*! ====================================================================================================================
  @file integrator.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.23.2011

  @brief 
    Implementation of updating information within the Body

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "integrator.h"
#include "compbody.h"

namespace Grimelins
{
    void Integrator::updateForces(Body &body, f32 dt, vec3 &force, vec3 &torque)
    {
      // gravity
      vec3 gravity_direction = vec3(0.0f,1.0f,0.0f);

      // at some point, we can change this to a variable so we can change it within each body...
      force = gravity_direction * -10.0f;

      // sine force to add some randomness to the motion
      force.x += 10.0f * sin((dt*0.9f + 0.5f)/halfturnPI);
      force.y += 11.0f * sin((dt*0.5f + 0.4f)/halfturnPI);
      force.z += 12.0f * sin((dt*0.7f + 0.9f)/halfturnPI);

      // sine torque to get some spinning action
      torque.x = 1.0f * sin((dt*0.9f + 0.5f)/halfturnPI);
      torque.y = 1.1f * sin((dt*0.5f + 0.4f)/halfturnPI);
      torque.z = 1.2f * sin((dt*0.7f + 0.9f)/halfturnPI);

      // damping torque, so that the accumulated w doesn't get out of control
      torque -= body.m_current.m_angVelocity * 0.2f;
    }

    void Integrator::Evaluate(Body &body)
    {
      // update the body based on updated information
      body.m_current.m_velocity = body.m_current.m_momentum * body.m_current.m_invMass;
      body.m_current.m_angVelocity = body.m_current.m_angMomentum * body.m_current.m_invInerTensor;
      body.m_current.m_orientation.normalize();
      body.m_current.m_spin = (0.5f) * Quaternion(0.0f, body.m_current.m_angVelocity) * body.m_current.m_orientation;
    }
}; // namespace Grimelins