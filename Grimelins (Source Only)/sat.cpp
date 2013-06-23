/*! ====================================================================================================================
  @file sat.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Oct.21.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "sat.h" 
#include "collision.h"

namespace Grimelins
{
  u1 SAT::testCollision(const BoundingVolume* const boundVol1, const BoundingVolume* const boundVol2, Contact* &contactInfo)
  {
    ErrorIf(boundVol1 == boundVol2, "bounding volumes are the same object");

    // if the 2 bv's are OBB
    const OBB* const obb1 = DCAST(const OBB* const, boundVol1);
    const OBB* const obb2 = DCAST(const OBB* const, boundVol2);

    if(obb1 && obb2)
      return compareOBB(obb1,obb2, contactInfo);
    else
      return false;
  }

  u1 SAT::compareOBB(const OBB* const obb1, const OBB* const obb2, Contact* &contactInfo)
  {
    // collision info
    SAT_Info satInfo;
    satInfo.minPenDepth = 1000000000.0f; // ridiculously large number
    satInfo.parallel = false;

    // distance between the 2 obb's
    vec3 distance = obb2->getCenter() - obb1->getCenter();

    // Calculate the separating axis (L)
    //-----------------------------------------------------
    // L = Ax, if true L is a separating axis || to Ax
    // (there exists a separating plane with normal, Ax)
    //-----------------------------------------------------
    // for each axis
    for(int i = 0; i < 3; ++i)
    {
      // calculate the dot between Ax and Bx,By,Bz
      satInfo.obbDot[0][i] = obb1->getAxis(0).Dot(obb2->getAxis(i));
      // get the absolute of the dot
      satInfo.absObbDot[0][i] = fabs(satInfo.obbDot[0][i]);
      // if the abs is greater than the cutoff for cosine of angles between the axes
      if(satInfo.absObbDot[0][i] > satInfo.cutOff)
        satInfo.parallel = true; // then return that they are parallel
    }

    satInfo.obb1center[0] = obb1->getAxis(0).Dot(distance);
    satInfo.center = satInfo.obb1center[0];
    satInfo.radius1 = obb1->getExtent(0);
    satInfo.radius2 = obb2->getExtent(0) * satInfo.absObbDot[0][0] + obb2->getExtent(1) * satInfo.absObbDot[0][1] + obb2->getExtent(2) * satInfo.absObbDot[0][2];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;

    float penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(0), distance); // get the normal
    }

    //-----------------------------------------------------
    // Ay
    //-----------------------------------------------------
    // for each axis
    for(int i = 0; i < 3; ++i)
    {
      // calculate the dot
      satInfo.obbDot[1][i] = obb1->getAxis(1).Dot(obb2->getAxis(i));
      // get the absolute of the dot
      satInfo.absObbDot[1][i] = fabs(satInfo.obbDot[1][i]);
      // if the abs is greater than the cutoff for cosine of angles between the axes
      if(satInfo.absObbDot[1][i] > satInfo.cutOff)
        satInfo.parallel = true; // then return that they are parallel
    }

    satInfo.obb1center[1] = obb1->getAxis(1).Dot(distance);
    satInfo.center = satInfo.obb1center[1];
    satInfo.radius1 = obb1->getExtent(1);
    satInfo.radius2 = obb2->getExtent(0) * satInfo.absObbDot[1][0] + obb2->getExtent(1) * satInfo.absObbDot[1][1] + obb2->getExtent(2) * satInfo.absObbDot[1][2];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(1), distance); // get the normal
    }

    //-----------------------------------------------------
    // Az
    //-----------------------------------------------------
    // for each axis
    for(int i = 0; i < 3; ++i)
    {
      // calculate the dot
      satInfo.obbDot[2][i] = obb1->getAxis(2).Dot(obb2->getAxis(i));
      // get the absolute of the dot
      satInfo.absObbDot[2][i] = fabs(satInfo.obbDot[2][i]);
      // if the abs is greater than the cutoff for cosine of angles between the axes
      if(satInfo.absObbDot[2][i] > satInfo.cutOff)
        satInfo.parallel = true; // then return that they are parallel
    }

    satInfo.obb1center[2] = obb1->getAxis(2).Dot(distance);
    satInfo.center = satInfo.obb1center[2];
    satInfo.radius1 = obb1->getExtent(2);
    satInfo.radius2 = obb2->getExtent(0) * satInfo.absObbDot[2][0] + obb2->getExtent(1) * satInfo.absObbDot[2][1] + obb2->getExtent(2) * satInfo.absObbDot[2][2];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(2), distance); // get the normal
    }    
    
    //-----------------------------------------------------
    // Bx
    //-----------------------------------------------------
    satInfo.obb2center[0] = obb1->getAxis(0).Dot(distance);
    satInfo.center = satInfo.obb1center[0];
    satInfo.radius1 = obb1->getExtent(0) * satInfo.absObbDot[0][0] + obb1->getExtent(1) * satInfo.absObbDot[1][0] + obb1->getExtent(2) * satInfo.absObbDot[2][0];
    satInfo.radius2 = obb2->getExtent(0);
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb2->getAxis(0), distance); // get the normal
    }

    //-----------------------------------------------------
    // By
    //-----------------------------------------------------
    satInfo.obb2center[1] = obb1->getAxis(1).Dot(distance);
    satInfo.center = satInfo.obb1center[1];
    satInfo.radius1 = obb1->getExtent(0) * satInfo.absObbDot[0][1] + obb1->getExtent(1) * satInfo.absObbDot[1][1] + obb1->getExtent(2) * satInfo.absObbDot[2][1];
    satInfo.radius2 = obb2->getExtent(1);
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb2->getAxis(1), distance); // get the normal
    }

    //-----------------------------------------------------
    // Bz
    //-----------------------------------------------------
    satInfo.obb2center[2] = obb1->getAxis(2).Dot(distance);
    satInfo.center = satInfo.obb1center[2];
    satInfo.radius1 = obb1->getExtent(0) * satInfo.absObbDot[0][2] + obb1->getExtent(1) * satInfo.absObbDot[1][2] + obb1->getExtent(2) * satInfo.absObbDot[2][2];
    satInfo.radius2 = obb2->getExtent(2);
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb2->getAxis(2), distance); // get the normal
    }

    // check for parallel faces. If they exist, the OBBs must intersect
    if(satInfo.parallel)
    {
      contactInfo = new Contact(obb1,obb2,satInfo.normal.Normalize(),penDepth);
      return true;
    }

    //-----------------------------------------------------
    // Ax x Bx
    //-----------------------------------------------------
    satInfo.center = satInfo.obb1center[2] * satInfo.obbDot[1][0] - satInfo.obb1center[1] * satInfo.obbDot[2][0];
    satInfo.radius1 = obb1->getExtent(1) * satInfo.absObbDot[2][0] + obb1->getExtent(2) * satInfo.absObbDot[1][0];
    satInfo.radius2 = obb2->getExtent(1) * satInfo.absObbDot[0][2] + obb2->getExtent(2) * satInfo.absObbDot[0][1];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(0).Cross(obb2->getAxis(0)), distance); // get the normal
    }

    //-----------------------------------------------------
    // Ax x By
    //-----------------------------------------------------
    satInfo.center = satInfo.obb1center[2] * satInfo.obbDot[1][1] - satInfo.obb1center[1] * satInfo.obbDot[2][1];
    satInfo.radius1 = obb1->getExtent(1) * satInfo.absObbDot[2][1] + obb1->getExtent(2) * satInfo.absObbDot[1][1];
    satInfo.radius2 = obb2->getExtent(0) * satInfo.absObbDot[0][2] + obb2->getExtent(2) * satInfo.absObbDot[0][0];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(0).Cross(obb2->getAxis(1)), distance); // get the normal
    }

    //-----------------------------------------------------
    // Ax x Bz
    //-----------------------------------------------------
    satInfo.center = satInfo.obb1center[2] * satInfo.obbDot[1][1] - satInfo.obb1center[1] * satInfo.obbDot[2][2];
    satInfo.radius1 = obb1->getExtent(1) * satInfo.absObbDot[2][2] + obb1->getExtent(2) * satInfo.absObbDot[1][2];
    satInfo.radius2 = obb2->getExtent(0) * satInfo.absObbDot[0][1] + obb2->getExtent(1) * satInfo.absObbDot[0][0];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(0).Cross(obb2->getAxis(2)), distance); // get the normal
    }

    //-----------------------------------------------------
    // Ay x Bx
    //-----------------------------------------------------
    satInfo.center = satInfo.obb1center[0] * satInfo.obbDot[2][0] - satInfo.obb1center[2] * satInfo.obbDot[0][0];
    satInfo.radius1 = obb1->getExtent(0) * satInfo.absObbDot[2][0] + obb1->getExtent(2) * satInfo.absObbDot[0][0];
    satInfo.radius2 = obb2->getExtent(1) * satInfo.absObbDot[1][2] + obb2->getExtent(2) * satInfo.absObbDot[1][1];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(1).Cross(obb2->getAxis(0)), distance); // get the normal
    }

    //-----------------------------------------------------
    // Ay x By
    //-----------------------------------------------------
    satInfo.center = satInfo.obb1center[0] * satInfo.obbDot[2][1] - satInfo.obb1center[2] * satInfo.obbDot[0][1];
    satInfo.radius1 = obb1->getExtent(0) * satInfo.absObbDot[2][1] + obb1->getExtent(2) * satInfo.absObbDot[0][1];
    satInfo.radius2 = obb2->getExtent(0) * satInfo.absObbDot[1][2] + obb2->getExtent(2) * satInfo.absObbDot[1][0];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(1).Cross(obb2->getAxis(1)), distance); // get the normal
    }

    //-----------------------------------------------------
    // Ay x Bz
    //-----------------------------------------------------
    satInfo.center = satInfo.obb1center[0] * satInfo.obbDot[2][2] - satInfo.obb1center[2] * satInfo.obbDot[0][2];
    satInfo.radius1 = obb1->getExtent(0) * satInfo.absObbDot[2][2] + obb1->getExtent(2) * satInfo.absObbDot[0][2];
    satInfo.radius2 = obb2->getExtent(0) * satInfo.absObbDot[1][1] + obb2->getExtent(1) * satInfo.absObbDot[1][0];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(1).Cross(obb2->getAxis(2)), distance); // get the normal
    }

    //-----------------------------------------------------
    // Az x Bx
    //-----------------------------------------------------
    satInfo.center = satInfo.obb1center[1] * satInfo.obbDot[0][0] - satInfo.obb1center[0] * satInfo.obbDot[1][0];
    satInfo.radius1 = obb1->getExtent(0) * satInfo.absObbDot[1][0] + obb1->getExtent(1) * satInfo.absObbDot[0][0];
    satInfo.radius2 = obb2->getExtent(1) * satInfo.absObbDot[2][2] + obb2->getExtent(2) * satInfo.absObbDot[2][1];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(2).Cross(obb2->getAxis(0)), distance); // get the normal
    }

    //-----------------------------------------------------
    // Az x By
    //-----------------------------------------------------
    satInfo.center = satInfo.obb1center[1] * satInfo.obbDot[0][1] - satInfo.obb1center[0] * satInfo.obbDot[1][1];
    satInfo.radius1 = obb1->getExtent(0) * satInfo.absObbDot[1][1] + obb1->getExtent(1) * satInfo.absObbDot[0][1];
    satInfo.radius2 = obb2->getExtent(0) * satInfo.absObbDot[2][2] + obb2->getExtent(2) * satInfo.absObbDot[2][0];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(2).Cross(obb2->getAxis(1)), distance); // get the normal
    }

    //-----------------------------------------------------
    // Az x Bz
    //-----------------------------------------------------
    satInfo.center = satInfo.obb1center[1] * satInfo.obbDot[0][2] - satInfo.obb1center[0] * satInfo.obbDot[1][2];
    satInfo.radius1 = obb1->getExtent(0) * satInfo.absObbDot[1][2] + obb1->getExtent(1) * satInfo.absObbDot[0][2];
    satInfo.radius2 = obb2->getExtent(0) * satInfo.absObbDot[2][1] + obb2->getExtent(1) * satInfo.absObbDot[2][0];
    satInfo.min1 = -satInfo.radius1;
    satInfo.max1 = satInfo.radius1;
    satInfo.min2 = satInfo.center - satInfo.radius2;
    satInfo.max2 = satInfo.center + satInfo.radius2;
    penDepth = getPenDepth(&satInfo);
    // We have found a separation axis, no collision
    if(penDepth < 0.0f)
      return false;
    else if (penDepth < satInfo.minPenDepth) // overlap is a smaller penDepth
    {
      satInfo.minPenDepth = penDepth; // update the minimum
      satInfo.normal = getNormal(obb1->getAxis(2).Cross(obb2->getAxis(2)), distance); // get the normal
    }

    // get the contact info
    contactInfo = new Contact(obb1,obb2,satInfo.normal.Normalize(),penDepth);
    return true;
  }

  f32 minlength(f32 a, f32 b)
  {
    return !(b < a) ? a:b;
  }

  f32 maxlength(f32 a, f32 b)
  {
    return (a < b) ? b:a;
  }

  f32 SAT::getPenDepth(SAT_Info* data)
  {
    // length of intervals
    f32 len1 = data->max1 - data->min1;
    f32 len2 = data->max2 - data->min2;

    // total length of intervals
    f32 lenTotal = maxlength(data->max1,data->max2) - minlength(data->min1,data->min2);

    // current penetration depth
    return len1 + len2 - lenTotal;
  }

  vec3 SAT::getNormal(const vec3& axis, const vec3& distance)
  {
    if(distance.Dot(axis) >= 0.0f)
      return axis;
    else
      return -axis;
  }
}; // namespace Grimelins