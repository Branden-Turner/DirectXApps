/*! ====================================================================================================================
  @file boundingvolume.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.26.2011

  @brief 
    Holds information on the body pointer and whether this is broad phase or narrow

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_BOUNDINGVOLUME_H
#define VIRTUS_BOUNDINGVOLUME_H
#include "compbody.h"

namespace Grimelins
{
  //---------------------------------------------------------------------
  // Class BoundingVolume
  // this class represents the bounding volume that contains a rigid body.
  // It will be inherited to each box, sphere, capsule to be used to compute 
  // collision detection
  //---------------------------------------------------------------------
  class BoundingVolume
  {
  public:
    // constructor, destructor
    BoundingVolume(): m_body(0) { }
    virtual ~BoundingVolume() { }

    // get and set for the body pointer
    Body* getBodyPointer() const { return m_body; }
    void setBodyPointer(Body* bodyPointer) { m_body = bodyPointer; }

    // updates the orientation of the aabb
    virtual void update(const vec3& new_center, const Quaternion& rotation) = 0;

  protected:
    // pointer to the body that the bounding volume contains
    Body* m_body;
  };
}; // namespace Virtus

#endif // VIRTUS_BOUNDINGVOLUME_H