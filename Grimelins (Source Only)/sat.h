/*! ====================================================================================================================
  @file sat.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Oct.21.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_SAT_H
#define VIRTUS_SAT_H
#include "obb.h"

namespace Grimelins
{
  class Contact;
  
  class SAT
  {
  public:
    SAT() {}          // constructor
    virtual ~SAT() {} // destructor

    // test the collision pair
    bool testCollision(const BoundingVolume* const boundVol1, const BoundingVolume* const boundVol2, Contact* &contactInfo);

  private:
    // this is a struct to hold all of the SAT information we need
    struct SAT_Info
    {
      // variables for calculating
      f32 center, radius1, radius2, min1, min2, max1, max2;
      vec3 normal;

      // arrays of dot product info
      f32 obbDot[3][3];
      f32 absObbDot[3][3];
      f32 obb1center[3];
      f32 obb2center[3];

      // collision info
      f32 minPenDepth;
      f32 cutOff;
      bool parallel;

      SAT_Info():cutOff(0.99f) {}
    };
    // Calculates a contact if the 2 obbs are colliding, returns true
    static u1 compareOBB(const OBB* const obb1, const OBB* const obb2, Contact* &contactInfo);
    // Gets the penetration depth
    static float getPenDepth(SAT_Info* data);
    // Gets the contact's normal
    static vec3 getNormal(const vec3& axis, const vec3& distance); 
  };
}; // namespace Grimelins

#endif // Grimelins_SAT_H