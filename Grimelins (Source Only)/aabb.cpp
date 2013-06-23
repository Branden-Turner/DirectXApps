/*! ====================================================================================================================
  @file aabb.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.27.2011

  @brief 
    Implementation for AABB

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "aabb.h"

namespace Grimelins
{
    AABB::AABB( const vec3& center, f32 ext_x, f32 ext_y, f32 ext_z )
    {
      // set up the initial data
      m_center = center;
      m_extent[0] = m_origAABBExtent[0] = ext_x;
      m_extent[1] = m_origAABBExtent[1] = ext_y;
      m_extent[2] = m_origAABBExtent[2] = ext_z;
    }

    AABB::~AABB()
    {

    }

    vec3 AABB::getVertex(u32 index) const
    {
      // check if index is valid
      if(index >= 0 && index < 8) 
      { //switch for all 8 cases
        switch(index){
          case 0: return m_center + vec3(m_extent[0], m_extent[1], -m_extent[2]);
          case 1: return m_center + vec3(m_extent[0], m_extent[1], m_extent[2]);
          case 2: return m_center + vec3(-m_extent[0], m_extent[1], m_extent[2]);
          case 3: return m_center + vec3(-m_extent[0], m_extent[1], -m_extent[2]);
          case 4: return m_center + vec3(m_extent[0], -m_extent[1], -m_extent[2]);
          case 5: return m_center + vec3(m_extent[0], -m_extent[1], m_extent[2]);
          case 6: return m_center + vec3(-m_extent[0], -m_extent[1], m_extent[2]);
          case 7: return m_center + vec3(-m_extent[0], -m_extent[1], -m_extent[2]);
        }
      }
      else{
        // The index value is not valid, we throw an exception
        ErrorIf(index < 0 || index >= 8, "Exception : The index value has to be between 0 and 8");
      }

      return vec3();
    }

    f32 AABB::getExtent(u32 index) const
    {
      // The index value is not valid, we throw an exception
      ErrorIf(index < 0 || index >= 3, "Exception : The index value has to be between 0 and 3");
      
      return m_extent[index];
    }

    void AABB::setExtent(u32 index, f32 extent)
    {
      // The index value is not valid, we throw an exception
      ErrorIf(index < 0 || index >= 3, "Exception : The index value has to be between 0 and 3");

      this->m_extent[index] = extent;
    }

    f32 AABB::getMinValue(u32 axis) const
    {
      // switch for the 3 axis
      switch(axis){
        case 0: return m_center.x - m_extent[0];
        case 1: return m_center.y - m_extent[1];
        case 2: return m_center.z - m_extent[2];
        default: // The index value is not valid, we throw an exception
          ErrorIf(axis < 0 || axis >= 3, "Exception : The index value has to be between 0 and 3");
      }
      return 0;
    }

    f32 AABB::getMaxValue(u32 axis) const
    {
      // switch for the 3 axis
      switch(axis){
        case 0: return m_center.x + m_extent[0];
        case 1: return m_center.y + m_extent[1];
        case 2: return m_center.z + m_extent[2];
        default: // The index value is not valid, we throw an exception
          ErrorIf(axis < 0 || axis >= 3, "Exception : The index value has to be between 0 and 3");
      }
      return 0;
    }

    // Returns true if the current AABB is overlapping the rhs, 2 overlap if 
    // they overlap in the 3 axis' at the same time
    bool AABB::testCollision(const AABB& aabb) const
    {
      vec3 center2 = aabb.getCenter();
      // test each axis
      if(std::abs(m_center.x - center2.x) > (m_extent[0] + aabb.getExtent(0)))
        return false;
      if(std::abs(m_center.y - center2.y) > (m_extent[1] + aabb.getExtent(1)))
        return false;
      if(std::abs(m_center.z - center2.z) > (m_extent[2] + aabb.getExtent(2)))
        return false;

      // collision!
      return true;
    }

    void AABB::update(const vec3& new_center, const Quaternion& rotation)
    {
      // update the center of AABB
      m_center = new_center;

      // Recompute the new extents size from the rotated AABB
      // rotation matrix
      Matrix4 rot_mat = rotation.matrix();
      // for all axis
      for (s32 i = 0; i < 3; ++i)
      {
        m_extent[i] = 0.0;
        // for each rotated axis of the orig AABB
        for(s32 j = 0; j < 3; ++j)
        {
          // add the size of the proj of the current rotated axis to the extent of current axis
          m_extent[i] += std::abs(rot_mat.m[i][j]) * m_origAABBExtent[j];
        }
      }
    }
}; // namespace Grimelins