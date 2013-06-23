/*! ====================================================================================================================
  @file sap.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.30.2011

  @brief 
    Implementation of Sweep and Prune

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "sap.h"
#include "physics.h"

namespace Grimelins
{
    // initialize the static variable
    u16 SAP::m_sortAxis = 0;

    u1 SAP::compareAABB(const AABB* a, const AABB* b)
    {
      // compare the min values on a given axis between the 2 bodies
      return (a->getMinValue(m_sortAxis) < b->getMinValue(m_sortAxis));
    }

    void SAP::removeAABB(Bodies bodies)
    {
      // iterator and temp aabb
      cAABB::iterator iter;
      const AABB* aabb;

      // remove the AABB of the bodies that have been removed
      for(Bodies::iterator it = bodies.begin(); it != bodies.end(); ++it)
      {
        // get the body and return the aabb
        aabb = RCAST(const AABB*, (*it)->getAABB());
        // find that aabb in the sorted list
        iter = find(m_sortedAABB.begin(), m_sortedAABB.end(), aabb);
        // erase that aabb from the list
        m_sortedAABB.erase(iter);
      }
    }

    void SAP::addAABB(Bodies bodies)
    {
      const AABB* aabb;

      // loop through the bodies
      for(Bodies::iterator it = bodies.begin(); it != bodies.end(); ++it)
      {
        aabb = 0;
        // get the aabb of the current body
        aabb = DCAST(const AABB*, (*it)->getAABB());
        // put the aabb of that body on the list
        m_sortedAABB.push_back(aabb);
      }
    }

    void SAP::CheckCollisionPairs(Bodies addedBodies, Bodies removedBodies, BodyList& possibleCollisionPairs)
    {
      // iterators
      cAABB::iterator it;
      cAABB::iterator it2;
      // info on the collisions
      vec3 center;
      f64 c_coord[3];
      s32 i;
      const Body* body;
      u32 num_aabb = m_sortedAABB.size();
      m_sortedAABB.clear();

      /// NOT SURE WE NEED THIS
      // variables for finding the variance in aabbs
      f64 variance[3];
      f64 v[] = {0.0f,0.0f,0.0f};
      f64 vsquare[] = {0.0f,0.0f,0.0f};

      // remove the bodies to remove
      removeAABB(removedBodies);

      // add the bodies to add
      addAABB(addedBodies);
  
      // sort the set of aabb
      sort(m_sortedAABB.begin(), m_sortedAABB.end(), compareAABB);

      // Sweep the sorted set
      for(cAABB::iterator it = m_sortedAABB.begin(); it != m_sortedAABB.end(); ++it)
      {
        // if the collision of the aabb's body is disabled
        if (!(*it)->getBodyPointer()->IsCollisionEnabled())
          continue;

        // center of current AABB
        center = (*it)->getCenter();
        c_coord[0] = center.x;
        c_coord[1] = center.y;
        c_coord[2] = center.z;


        // update variance values
        for(i = 0; i < 3; i++)
        {
          v[i] += c_coord[i];
          vsquare[i] += c_coord[i] * c_coord[i];
        }

        // test collisions against all overlapping AABBs following the current
        for(it2 = it+1; it2 != m_sortedAABB.end(); ++it2)
        {
          // stop when the tested AABBs are beyond the end of the current AABB
          if((*it2)->getMinValue(m_sortAxis) > (*it)->getMaxValue(m_sortAxis)) break;

          body = (*it2)->getBodyPointer();

          // test if both AABBs overlap
          if(body->IsCollisionEnabled() && (*it)->testCollision(*(*it2)) && ((*it)->getBodyPointer() != (*it2)->getBodyPointer()))
          {
            // add the current pair of AABBs into the collision pairs set
            possibleCollisionPairs.push_back(std::make_pair((*it)->getBodyPointer(), (*it2)->getBodyPointer()));
          }
        }
      }
      
      // compute the variance of the distribution of the AABBs
      for(i = 0; i<3; ++i)
        variance[i] = vsquare[i] - v[i] * v[i] / num_aabb;

     // update the sorted axis according to the axis with the largest variance
      m_sortAxis = 0;
      if (variance[1] > variance[0]) m_sortAxis = 1;
      if (variance[2] > variance[m_sortAxis]) m_sortAxis = 2;
    }

}; // namespace Virtus