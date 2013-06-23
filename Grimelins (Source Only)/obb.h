/*! ====================================================================================================================
  @file obb.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Oct.4.2011

  @brief 
    Calculates the OBB for the Narrow phase and also calculates an AABB for Broad phase

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_OBB_H
#define VIRTUS_OBB_H
#include "boundingvolume.h"

namespace Grimelins
{
    class AABB;

    //-------------------------------------------------
    // Class OBB
    // this class is for Oriented Bounding Box.
    //-------------------------------------------------
    class OBB : public BoundingVolume
    {
    protected:
      vec3 m_center;      // center point of obb
      vec3 m_oldAxis[3];  // array of vectors of the starting axis
      vec3 m_axis[3];     // array of vectors of the axis
      f32 m_extents[3];   // array of floats of length of the extents

    public:
      // constructor, destructor
      OBB(const vec3& center, const vec3& axis1, const vec3& axis2,const vec3& axis3, f32 ext1, f32 ext2, f32 ext3);
      virtual ~OBB();

      //--------------------------------------------------------------
      // Setters & Getters
      //--------------------------------------------------------------
      vec3 getCenter() const { return m_center; }
      void setCenter(const vec3& center) { this->m_center = center; }
      vec3 getAxis(u32 index) const;
      void setAxis(u32 index, const vec3& axis);
      vec3 getVertex(u32 index) const;
      std::vector<vec3> getFace(u32 index) const;
      f32 getExtent(u32 index) const;
      void setExtent(u32 index, f32 extent);

      //--------------------------------------------------------------
      // Functions
      //--------------------------------------------------------------
      // Return all the vertices that are projected extreme of the BV on the axis
      virtual std::vector<vec3> getExtVert(const vec3& axis) const;
      // Update the OBB to the new rigid body orientation
      virtual void update(const vec3& newCenter, const Quaternion& rotQuat);
      // Return the AABB for the broad phase
      virtual AABB* computeAABB() const;
     };
}; // namespace Virtus

#endif // VIRTUS_OBB_H