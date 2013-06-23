/*! ====================================================================================================================
  @file sap.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.30.2011

  @brief 
    Interface of the Sweep and Prune algorithm

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_SAP_H
#define VIRTUS_SAP_H
#include "aabb.h"

namespace Grimelins
{
    class SAP
    {
    public:
      SAP() {}            // constructor
      virtual ~SAP() {}   // destructor

	  // simplify body vectors
      typedef std::vector<Body*> Bodies; 
      // simplify Collision pairs
      typedef std::vector<std::pair<const Body*, const Body*> > BodyList;
	  // simplify AABB vectors
      typedef std::vector<const AABB*> cAABB;
      // Check the collision pairs
      void CheckCollisionPairs(Bodies addedBodies, Bodies removedBodies,
        BodyList& possibleCollisionPairs);

    private:
      //---------------------------------------------------
      // Data
      //---------------------------------------------------
      // sorted set of AABB of the bodies on one of the axis
      cAABB m_sortedAABB;
      static u16 m_sortAxis; // current sorting axis(x=0, y=1, z=2)

      //---------------------------------------------------
      // Functions
      //---------------------------------------------------
      // method that compares 2 AABBs
      static u1 compareAABB(const AABB* a, const AABB* b);
      // Removes AABB representation of a given set of bodies from the sorted list
      void removeAABB(Bodies bodies);
      // add the AABB representation of a given set of bodies from the sorted list
      void addAABB(Bodies bodies);
    };
}; // namespace Virtus

#endif // VIRTUS_SAP_H