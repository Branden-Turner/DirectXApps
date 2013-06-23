/*! ====================================================================================================================
  @file integrators.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.9.2011

  @brief 
    Integrators for physics

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_INTEGRATORS_H
#define VIRTUS_INTEGRATORS_H

namespace Grimelins
{

    class Body;
    //----------------------------------------------
    // Class Integrator
    // This class can represent several integrators
    //----------------------------------------------
    class Integrator
    {
    public:
      Integrator(hstring const &type): m_type(type) {};   // Constructor
      virtual ~Integrator(void) {};                       // Destructor
      virtual hstring getType(void) {return m_type;}      // Which integrator?
      // This will be done within any integrator we choose (RK4, Verlet, etc)
      virtual void Integrate(Body &body, f32 dt)=0;
  
    protected:
      // Calculates forces and information for the Body
      virtual void updateForces(Body &body, f32 dt, vec3 &force, vec3 &torque);   
      virtual void Evaluate(Body &body); 

	  f32 gravity;
    private:
      hstring m_type; // type of integrator

    };

}; // namespace Grimelins

#endif // VIRTUS_INTEGRATORS_H