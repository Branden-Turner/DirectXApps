/*! ====================================================================================================================
  @file integrator_RK4.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.16.2011

  @brief 
    Implementation of RK4

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "compbody.h"
#include "integrator.h"
#include "integratorrk4.h"

namespace Grimelins
{
    // this is for the integration steps, so that this division only happens once
    static const f32 SIXTH = 1.0f / 6.0f;

    void Integrate_RK4::Integrate( Body &body, f32 dt )
    {
      // get the system time, this was a pointer before
      f32 t = gCore->Time.Get();
    
      Derivative a,b,c,d;
      // call the helper functions that are steps in the integration
      Current(body, t, a);
      Next(body, t, dt, b);
      Next(body, t, dt, c);
      Next(body, t, dt, d);

      // update the body with the new information
      body.m_current.m_position += (a.m_velocity + (b.m_velocity + c.m_velocity) * 2.0f + d.m_velocity) * SIXTH * dt;
      body.m_current.m_momentum += (a.m_force + (b.m_force + c.m_force) * 2.0f + d.m_force) * SIXTH * dt;
      body.m_current.m_orientation += (a.m_spin + (b.m_spin + c.m_spin) * 2.0f + d.m_spin) * SIXTH * dt;
      body.m_current.m_angMomentum += (a.m_torque + (b.m_torque + c.m_torque) * 2.0f + d.m_torque) * SIXTH * dt;
      
      // updates other data in the body with this information
      Evaluate(body);
    }

    void Integrate_RK4::Current(Body &body, f32 t, Derivative &D)
    {
      // updates the Derivative data with the current body data
      D.m_velocity = body.m_current.m_velocity;
      D.m_spin = body.m_current.m_spin;
      // calculates torque and forces(gravity, frictions, etc) with system time
      updateForces(body, t, D.m_force, D.m_torque);
    }

    void Integrate_RK4::Next(Body &body, f32 t, f32 dt, Derivative &D)
    {
      // updates the Body's current data with derivative data
      body.m_current.m_position += D.m_velocity * dt;
      body.m_current.m_momentum += D.m_force * dt;
      body.m_current.m_orientation += D.m_spin * dt;
      body.m_current.m_angMomentum += D.m_torque * dt;
      // updates other data in the body with this information
      Evaluate(body);

      // update the Derivative information with the current body data
      D.m_velocity = body.m_current.m_velocity;
      D.m_spin = body.m_current.m_spin;
      // calculates torque and forces with the system time + dt
      updateForces(body, t + dt, D.m_force, D.m_torque);
    }

}; // namespace Grimelins