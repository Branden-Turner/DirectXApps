/*! ====================================================================================================================
  @file collision.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.30.2011

  @brief 
    Implementation file for class Collision

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "collision.h"
#include "physics.h"
#include "constraints.h"
#include "compbody.h"
#include "obb.h"
#include "sap.h"
#include "sat.h"

namespace Grimelins
{
  Collision::Collision(Physics* physics)
  {
    m_physics_world = physics;

    // create our SAP and SAT
    m_BroadPhase = new SAP();
    m_NarrowPhase = new SAT();
  }

  Collision::~Collision()
  {

  }

  static u1 CheckColTypes(Body* b1, Body* b2, CollisionType t1, CollisionType t2)
  {
    return ((b1->getColType() == t1 && b2->getColType() == t2) ||
            (b1->getColType() == t2 && b2->getColType() == t1));
  }

  u1 Collision::computeCollision()
  {
    //m_physics_world->removeAllContactConstraints

    //Detect what kinds of collisions happened last frame and react to those with messages.
    for(u32 i = 0; i < m_contacts.size(); ++i)
    {
      Contact* contact = m_contacts.at(i);
      Body* b1 = contact->m_obb1->getBodyPointer();
      Body* b2 = contact->m_obb2->getBodyPointer();

      ErrorIf(!b1->parent()->GetMyListenerHandle(), "empty handle supplied by contact pair.");
      ErrorIf(!b2->parent()->GetMyListenerHandle(), "empty handle supplied by contact pair.");

      if( CheckColTypes(b1, b2, Environment, Player1) )
      {
        gCore->Events.PostTo(b1->parent()->GetMyListenerHandle(), H("collide_env_p1"));
        gCore->Events.PostTo(b2->parent()->GetMyListenerHandle(), H("collide_env_p1"));

        if(b1->getColType() == Environment)
        {
          b1->addCollision(Player1);
          b2->addCollision(Environment);
        }
        else
        {
          b2->addCollision(Player1);
          b1->addCollision(Environment);
        }
      }
      else if( CheckColTypes(b1, b2, Player1, DamageSource) )
      {
        gCore->Events.PostTo(b1->parent()->GetMyListenerHandle(), H("collide_p1_dmg"));
        gCore->Events.PostTo(b2->parent()->GetMyListenerHandle(), H("collide_p1_dmg"));

        if(b1->getColType() == Player1)
        {
          b1->addCollision(DamageSource);
          b2->addCollision(Player1);
        }
        else
        {
          b2->addCollision(DamageSource);
          b1->addCollision(Player1);
        }
      }
      else if( CheckColTypes(b1, b2, Boss, DamageSource) )
      {
        gCore->Events.PostTo(b1->parent()->GetMyListenerHandle(), H("collide_boss_dmg"));
        gCore->Events.PostTo(b2->parent()->GetMyListenerHandle(), H("collide_boss_dmg"));

        if(b1->getColType() == Boss)
        {
          b1->addCollision(DamageSource);
          b2->addCollision(Boss);
        }
        else
        {
          b2->addCollision(DamageSource);
          b1->addCollision(Boss);
        }
      }
      else if( CheckColTypes(b1, b2, Boss, Player1) )
      {
        gCore->Events.PostTo(b1->parent()->GetMyListenerHandle(), H("collide_boss_p1"));
        gCore->Events.PostTo(b2->parent()->GetMyListenerHandle(), H("collide_boss_p1"));

        if(b1->getColType() == Boss)
        {
          b1->addCollision(Player1);
          b2->addCollision(Boss);
        }
        else
        {
          b2->addCollision(Player1);
          b1->addCollision(Boss);
        }
      }
      else if( CheckColTypes(b1, b2, Environment, DamageSource) )
      {
        gCore->Events.PostTo(b1->parent()->GetMyListenerHandle(), H("collide_env_dmg"));
        gCore->Events.PostTo(b2->parent()->GetMyListenerHandle(), H("collide_env_dmg"));

        if(b1->getColType() == Environment)
        {
          b1->addCollision(DamageSource);
          b2->addCollision(Environment);
        }
        else
        {
          b2->addCollision(DamageSource);
          b1->addCollision(Environment);
        }
      }


      b1->setCollisionDebug(false);
      b2->setCollisionDebug(false);
    }

    m_possibleCollPairs.clear();
    m_contacts.clear();

    //compute broad phase collision detection
    BroadPhase();

    // compute narrow phase collision detection
    NarrowPhase();

    // compute all the new contacts
    AllContacts();

    // return true if at least one contact has been found
    return(m_contacts.size() > 0);
  }

  u32 Collision::GetContactSize()
  {
    return m_contacts.size();
  }

  void Collision::BroadPhase()
  {
    // clear the set of possible colliding pairs of bodies
    m_possibleCollPairs.clear();

    // compute the set of possible collision pairs of bodies
    m_BroadPhase->CheckCollisionPairs(m_physics_world->getAddedBodies(), m_physics_world->getRemovedBodies(), m_possibleCollPairs);
  }
    
  void Collision::NarrowPhase()
  {
    // for each possible collision pair of bodies
    for(u32 i = 0; i < m_possibleCollPairs.size(); i++)
    {
      Contact* contact_info = 0;
      
      if(m_NarrowPhase->testCollision(m_possibleCollPairs.at(i).first->getOBB(), m_possibleCollPairs.at(i).second->getOBB(), contact_info))
      {
        // Error check, Is there info on the contact?
        ErrorIf(!contact_info, "There is no information on the contact Collision::NarrowPhase");
        //add the contact info to the current list of collision info
        m_contacts.push_back(contact_info);
      }
        
    }
  }

  void Collision::AllContacts()
  {
    // for each possible contact info
    for(u32 i = 0; i < m_contacts.size(); i++)
    {
      Contact* contact_info = m_contacts.at(i);
      // Error check, Is there info on the contact?
      ErrorIf(!contact_info, "There is no information on the contact Collision::AllContacts");
      // compute one or several new contacts and add them to the physics world
      getContact(contact_info);
    }
  }

  void Collision::getContact(const Contact* const contact_info)
  {
    // extract info form the contact struct
    const OBB* const obb1 = contact_info->m_obb1;
    const OBB* const obb2 = contact_info->m_obb2;
    vec3 normal = contact_info->m_normal;
    f32 penetrationDepth = contact_info->m_penetrationDepth;
    // get the extreme points
    std::vector<vec3> obb1ExPts = obb1->getExtVert(normal);
    std::vector<vec3> obb2ExPts = obb2->getExtVert(-normal);
    // number of points
    u32 num_Vert_Obb1 = obb1ExPts.size();
    u32 num_Vert_Obb2 = obb2ExPts.size();
    // Make sure we have correct info
    ErrorIf(num_Vert_Obb1!=1 && num_Vert_Obb1!=2 && num_Vert_Obb1!=4, "number of vertices for oob1 is not 1,2 or 4");
    ErrorIf(num_Vert_Obb2!=1 && num_Vert_Obb2!=2 && num_Vert_Obb2!=4, "number of vertices for oob2 is not 1,2,or 4");
    ErrorIf(!(Grimelins::approxEqual(normal.Length(),1.0f)));

    // if it's a vertex contact
    if(num_Vert_Obb1 == 1)
    {
      //create a new collision point and add it
      m_physics_world->addConstraint(new CollisionPoint(obb1->getBodyPointer(), obb2->getBodyPointer(),normal, penetrationDepth, obb1ExPts));
    }
    // if it's a vertex contact
    else if(num_Vert_Obb2 == 1)
    {
      m_physics_world->addConstraint(new CollisionPoint(obb1->getBodyPointer(),obb2->getBodyPointer(),normal,penetrationDepth, obb2ExPts));
    }
    // if it's edge-edge contact
    else if(num_Vert_Obb1 == 2 && num_Vert_Obb2 == 2)
    {
      // compute the two vectors of the segment lines
      vec3 d1 = obb1ExPts[1] - obb1ExPts[0];
      vec3 d2 = obb2ExPts[1] - obb2ExPts[0];

      f32 alpha, beta;
      std::vector<vec3> contactSet;

      // if they are parallel
      if(d1.isParallelWith(d2))
      {
        vec3 contactA;
        vec3 contactB;

        // compute the intersection between 2 edges
        getParallelIntersection(obb1ExPts, obb2ExPts, contactA, contactB);

        // add the 2 contact points in the contact set
        contactSet.push_back(contactA);
        contactSet.push_back(contactB);
      }
      // if the 2 edges are not parallel
      else
      {
        // compute the closet two points between the 2 segments
        closestPoints(obb1ExPts[0], d1, obb2ExPts[0], d2, &alpha, &beta);
        vec3 pointA = obb1ExPts[0] + d1 * alpha;
        vec3 pointB = obb2ExPts[0] + d2 * beta;

        // Compute the contact point as halfway between the 2 closest
        vec3 contactPoint = (pointA + pointB) * 0.5f;

        // add the contact point into the contact set
        contactSet.push_back(contactPoint);
      }

      // create a new contact and add it to the world
      m_physics_world->addConstraint(new CollisionPoint(obb1->getBodyPointer(), obb2->getBodyPointer(), normal, penetrationDepth, contactSet));
    }
    // if it's edge-face contact
    else if(num_Vert_Obb1 == 2 && num_Vert_Obb2 == 4)
    {
      // compute the projection of the edge of OBB1 onto the same plane
      vecVectors edge = projPoints(obb1ExPts, obb2ExPts[0], normal);
        
      // clip the edge of obb1 using the face of obb2
      std::vector<vec3> clipEdge = clipSegment(edge, obb2ExPts);

      // nearest edge is not within the rectangle
      if(clipEdge.size() != 2)
      {
        edge.clear();
        edge.push_back(nearPtOnRect(edge[0], obb2ExPts));
        edge.push_back(nearPtOnRect(edge[1], obb2ExPts));
        clipEdge = clipSegment(edge, obb2ExPts);
      }
        
      // move the clipped edge halfway between the edge of obb1 and face of obb2
      clipEdge = movePts(clipEdge, (-normal * 2.0f)/penetrationDepth);

      ErrorIf(clipEdge.size() != 2, "Clipped edges is not 2, OBB::computeContact");

      // create a new contact and add it to the physics world
      m_physics_world->addConstraint(new CollisionPoint(obb1->getBodyPointer(), obb2->getBodyPointer(), normal, penetrationDepth, clipEdge));
    }
    // if it's an edge-face contact
    else if(num_Vert_Obb1 == 4 && num_Vert_Obb2 == 2)
    {
      // compute the projection of the edge of OBB2 onto the same plane
      vecVectors edge = projPoints(obb2ExPts, obb1ExPts[0], normal);

      // clip the edge of OBB2 using the face OBB1
      std::vector<vec3> clipEdge = clipSegment(edge, obb1ExPts);

      // nearest edge is not within the rectangle
      if(clipEdge.size() != 2)
      {
        edge.clear();
        edge.push_back(nearPtOnRect(edge[0], obb1ExPts));
        edge.push_back(nearPtOnRect(edge[1], obb1ExPts));
        clipEdge = clipSegment(edge, obb1ExPts);
      }

      // move the clipped edge halfway between the face of obb1 and the edge of obb2
      
      clipEdge = movePts(clipEdge, (normal * penetrationDepth) / 2.0f);

      ErrorIf(clipEdge.size() != 2, "Clipped edges is not 2, OBB::computeContacts");
        
      // create new contact and add it to the physics world
      m_physics_world->addConstraint(new CollisionPoint(obb1->getBodyPointer(),obb2->getBodyPointer(),normal,penetrationDepth,clipEdge));
    }
    // if it's a face-face contact
    else
    {
//         // compute the projection of the face vertices of obb2 onto oob1
//       vecVectors faceObb2 = projPoints(obb2ExPts,obb1ExPts[0], normal);
// 
//       // clip the face of Obb2 using Obb1
//       std::vector<vec3> clipFace = clipPolygon(faceObb2, obb1ExPts);
// 
//       // move the clipped face
//       clipFace = movePts(clipFace, (normal * penetrationDepth) / 2.0f);
//       ErrorIf(clipFace.size() < 3, "Clipped Face has less than 3 vertices OBB::computeContacts");
//       //TODO: Fitz had an assert stop here. I saved some of the local variable values for you, hope it helps find the error.
//       //clipFace was a zero vector(no elements - this triggered the assert)
//       //normal was 1,-0,-0
//       //penetrationDepth was 25
//       //faceObb2 was [0]12.5, 30.0, -110.0   [1]12.5, 30.0, 70.0   [2]12.5, 10.0, 70.0   [3]12.5, 10.0, -110.0
//       //obb1ExPts was [0]12.5, -12.5, -12.5    [1]12.5, -12.5, 12.5   [2]12.5, 12.5, 12.5    [3]12.5, 12.5, -12.5
// 
//       // create a new contact and add it to the physics world
//       m_physics_world->addConstraint(new CollisionPoint(obb1->getBodyPointer(), obb2->getBodyPointer(),normal,penetrationDepth,clipFace));
    }
    // Debug Draw
    Body* body1 = contact_info->m_obb1->getBodyPointer();
    body1->setCollisionDebug();
    Body* body2 = contact_info->m_obb2->getBodyPointer();
    body2->setCollisionDebug();
  }

  void Collision::getParallelIntersection(const vecVectors& seg1, const vecVectors& seg2, vec3& resultA, vec3& resultB)
  {
    //compute the segment vectors
    vec3 d1 = seg1[1] - seg1[0];
    vec3 d2 = seg2[1] - seg2[0];

    // the 2 seg's should be parallel
    ErrorIf(!d1.isParallelWith(d2), "Segments are not parallel");

    // compute the projection of the 2 pts 
    f32 projseg2A = d1.Normalize().Dot(seg2[0] - seg1[0]);
    f32 projseg2B = d1.Normalize().Dot(seg2[1] - seg1[0]);

    // the projections should intersect
    ErrorIf(projseg2A < 0.0f && projseg2B < 0.0f, "Projection intervals not intersecting");
    ErrorIf(projseg2A < d1.Length() && projseg2B > d1.Length(), "Projections intervals not intersecting");

    // compute the vector from a point on the line  to the orthogonal of the other line
    vec3 point = (seg1[0] + d1 * ((seg2[0]-seg1[0]).Dot(d1) / (d1.Dot(d1)))); 
    vec3 v = seg2[0] - point;

    // return the segment intersection
    if(projseg2A >= 0 && projseg2A <= d1.Length() && projseg2B >= d1.Length())
    {
      //move the contact pts halfway between 2 segments
      resultA = seg2[0] - v * 0.5f;
      resultB = seg1[1] + v * 0.5f;
    }
    else if(projseg2A <= 0 && projseg2B >= 0 && projseg2B <= d1.Length())
    {
      //move the contact pts halfway between 2 segments
      resultA = seg1[0] + v * 0.5f;
      resultB = seg2[1] - v * 0.5f;
    }
    else if(projseg2A <= 0 && projseg2B >= d1.Length())
    {
      //move the contact pts halfway between 2 segments
      resultA = seg1[0] + v * 0.5f;
      resultB = seg1[1] + v * 0.5f;
    }
    else if(projseg2A <= d1.Length() && projseg2B <= d1.Length())
    {
      //move the contact pts halfway between 2 segments
      resultA = seg2[0] - v * 0.5f;
      resultB = seg2[1] - v * 0.5f;
    }
  }

  void Collision::closestPoints(const Vector3 &point1, const Vector3 &d1, const Vector3 &point2, const Vector3 & d2, f32* alpha, f32 * beta)
  {
    vec3 r = point1 - point2;
    f32 a = d1.Dot(d1);
    f32 b = d1.Dot(d2);
    f32 c = d1.Dot(r);
    f32 e = d2.Dot(d2);
    f32 f = d2.Dot(r);
    f32 d = a*e - b*b;

    // the 2 lines must not be parallel
    ErrorIf(Grimelins::approxEqual(d, 0.0f), "the 2 segments cannot be parallel");

    // compute the values
    *alpha = (b*f - c*e) / d;
    *beta = (a*f - b*c) / d;
  }

  Collision::vecVectors Collision::projPoints(const vecVectors& points, const vec3 &A, const vec3& normal)
  {
    ErrorIf(normal.Length() == 0.0f, "the length of the normal is 0, Collision::projPoints");

    vecVectors proj_points;
    vec3 n = normal;
    n.Normalize();

    // for each point of the set
    for(u32 i = 0; i < points.size(); ++i)
    {
      // compute the projection of the point onto the plane
      proj_points.push_back(points[i] - (n * (points[i] - A).Dot(n)));
    }// should this be a +?

    // return the projected set points
    return proj_points;
  }

  Collision::vecVectors Collision::clipSegment(vecVectors& edge, const vecVectors& obbExtreme)
  {
    ErrorIf(edge.size() != 2, "Edge doesn't have 2 pts, Collision::clipSegment");
    ErrorIf(obbExtreme.size() != 4, "Rectangle doesn't have 4 pts, Collision::clipSegment");

    f32 const epsilon = .01f;
    vecVectors input = edge;
    vecVectors output;

    for(u32 i = 0; i < 4; ++i)
    {
      output.clear();

      // current clipped segment
      vec3 seg1 = input[0];
      vec3 seg2 = input[1];

      // edge of the clipped rectangle
      vec3 rect1 = obbExtreme[i];
      vec3 rect2 = obbExtreme[(i+1)%4];
      vec3 planeN = obbExtreme[(i+2)%4] - obbExtreme[(i+1)%4];

      // if the seg2 is inside the clipped plane
      if(planeN.Dot(seg2 - rect1) >= -epsilon)
      {
        // if the point seg1 if inside the clipped plane
        if(planeN.Dot(seg1 - rect1) >= -epsilon)
        {
          output.push_back(seg2);
          output.push_back(seg1);
        }
        else // seg2 is inside and seg1 is outside
        {
          // get the intersection between seg1seg2 and the plane
          vec3 intersect = getLineIntersect(seg1, seg2-seg1, rect1, rect2-rect1);
          output.push_back(seg2);
          output.push_back(intersect);
        }
      }
      // seg2 is outside and seg1 is inside
      else if(planeN.Dot(seg1-rect1) > -epsilon)
      {
        // get the intersection between seg1seg2 and the plane
        vec3 intersect = getLineIntersect(seg1, seg2-seg1, rect1, rect2-rect1);
        output.push_back(seg1);
        output.push_back(intersect);
      }

      input = output;
    }
      
    return output;
  }

  Collision::vecVectors Collision::clipPolygon(vecVectors& face, const vecVectors& obbExtreme)
  {
    ErrorIf(obbExtreme.size() != 4, "Rectangle doesn't have 4 pts, Collision::clipPolygon");

    f32 const epsilon = .1f;

    vecVectors input = face;
    vecVectors output;

    // each edge
    for(u32 i = 0; i < 4; ++i)
    {
      output.clear();
      if(input.size() == 0)
        break;

      // edges of clip plane
      vec3 planeN = obbExtreme[(i+2) % 4] - obbExtreme[(i+1) % 4];
      vec3 rect1 = obbExtreme[i];
      vec3 rect2 = obbExtreme[(i+1) % 4];
      vec3 poly1 = input[0];
      //TODO: Ask jodi how input can possibly be zero length!
      // each vertex
      for(u32 j = 0; j < input.size(); ++j)
      {
        vec3 poly2 = input[(j+1) % input.size()];
        // if the poly2 is inside the plane
        f32 debug = planeN.Dot(poly2 - rect1);
        if(planeN.Dot(poly2 - rect1) >= -epsilon)
        {
          // if poly1 is also inside
          if(planeN.Dot(poly1 - rect1) >= -epsilon)
            output.push_back(poly2); 
          else // if poly1 is outside
          {
            vec3 intersect = getLineIntersect(poly1, poly2-poly1, rect1, rect2-rect1);
            output.push_back(intersect);
            output.push_back(poly2);
          }
        }
        else if(planeN.Dot(poly1-rect1) > 0.0f)
        {
          // get the intersection between poly1poly2 and the plane
          vec3 intersect = getLineIntersect(poly1, poly2-poly1, rect1, rect2-rect1);
          output.push_back(intersect);
        }
        poly1 = poly2;
      }
//       ErrorIf(output.empty(), "There was a face-face collission... which of course is broken.");
      input = output;
    }
    return output;
  }

  Collision::vecVectors Collision::movePts(vecVectors& clippedFace, vec3& vec)
  {
    vecVectors result;

    // for each point on the face
    for(u32 i = 0; i < clippedFace.size(); ++i)
      result.push_back(clippedFace[i] + vec); // move the point

    return result;
  }

  vec3 Collision::nearPtOnRect(const vec3& point, const vecVectors& obbExtreme)
  {
    ErrorIf(obbExtreme.size() != 4, "Rectangle does not contain 4 pts, Collision::nearPtOnRect");
      
    vec3 result;

    f32 ptsToSeg1 = distancePtToLine(point, obbExtreme[0], obbExtreme[1] - obbExtreme[0]);
    f32 ptsToSeg2 = distancePtToLine(point, obbExtreme[1], obbExtreme[2] - obbExtreme[1]);
    f32 ptsToSeg3 = distancePtToLine(point, obbExtreme[2], obbExtreme[3] - obbExtreme[2]);
    f32 ptsToSeg4 = distancePtToLine(point, obbExtreme[3], obbExtreme[0] - obbExtreme[3]);
    f32 seg1ToSeg3 = distancePtToLine(obbExtreme[0], obbExtreme[3], obbExtreme[3] - obbExtreme[2]);
    f32 seg2ToSeg4 = distancePtToLine(obbExtreme[1], obbExtreme[3], obbExtreme[0] - obbExtreme[3]);

    // check if point is between the lines of the first pair
    if(ptsToSeg1 <= seg1ToSeg3 && ptsToSeg3 <= seg1ToSeg3)
    {
      // which one is nearest?
      // segment 2
      if(ptsToSeg2 <= ptsToSeg4)
      {
        // get the proj of the point onto segment 2
        result = getProjPtToLine(point, obbExtreme[1], obbExtreme[2] - obbExtreme[1]);
      }
      else // segment 4
      {
        // get the proj of the point onto segment 4
        result = getProjPtToLine(point, obbExtreme[3], obbExtreme[0] - obbExtreme[3]);
      }
    }
    // check if point is between the lines of the second pair
    else if(ptsToSeg2 <= seg2ToSeg4 && ptsToSeg4 <= seg2ToSeg4)
    {
      // which one is nearest?
      // segment 1
      if(ptsToSeg1 <= ptsToSeg3)
      {
        // get the proj of the point onto segment 1
        result = getProjPtToLine(point, obbExtreme[0], obbExtreme[1] - obbExtreme[0]);
      }
      else // segment 3
      {
        // get the proj of the oint onto segment 3
        result = getProjPtToLine(point, obbExtreme[2], obbExtreme[3] - obbExtreme[2]);
      }
    }
    // check corners
    else if(ptsToSeg4 <= ptsToSeg2)
    {
      // the point is in the corner of obbExtreme[0]
      if(ptsToSeg1 <= ptsToSeg3)
        return obbExtreme[0];
      else // the point is in the corner of obbExtreme[3]
        return obbExtreme[3];
    }
    else // check corners
    {
      // the point is in the corner of obbExtreme[1]
      if(ptsToSeg1 <= ptsToSeg3)
        return obbExtreme[1];
      else // the point is in the corner of obbExtreme[2]
        return obbExtreme[2];
    }

    return result;
  }

  vec3 Collision::getLineIntersect(const vec3& linePt, const vec3& lineVec, const vec3& planePt, const vec3& planeN)
  {
//     ErrorIf(Grimelins::approxEqual(lineVec.Dot(planeN), 0.0), "Line vector is orthogonal to the plane normal, Collision::getLineIntersect");
    //TODO: Comment this error condition back in when face-face collision is working (Fitz commented it out)

    f32 d = planeN.Dot(planePt);
    f32 t = (d - planeN.Dot(linePt)) / planeN.Dot(lineVec);

    return linePt + lineVec * t;
  }

  f32 Collision::distancePtToLine(const vec3& point, const vec3& rect1, const vec3& rect2)
  {
    ErrorIf(rect2.Length() == 0, "Length of vector is zero, Collision::distancePtToLine");
    return((point - rect1).Cross(rect2).Length() / (rect2.Length()));
  }

  vec3 Collision::getProjPtToLine(const vec3& point, const vec3& rect1, const vec3& rect2)
  {
    return (rect1 + rect2 * ((point-rect1).Dot(rect2) / (rect2.Dot(rect2))));
  }

  Contact::Contact( const OBB* const obb1, const OBB* const obb2, const vec3& normal, f32 penetrationDepth )
    : m_obb1(obb1), m_obb2(obb2), m_normal(normal), m_penetrationDepth(penetrationDepth)
  {
  }

}; // namespace Virtus