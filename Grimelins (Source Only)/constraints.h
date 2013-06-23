/*! ====================================================================================================================
  @file constraints.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Nov.7.2011

  @brief 
    Solving the Constraints and stuff :)

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_CONSTRAINTS_H
#define VIRTUS_CONSTRAINTS_H

namespace Grimelins
{
  class Body;

	class Constraint
  {
  public:
    // Constructors, Destructors
    Constraint(Body* body1, Body* body2, u32 numConst, u1 active);
    virtual ~Constraint();

    // Gettors
    const Body* getBody1() const;
    const Body* getBody2() const;
    u32 getNumContraints() const;
    u1 isActive() const;

    // virtuals for inherited class
    virtual void Jacobian() = 0;
    virtual void lower() = 0;
    virtual void upper() = 0;
    virtual void error() = 0;

  protected:
    // collision pair
    Body* m_body1;
    Body* m_body2;
    // constraint is active. This would imply we have different physics rules in
    // different areas. 
    u1  m_isActive;        
    u32 m_numConstraints;
  };

  class CollisionPoint: public Constraint
  {
  public:
    // Constructor, Destructor
    CollisionPoint(Body* body1, Body* body2, vec3& normal, f32 penDepth, std::vector<vec3>& points);
    virtual ~CollisionPoint();

    // Gettors
    vec3 getNormal() const;
    vec3 getContactPts(u32 index) const;
    u32  getNumPts() const;
    f32  getPenDepth() const;

    // Inherited Virtuals
    void Jacobian();    // calculates the Jacobian (a matrix) for all constraints
    void lower();       // gets the lower bound values
    void upper();       // gets the upper bound values
    void error();       // the error values of the constraints

  private:
    vec3 m_normal;      // normal of the contact
    f32  m_penDepth;    // penetration depth of the collision
    std::vector<vec3> m_points; // contact point(s) of the collision
    u32  m_numPts;      // number of contact points
  };

}; // namespace Grimelins

#endif // VIRTUS_CONSTRAINTS_H
