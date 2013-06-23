/*! ====================================================================================================================
  @file aabb.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.27.2011

  @brief 
    Interface for AABB

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_AABB_H
#define VIRTUS_AABB_H
#include "boundingvolume.h"

namespace Grimelins
{
  //-------------------------------------------------
  // Class AABB
  // this class is for Axis Aligned Bounding Box.
  //-------------------------------------------------
  class AABB: public BoundingVolume
  {
  public:
    // constructors, destructor
    AABB(const vec3& center, f32 ext_x, f32 ext_y, f32 ext_z);
    virtual ~AABB();
    AABB(void); 
    AABB(const AABB&);

    //------------------------------------------
    // Setters & Getters
    //------------------------------------------
    vec3 getCenter() const { return m_center; }
    void setCenter(const vec3& center) { m_center = center; }
    vec3 getVertex(u32 index) const;
    f32 getExtent(u32 index) const;
    void setExtent(u32 index, f32 extent);
    f32 getMinValue(u32 axis) const;
    f32 getMaxValue(u32 axis) const;

    //------------------------------------------
    // Functions
    //------------------------------------------
    // Return true if the current AABB is overlapping rhs
    bool testCollision(const AABB& aabb) const;
    // update the orientation of the bounding box
    void update(const vec3& new_center, const Quaternion& rotation);

  protected:
    //------------------------------------------
    // Data
    //------------------------------------------
    vec3 m_center;           // Center of AABB
    f32 m_extent[3];         // Three extents size in x,y,z
    f32 m_origAABBExtent[3]; // Extents of the original AABB
  };
	
}; // namespace Grimelins

#endif // Grimelins_AABB_H