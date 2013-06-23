/*! ====================================================================================================================
  @file collision.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.27.2011

  @brief 
    Calculates and stores collision pairs, calls Broad Phase and Narrow Phase

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_COLLISION_H
#define VIRTUS_COLLISION_H
// #include "compbody.h"
// #include "physics.h"
// #include "obb.h"
// #include "sap.h"
// #include "sat.h"

namespace Grimelins
{
  class Body;
  class Physics;
  class OBB;
  class SAP;
  class SAT;

  // Holds information of contact pairs
  class Contact {
  public:
    const OBB* const m_obb1;       // object 1
    const OBB* const m_obb2;       // object2
    const vec3 m_normal;           // normal
    const f32  m_penetrationDepth; // penetration depth of collision
    // create a new contact
    Contact(const OBB* const obb1, const OBB* const obb2, const vec3& normal, f32 penetrationDepth);
  };


  class Collision
  {
  public:
    Collision(Physics* physics);  // constructor
    ~Collision(); // destructor

    u1 computeCollision();
    u32 GetContactSize();
  friend class Physics;

  private:
    Physics* m_physics_world; // pointer to our physics
    SAP*     m_BroadPhase;    // pointer to our broad-phase
    SAT*     m_NarrowPhase;   // pointer to our narrow-phase

    // Collision pairs and information about them
    typedef std::pair<const Body*, const Body* > CollisionPair;
    std::vector<CollisionPair> m_possibleCollPairs;
    std::vector<Contact*> m_contacts;

    // clean up the notation
    typedef std::vector<vec3> vecVectors;
    typedef std::vector<Body*> BodyList;

    //-------------------------------------------------
    // Collision functions
    //-------------------------------------------------
    void BroadPhase();    // Commence Broad Phase checking
    void NarrowPhase();   // Commence Narrow Phase checking
    void AllContacts();   // Compute all contacts in the contacts list
      
    //-------------------------------------------------
    // Helper functions
    //-------------------------------------------------
    // adds a contact to the physics world, for 2 colliding objects
    void getContact(const Contact* const contact_info);
    // get the intersection between 2 parallel segments
    void getParallelIntersection(const vecVectors& seg1, const vecVectors& seg2, vec3& resultA, vec3& resultB);
    // Returns points such that the 2 points given, are the closest point between 2 lines
    void closestPoints(const Vector3 &point1, const Vector3 &d1, const Vector3 &point2, const Vector3 & d2, f32* alpha, f32 * beta);
    // Calculates the proj of a set of 3D points onto a 3D plane
    vecVectors projPoints(const vecVectors& points, const vec3 &A, const vec3& normal);
    // Clips a 3d segment with a 3d rect
    vecVectors clipSegment(vecVectors& edge, const vecVectors& obbExtreme);
    // Uses Sutherland-Hodgman clipping algorithm to clip a polygon using a rect
    vecVectors clipPolygon(vecVectors& face, const vecVectors& obbExtreme);
    // move a set of points by a given vector
    vecVectors movePts(vecVectors& clippedFace, vec3& vec);
    // Calculates a point that is the nearest point to point that is inside the rectangle
    vec3 nearPtOnRect(const vec3& point, const vecVectors& obbExtreme);
    // Get the intersection point between a line and a plane
    vec3 getLineIntersect(const vec3& linePt, const vec3& lineVec, const vec3& planePt, const vec3& planeN);
    // get the distance between a point and a line
    f32 distancePtToLine(const vec3& point, const vec3& rect1, const vec3& rect2);
    // get the ortho proj of a point on a line
    vec3 getProjPtToLine(const vec3& point, const vec3& rect1, const vec3& rect2);
  };
}; // namespace Grimelins

#endif // VIRTUS_COLLISION_H