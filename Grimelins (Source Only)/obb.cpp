/*! ====================================================================================================================
  @file obb.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Oct.4.2011

  @brief 
    Implementation for the class OBB

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "obb.h"
#include "aabb.h"

namespace Grimelins
{
  OBB::OBB(const vec3& center, const vec3& axis1, const vec3& axis2,const vec3& axis3, f32 ext1, f32 ext2, f32 ext3)
  {
    m_center = center;
    m_axis[0] = m_oldAxis[0] = axis1.Normalized();
    m_axis[1] = m_oldAxis[1] = axis2.Normalized();
    m_axis[2] = m_oldAxis[2] = axis3.Normalized();
    m_extents[0] = ext1;
    m_extents[1] = ext2;
    m_extents[2] = ext3;
  }

  OBB::~OBB()
  {

  }

  vec3 OBB::getAxis(u32 index) const
  {
    // The index value is not valid, we throw an exception
    ErrorIf(index < 0 || index >= 3, "Exception : The index value has to be between 0 and 3");

    return m_axis[index];
  }

  void OBB::setAxis(u32 index, const vec3& axis)
  {
    // The index value is not valid, we throw an exception
    ErrorIf(index < 0 || index >= 3, "Exception : The index value has to be between 0 and 3");
      m_axis[index] = axis;
  }

  vec3 OBB::getVertex(u32 index) const
  {
    vec3 vertex;
    // check if index is valid
    if(index >= 0 && index < 8)
    { 
      //switch for all 8 cases
      switch(index){
        case 0: vertex = m_center + (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]) - (m_axis[2]*m_extents[2]); break;
        case 1: vertex = m_center + (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]) + (m_axis[2]*m_extents[2]); break;
        case 2: vertex = m_center - (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]) + (m_axis[2]*m_extents[2]); break;
        case 3: vertex = m_center - (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]) - (m_axis[2]*m_extents[2]); break;
        case 4: vertex = m_center + (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]) - (m_axis[2]*m_extents[2]); break;
        case 5: vertex = m_center + (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]) + (m_axis[2]*m_extents[2]); break;
        case 6: vertex = m_center - (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]) + (m_axis[2]*m_extents[2]); break;
        case 7: vertex = m_center - (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]) - (m_axis[2]*m_extents[2]); break;
      }
    }
    else{
      // The index value is not valid, we throw an exception
      ErrorIf(index < 0 || index >= 8, "Exception : The index value has to be between 0 and 8");
    }
    return vertex;
  }

  std::vector<vec3> OBB::getFace(u32 index) const
  {
    // The index value is not valid, we throw an exception
    ErrorIf(index < 0 || index >= 6, "Exception : The index value has to be between 0 and 6");
      
    std::vector<vec3> verts;
    // which face do you want?
    switch(index){
      case 0: verts.push_back(m_center + (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]) - (m_axis[2]*m_extents[2]));
              verts.push_back(m_center + (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]) + (m_axis[2]*m_extents[2]));
              verts.push_back(m_center + (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]) + (m_axis[2]*m_extents[2]));
              verts.push_back(m_center + (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]) - (m_axis[2]*m_extents[2]));
              break;
      case 1: verts.push_back(m_center - (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]) - (m_axis[2]*m_extents[2]));
              verts.push_back(m_center - (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]) + (m_axis[2]*m_extents[2]));
              verts.push_back(m_center - (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]) + (m_axis[2]*m_extents[2]));
              verts.push_back(m_center - (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]) - (m_axis[2]*m_extents[2]));
              break;
      case 2: verts.push_back(m_center + (m_axis[1]*m_extents[1]) + (m_axis[0]*m_extents[0]) - (m_axis[2]*m_extents[2]));
              verts.push_back(m_center + (m_axis[1]*m_extents[1]) + (m_axis[0]*m_extents[0]) + (m_axis[2]*m_extents[2]));
              verts.push_back(m_center + (m_axis[1]*m_extents[1]) - (m_axis[0]*m_extents[0]) + (m_axis[2]*m_extents[2]));
              verts.push_back(m_center + (m_axis[1]*m_extents[1]) - (m_axis[0]*m_extents[0]) - (m_axis[2]*m_extents[2]));
              break;
      case 3: verts.push_back(m_center - (m_axis[1]*m_extents[1]) + (m_axis[0]*m_extents[0]) - (m_axis[2]*m_extents[2]));
              verts.push_back(m_center - (m_axis[1]*m_extents[1]) + (m_axis[0]*m_extents[0]) + (m_axis[2]*m_extents[2]));
              verts.push_back(m_center - (m_axis[1]*m_extents[1]) - (m_axis[0]*m_extents[0]) + (m_axis[2]*m_extents[2]));
              verts.push_back(m_center - (m_axis[1]*m_extents[1]) - (m_axis[0]*m_extents[0]) - (m_axis[2]*m_extents[2]));
              break;
      case 4: verts.push_back(m_center + (m_axis[2]*m_extents[2]) + (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]));
              verts.push_back(m_center + (m_axis[2]*m_extents[2]) + (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]));
              verts.push_back(m_center + (m_axis[2]*m_extents[2]) - (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]));
              verts.push_back(m_center + (m_axis[2]*m_extents[2]) - (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]));
              break;
      case 5: verts.push_back(m_center - (m_axis[2]*m_extents[2]) + (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]));
              verts.push_back(m_center - (m_axis[2]*m_extents[2]) + (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]));
              verts.push_back(m_center - (m_axis[2]*m_extents[2]) - (m_axis[0]*m_extents[0]) + (m_axis[1]*m_extents[1]));
              verts.push_back(m_center - (m_axis[2]*m_extents[2]) - (m_axis[0]*m_extents[0]) - (m_axis[1]*m_extents[1]));
              break;    
    }
    ErrorIf(verts.size() != 4, "the number of vertices is not 4");
    return verts;
  }

  f32 OBB::getExtent(u32 index) const
  {
    ErrorIf(index < 0 || index >= 3, "The index value is out of range, OBB::getExtent");

    return m_extents[index];
  }

  void OBB::setExtent(u32 index, f32 extent)
  {
    ErrorIf(index < 0 || index >= 3, "The index value is out of range, OBB::setExtent");

    this->m_extents[index] = extent;
  }

  std::vector<vec3> OBB::getExtVert(const vec3& dirAxis) const
  {
    ErrorIf(dirAxis.Length() == 0, "The length of the directional axis is 0, OBB::getExtVert");

    std::vector<vec3> extremeVertices;

    // Check if the given axis is parallel to an axis on the OBB
    if(m_axis[0].isParallelWith(dirAxis))
    {
      // if both axis are in the same direction
      if(m_axis[0].Dot(dirAxis) >= 0)
        extremeVertices = getFace(0);
      else
        extremeVertices = getFace(1);
    }
    else if(m_axis[1].isParallelWith(dirAxis))
    {
      // if both axis are in the same direction
      if(m_axis[1].Dot(dirAxis) >= 0)
        extremeVertices = getFace(2);
      else
        extremeVertices = getFace(3);
    }
    else if(m_axis[2].isParallelWith(dirAxis))
    {
      // if both axis are in the same direction
      if(m_axis[2].Dot(dirAxis) >= 0)
        extremeVertices = getFace(4);
      else
        extremeVertices = getFace(5);
    }
    else // the extreme is made of a unique vertex or edge
    {
      f32 maxProjLength = 0.0f;

      // for each vertex of the OBB
      for(u32 i = 0; i < 8; ++i)
      {
        vec3 vert = getVertex(i);

        // get the projection of the current
        f32 projLen = dirAxis.Dot(vert - m_center) / dirAxis.Length();
          
        // Epsilon = .00001f
        if(projLen > maxProjLength + .00001f)
        {
          // reset the max length
          maxProjLength = projLen;
          // clear the vectices
          extremeVertices.clear();
          // push in this vert
          extremeVertices.push_back(vert);
        }
        // else they are within an episilon of each other
        else if(Grimelins::approxEqual(projLen,maxProjLength))
          extremeVertices.push_back(vert); // don't clear and add this vert
      }
      ErrorIf(extremeVertices.size() != 1 && extremeVertices.size() != 2, "The size of Extreme Vertices is not 1 or 2, OBB::getExtVert");
    }
    ErrorIf(extremeVertices.size() != 1 && extremeVertices.size() != 2 && extremeVertices.size() != 4, "An extreme should be a vertex, edge, or face, OBB::getExtVert");
      
    return extremeVertices;
  }

  void OBB::update(const vec3& newCenter, const Quaternion& rotQuat)
  {
    // update the center of the OBB
    m_center = newCenter;

    // update the 3 axis of the OBB by rotating and normalize them
    m_axis[0] = m_oldAxis[0].RotateWithQuat(rotQuat).Normalize();
    m_axis[1] = m_oldAxis[1].RotateWithQuat(rotQuat).Normalize();
    m_axis[2] = m_oldAxis[2].RotateWithQuat(rotQuat).Normalize();
  }

  AABB* OBB::computeAABB() const
  {
    f32 maxLen[] = {0.0f,0.0f,0.0f};
    vec3 vert;
    f32 len;

    // for each vertex of obb
    for(s32 i = 0; i < 8; i++)
    {
      vert = getVertex(i) - m_center;
      for(s32 j = 0; j < 3; j++)
      {
        len = std::abs(vert.v[j]);
        if(len > maxLen[j])
          maxLen[j] = len;
      }
    }

    // create and return the aabb
    return new AABB(m_center, maxLen[0], maxLen[1], maxLen[2]);
  }
}; // namespace Virtus