/*! ====================================================================================================================
  @file integrator_RK4.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.16.2011

  @brief 
    Interface for Integrate_RK4

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_INTEGRATOR_RK4_H
#define VIRTUS_INTEGRATOR_RK4_H
#include "integrator.h"

namespace Grimelins
{
  //----------------------------------------------
  // Class Integrate_RK4
  // This class represents the RK4 physics
  // integrator
  //----------------------------------------------
  class Integrate_RK4: public Integrator
  {
  public:
    Integrate_RK4(void):Integrator(H("RK4")) {}   // constructor
    virtual ~Integrate_RK4(void) {}               // destructor
      
    virtual void Integrate(Body &body, f32 dt);   // Integrate with RK4

  private:
    // Holds the derivative info for RK4
    struct Derivative {
      vec3 m_velocity; 
      vec3 m_force;
      Quaternion m_spin;
      vec3 m_torque;
    };

    // Helper functions for RK4
    void Current(Body &body, f32 t, Derivative &D);
    void Next(Body &body, f32 t, f32 dt, Derivative &D);

  };
}; // namespace Grimelins

#endif // VIRTUS_INTEGRATOR_RK4_H