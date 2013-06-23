/*! ====================================================================================================================
  @file constraints.cpp

  Codename Grimelins

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Nov.7.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "constraints.h"
#include "compbody.h"

namespace Grimelins
{
	Constraint::Constraint(Body* body1, Body* body2, u32 numConst, u1 active)
    : m_body1(body1), m_body2(body2), m_numConstraints(numConst), m_isActive(active)
  {

  }

  Constraint::~Constraint()
  {
  
  }

  const Body* Constraint::getBody1() const
  {
    return m_body1;
  }

  const Body* Constraint::getBody2() const
  {
    return m_body2;
  }

  u32 Constraint::getNumContraints() const
  {
    return m_numConstraints;
  }

  u1 Constraint::isActive() const
  {
    return m_isActive;
  }

  CollisionPoint::CollisionPoint(Body* body1, Body* body2, vec3& normal, f32 penDepth, std::vector<vec3>& points)
    :Constraint(body1,body2,3*points.size(),true), m_normal(normal), m_penDepth(penDepth),
     m_points(points), m_numPts(points.size())
  {
    
  }
    
  CollisionPoint::~CollisionPoint()
  {

  }

  vec3 CollisionPoint::getNormal() const
  {
    return m_normal;
  }

  vec3 CollisionPoint::getContactPts(u32 index) const
  {
    ErrorIf(index < 0 || index >= m_numPts, "Out of range");
    return m_points[index];
  }

  u32 CollisionPoint::getNumPts() const
  {
    return m_numPts;
  }

  f32 CollisionPoint::getPenDepth() const
  {
    return m_penDepth;
  }

  void CollisionPoint::Jacobian()
  {

  }

  void CollisionPoint::lower()
  {

  }

  void CollisionPoint::upper()
  {

  }

  void CollisionPoint::error()
  {

  }

}; // namespace Grimelins
