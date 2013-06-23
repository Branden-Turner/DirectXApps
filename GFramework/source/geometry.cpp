#include "precompiled.h"

namespace cs350
{




  Triangle::Triangle( const vec3& vone, const vec3& vtwo, const vec3& vthree ) : p1(vone), 
                                                                                 p2(vtwo), 
                                                                                 p3(vthree)
  {
  }

  Triangle::Triangle( const Vertex_Pos_Clr_Nrm_Tex& _v1, const Vertex_Pos_Clr_Nrm_Tex& _v2, const Vertex_Pos_Clr_Nrm_Tex& _v3 )
    :vert1(_v1), vert2(_v2), vert3(_v3)
  {
    p1 = _v1.pos;
    p2 = _v2.pos;
    p3 = _v3.pos;
  }

  void GeometryLibrary::addTriToScene( const Triangle& tri )
  {
    staticSceneTris.push_back(tri);
  }

  Pt GeometryLibrary::projPtToLine( const Pt& p, const Line& l )
  {
    Pt pPrime;
    float t;

    t = (p - l.p).Dot(l.v / l.v.Dot(l.v));

    pPrime = l.p + l.v * t;

    return pPrime;
  }

  Pt GeometryLibrary::projPtToPlane( const Pt& p, const Plane& pl )
  {
    Pt pPrime;
    float t;

    //Figure out the distance from the point to the plane
    t = (p - pl.q).Dot(pl.n / pl.n.Dot(pl.n));

    //Subtract it from the point in the direction of the normal of the plane
    pPrime = p - pl.n * t;

    return pPrime;
  }

  float GeometryLibrary::distPts( const Pt& p1, const Pt& p2 )
  {
    return (p2 - p1).Mag();
  }

  float GeometryLibrary::distPtLs( const Pt& p, const LineSegment& ls )
  {
    //grab the vector that represents the line containing the segment
    Vc v = (ls.p2 - ls.p1);

    //project p to the line containing the segment
    float t = (p - ls.p1).Dot( v / v.Dot(v) );

    //clamp t
    clamp(t, 0.0f, 1.0f);

    //Find p' with new t value
    Pt pPrime = ls.p1 + v * t;

    //Get the distance between p and p'
    return distPts(p, pPrime);
  }


  float GeometryLibrary::distPtLine( const Pt& p, const Line& l )
  {
    //Project the point onto the line then find the distance between the two
    Pt pPrime = projPtToLine(p, l);
    return distPts(p, pPrime);
  }


  float GeometryLibrary::distPtRay( const Pt& p, const Ray& r )
  {
    //project p to the line containing the ray
    float t = (p - r.p).Dot( r.v / r.v.Dot(r.v) );

    //clamp t to 0
    t = max(t, 0.0f);

    //Find p' with new t value
    Pt pPrime = r.p + r.v * t;

    //Get the distance between p and p'
    return distPts(p, pPrime);
  }

  float GeometryLibrary::distPtPlane( const Pt& p, const Plane& pl )
  {
    //Simply find p' of the point and find the distance between the two
    Pt pPrime = projPtToPlane(p, pl);
    return distPts(p, pPrime);
  }

  float GeometryLibrary::distPtTri( const Pt& p, const Triangle& tri )
  {
    //Project the point onto the plane containing the triangle
    Plane triPlane(tri);
    Pt pPrime = projPtToPlane(p, triPlane);

    //If on the side of a given segment find distance to that
    if(triPlane.n.Cross(tri.p3 - tri.p1).Dot(pPrime - tri.p1) > 0.0f)
      return distPtLs(p, LineSegment(tri.p1, tri.p3));
    else if(triPlane.n.Cross(tri.p1 - tri.p2).Dot(pPrime - tri.p2) > 0.0f)
      return distPtLs(p, LineSegment(tri.p1, tri.p2));
    else if(triPlane.n.Cross(tri.p2 - tri.p3).Dot(pPrime - tri.p3) > 0.0f)
      return distPtLs(p, LineSegment(tri.p2, tri.p3));
    //Otherwise you're somewhere above or below the triangle
    else
      return distPts(p, pPrime);
  }

  float GeometryLibrary::distPtSphere( const Pt& p, const Sphere& sp )
  {
    return distPts(p, sp.center) - sp.radius;
  }

  float GeometryLibrary::distPtAABB( const Pt& p, const AABB& box )
  {
    //construct pmin and pmax out of the box's center and extents
    Pt pmin, pmax;

    pmin = box.center - box.extents;
    pmax = box.center + box.extents;

    //find pPrime by clamping the values of p to the extents of the AABB, finding the closest point
    //on the surfaces of the box.
    Pt pPrime = p;
    
    clamp(pPrime.x, pmin.x, pmax.x);
    clamp(pPrime.y, pmin.y, pmax.y);
    clamp(pPrime.z, pmin.z, pmax.z);

    return distPts(p, pPrime);
  }

  float GeometryLibrary::distLines( const Line& l1, const Line& l2 )
  {
    //Set up the variables in our equation
    //Gotten from the derivation of u(s,t) = l2(t) - l1(s)
    //Where l1(s) = l1.p + s*l1.v and l2(t) = l2.p + t*l2.v
    //We're looking for either the intersection (returning 0), or the magnitude of
    //The vector that is perpendicular to both when they cross an axis
    float a = l1.v.SqMag();
    float b = l1.v.Dot(l2.v);
    float c = l2.v.SqMag();
    float d = (l2.p - l1.p).Dot(l1.v);
    float e = (l2.p - l1.p).Dot(l2.v);

    //We find s and t to find the points on both lines that satisfy the equation,
    float t, s;
    float acMinBsq = (a*c - b*b);

    //if ac - b^2 is zero, then the lines are parallel and we just use t = e / c and s = 0
    if(acMinBsq == 0.0f)
    {
      s = 0.0f;
      t = e / c;  
    }
    //Otherwise we solve the equation for s and t
    else
    {
      s = (c*d - b*e) / acMinBsq;
      t = (b*d - a*e) / acMinBsq;
    }

    //Then we use s and t to find the two points on these lines closest to each other
    Pt p1, p2;

    p1 = l1.p + l1.v * s;
    p2 = l2.p + l2.v * t;

    //Then we find the distance between those points.
    return distPts(p1, p2);
  }

  float GeometryLibrary::distLineSphere( const Line& l1, const Sphere& sp )
  {
    return distPtLine(sp.center, l1) - sp.radius;
  }

  float GeometryLibrary::distLineSegs( const LineSegment& l1, const LineSegment& l2 )
  {
    Vc v1 = l1.p2 - l1.p1;
    Vc v2 = l2.p2 - l2.p1;

    //Set up the problem just like you would lines
    float a = v1.SqMag();
    float b = v1.Dot(v2);
    float c = v2.SqMag();
    float d = (l1.p1 - l2.p1).Dot(v1);
    float e = (l1.p1 - l2.p1).Dot(v2);

    //Check to see if either or both line segments are a single point (edge case)
    if(c == 0.0f && a == 0.0f)
      return distPts(l1.p1, l2.p1);
    else if(a == 0.0f)
      return distPtLs(l1.p1, l2);
    else if(c == 0.0f)
      return distPtLs(l2.p1, l1);

    //We find s and t to find the points on both segments that satisfy the equation,
    float t, s;
    float acMinBsq = (a*c - b*b);

    //Find s
    //if ac - b^2 is zero, then the segments are parallel and s' = 0
    if(acMinBsq == 0)
      s = 0.0f; 
    else
      s = (b*e - c*d) / acMinBsq;

    //Clamp it and use that to find t
    clamp(s, 0.0f, 1.0f);

    t = (e + s*b) / c;
    
    //Clamp t if we need to and then find a new s based on if t is negative or greater than one
    if(t < 0.0f)
    {
      t = 0.0f;
      s = -c / a;
      clamp(s, 0.0f, 1.0f);
    }
    else if(t > 1.0f)
    {
      t = 1.0f;
      s = (b - c) / a;
      clamp(s, 0.0f, 1.0f);
    }

    //Now find the new points and the distance between them
    Pt p1, p2;

    p1 = l1.p1 + v1 * s;
    p2 = l2.p1 + v2 * t;

    return distPts(p1, p2);
  }

  bool GeometryLibrary::contLineSeg( const Pt& p, const LineSegment& ls )
  {
    return distPtLs(p, ls) == 0.0f;
  }

  bool GeometryLibrary::contRay( const Pt& p, const Ray& r )
  {
    return distPtRay(p, r) == 0.0f;
  }

  bool GeometryLibrary::contLine( const Pt& p, const Line& l )
  {
    return distPtLine(p, l) == 0.0f;
  }

  bool GeometryLibrary::contPlane( const Pt& p, const Plane& pl )
  {
    return distPtPlane(p, pl) == 0.0f;
  }

  bool GeometryLibrary::contTri( const Pt& p, const Triangle& tri )
  {
    //Or just use barycentric coordinates...
    return distPtTri(p, tri) == 0.0f;
  }

  bool GeometryLibrary::contSphere( const Pt& p, const Sphere& sp )
  {
    return distPts(p, sp.center) <= sp.radius;
  }

  bool GeometryLibrary::contAABB( const Pt& p, const AABB& box )
  {
    return distPtAABB(p, box) == 0.0f;
  }

  float GeometryLibrary::angleLines( const Line& l1, const Line& l2 )
  {
    //Grab the vectors that represent each line 
    vec3 v1 = l1.v.Normalized();
    vec3 v2 = l2.v.Normalized();

    //give the caller the angle which is cos^-1(v dot v2)
    return acosf(v1.Dot(v2));
  }

  float GeometryLibrary::angleLinePlane( const Line& l, const Plane& pl )
  {
    //Grab the vectors that represent the line and the plane's normal 
    vec3 v1 = l.v.Normalized();
    vec3 v2 = pl.n.Normalized();

    //The angle between the line and the plane is simply sin^-1(v.n) 
    //think of the line as a hypotenuse and the normal as the opposite side
    //of a right triangle
    return asinf(v1.Dot(v2));
  }

  float GeometryLibrary::anglePlanes( const Plane& pl1, const Plane& pl2 )
  {
    //Angle between the planes is the same as the angle between their normals
    return acosf(pl1.n.Dot(pl2.n));
  }

  bool GeometryLibrary::parallelLines( const Line& l1, const Line& l2 )
  {
    //Grab the vectors that represent each line 
    vec3 v1 = l1.v.Normalized();
    vec3 v2 = l2.v.Normalized();

    //The vectors that represent these lines will either face each other or away from each other.
    return abs((v1.Dot(v2))) == 1.0f;
  }

  bool GeometryLibrary::parallelLinePlane( const Line& l, const Plane& pl )
  {
    //Grab the vectors that represent the line and the plane's normal 
    vec3 v1 = l.v.Normalized();

    //a line is parallel to a plane if it is perpendicular to the normal
    return (v1.Dot(pl.n)) == 0;
  }

  bool GeometryLibrary::parallelPlanes( const Plane& pl1, const Plane& pl2 )
  {
    //A plane is parallel with another if their normals face each other or away from each other.
    return abs((pl1.n.Dot(pl2.n))) == 1.0f;
  }

  bool GeometryLibrary::perpLines( const Line& l1, const Line& l2 )
  {
    //Grab the vectors that represent each line 
    vec3 v1 = l1.v.Normalized();
    vec3 v2 = l2.v.Normalized();

    //The vectors that represent these lines will either face each other or away from each other.
    return abs((v1.Dot(v2))) == 0.0f;
  }

  bool GeometryLibrary::perpLinePlane( const Line& l, const Plane& pl )
  {
    //Grab the vectors that represent the line and the plane's normal 
    vec3 v1 = l.v.Normalized();
    vec3 v2 = pl.n.Normalized();

    //a line is perpendicular to a plane if it's going in the same direction as the normal.
    return abs(v1.Dot(v2)) == 1.0f;
  }

  bool GeometryLibrary::perpPlanes( const Plane& pl1, const Plane& pl2 )
  {
    //Two planes are perpendicular if their normals are;
    return abs((pl1.n.Dot(pl2.n))) == 0.0f;
  }

  bool GeometryLibrary::copLinePlane( const Line& l, const Plane& pl )
  {
    //If the line is parallel with the plane, simply find the distance between a point on 
    //the line and the plane.  If it's zero, we're coplanar.
    return parallelLinePlane(l, pl) ? distPtPlane(l.p, pl) == 0 : false;
  }

  bool GeometryLibrary::copPlanes( const Plane& pl1, const Plane& pl2 )
  {
    //If the planes are parallel find the distance from one plane to the next 
    //if that's zero, they're the same plane.
    return parallelPlanes(pl1, pl2) ? distPtPlane(pl1.q, pl2) == 0 : false;
  }

  Intersection GeometryLibrary::intSegPlane( const LineSegment& ls, const Plane& pl )
  {
    Intersection intersect = intLinePlane( ls, pl);

    //If we got an intersection time that is in the segment, we can give the intersection back
    if(intersect.t >= 0.0f && intersect.t <= 1.0f)
      return intersect;
    else
      return Intersection();
  }

  Intersection GeometryLibrary::intSegTri( const LineSegment& ls, const Triangle& tri )
  {
    //First intersect the line segment and the plane formed by the triangle
    Intersection intersect = intSegPlane( ls, Plane(tri) );

    //If the point of intersection is contained in the triangle, we have an intersection
    if(intersect.intersecting && contTri(intersect.p, tri))
      return intersect;
    else
      return Intersection();
  }

  Intersection GeometryLibrary::intRayPlane( const Ray& r, const Plane& pl )
  {
    //treat the ray as a line
    Intersection intersect = intLinePlane(r, pl);

    //If its intersection isn't behind it, we're good
    if(intersect.t >= 0.0f)
      return intersect;
    else
      return Intersection();
  }

  Intersection GeometryLibrary::intRayTri( const Ray& r, const Triangle& tri )
  {
    //Intersect the ray with the plane containing the triangle
    Intersection intersect = intRayPlane( r, Plane(tri) );

    //If we intersect that plane and the point of intersection is in the triangle, we're good.
    if(intersect.intersecting && contTri(intersect.p, tri))
      return intersect;
    else
      return Intersection();
  }

  Intersection GeometryLibrary::intRaySphere( const Ray& r, const Sphere& sp )
  {
    Intersection intersect;

    //Set up the variables for the derived equation at^2 + bt + c
    float a = r.v.SqMag();
    float b = 2 * (r.v.Dot(r.p - sp.center));
    float c = (r.p - sp.center).SqMag() - (sp.radius * sp.radius);

    float discriminate = (b * b) - (4 * a * c);

    //No solution to the equation exists, so return the empty intersection
    if(discriminate < 0.0f)
      return intersect;
    //Or there's one solution (tangent to sphere)
    else if(discriminate == 0.0f)
    {
      intersect.t = -b / (2 * a);

      //If the intersection isn't behind the ray
      if(intersect.t > 0.0f)
      {
        intersect.intersecting = true;
        intersect.p = r.p + r.v * intersect.t;
        return intersect;
      }
      
      //Otherwise there's no intersection
      return Intersection();
    }
    //Or there's two solutions for the line containing the sphere
    else
    {
      //Now we specialize for the ray
      float t1, t2;

      //We're definitely intersecting, but where and how?
      intersect.intersecting = true;

      //Grab the two solutions to the quadratic equation of the line containing the ray
      t1 = (-b + sqrtf(discriminate)) / (2 * a);
      t2 = (-b - sqrtf(discriminate)) / (2 * a);

      //Two intersection points, use the closest point
      if(t1 >= 0.0f && t2 >= 0.0f)
        intersect.t = min(t1, t2);

      //One point, the other is behind the ray
      else if(t1 >= 0.0f && t2 < 0.0f)
        intersect.t = t1;
 
      //Same case, but with the other t value
      else if(t1 < 0.0f && t2 >= 0.0f)
        intersect.t = t2;

      //Line intersects, but ray does not.
      else if(t1 < 0.0f && t2 < 0.0f)
        return Intersection();

      //Figure out the point of intersection based on what we found t to be
      intersect.p = r.p + r.v * intersect.t;

      //Give back the intersection, whatever we decided it was
      return intersect;
    }
  }

  Intersection GeometryLibrary::intRayAABB( const Ray& r, const AABB& box )
  {
    Intersection intersect;

    //Generate the six halfspaces we want to test against
    Plane halfSpaces[6] = {Plane(box.center + Pt(0,0, box.extents.z), Vc(0,0, 1) ), //Front
                           Plane(box.center - Pt(0,0, box.extents.z), Vc(0,0,-1) ), //Back
                           Plane(box.center - Pt(box.extents.x, 0,0), Vc(-1,0,0) ), //Left
                           Plane(box.center + Pt(box.extents.x, 0,0), Vc( 1,0,0) ), //Right
                           Plane(box.center - Pt(0,box.extents.y, 0), Vc(0,-1,0) ), //Bottom
                           Plane(box.center + Pt(0,box.extents.y, 0), Vc(0, 1,0) )  //Top
                          };

    //This is the range we keep updating for all the halfspaces
    TRange range;

    //Generate the first range to intersect the rest with.
    range = cutRayWithHalfSpace(r, halfSpaces[0]);

    //Update the range of t with the rest of the halfspaces
    for(unsigned i = 1; i < 6; ++i)
    {
      range = intTRanges(range, cutRayWithHalfSpace(r, halfSpaces[i]));

      //if the range is the null set, then we just return an empty intersection
      if(range.tMax == FLT_MAX && range.tMin == FLT_MAX)
        return intersect;
    }
    
    //tmin <= tmax and either greater than 0
    if(range.tMin <= range.tMax)
    {
      //Take the lesser of the two, whichever is >= 0
      if(range.tMin >= 0.0f)
      {
        intersect.intersecting = true;
        intersect.t = range.tMin;
        intersect.p = r.p + r.v * intersect.t;
      }
      else if(range.tMax >= 0.0f)
      {
        intersect.intersecting = true;
        intersect.t = range.tMax;
        intersect.p = r.p + r.v * intersect.t;
      }
    }

    return intersect;
  }

  Intersection GeometryLibrary::intLinePlane( const Line& l, const Plane& pl )
  {
    //If they're parallel we have no intersection
    if(parallelLinePlane(l, pl)) return Intersection();

    //Otherwise they intersect, and we find the point of intersection
    Intersection intersect;
    intersect.intersecting = true;

    intersect.t = ( -pl.n.Dot(pl.q) - pl.n.Dot(l.p) ) / pl.n.Dot(l.v);
    intersect.p = l.p +  l.v * intersect.t;
    return intersect;
  }

  Intersection GeometryLibrary::intSpheres( const Sphere& sp1, const Sphere& sp2 )
  {
    //If the distance between the two centers is less than or equal to the combined radii, we're intersecting
    Intersection intersect;
    if(distPts(sp1.center, sp2.center) <= sp1.radius + sp2.radius)
      intersect.intersecting = true;

    return intersect;
  }

  Intersection GeometryLibrary::intTris( const Triangle& t1, const Triangle& t2 )
  {
    Intersection intersect;

    //Create three line segments out of each triangle
    LineSegment t1Segs[3] = {LineSegment(t1.p1, t1.p2), 
                             LineSegment(t1.p1, t1.p3), 
                             LineSegment(t1.p2, t1.p3)};

    LineSegment t2Segs[3] = {LineSegment(t2.p1, t2.p2), 
                             LineSegment(t2.p1, t2.p3), 
                             LineSegment(t2.p2, t2.p3)};

    //If any line segment on either triangle intersects the opposing triangle, we have an intersection
    for(unsigned i = 0; i < 3 && !intersect.intersecting; ++i)
      intersect = intSegTri(t1Segs[i], t2);

    if(intersect.intersecting) return intersect;

    for(unsigned i = 0; i < 3 && !intersect.intersecting; ++i)
      intersect = intSegTri(t2Segs[i], t1);

    return intersect;
  }

  Intersection GeometryLibrary::intAABBs( const AABB& box1, const AABB& box2 )
  {
    Intersection intersect;
    
    //If the distance between the centers on every axis is less than the sum of those extents
    //then we have an intersection
    float distX = abs(box1.center.x - box2.center.x);
    float distY = abs(box1.center.y - box2.center.y);
    float distZ = abs(box1.center.z - box2.center.z);

    if(distX < box1.extents.x + box2.extents.x &&
       distY < box1.extents.y + box2.extents.y &&
       distZ < box1.extents.z + box2.extents.z)
       intersect.intersecting = true;

    return intersect;
  }

  TRange GeometryLibrary::cutRayWithHalfSpace( const Ray& r, const Plane& halfspace )
  {
    float d1 = (r.p - halfspace.q).Dot(halfspace.n);
    float d2 = r.v.Dot(halfspace.n);
    float t = -d1 / d2;

    //V points toward rear halfspace
    if(d2 < 0.0f)
      return TRange(t, FLT_MAX);

    //V points toward front halfspace
    else if(d2 > 0.0f)
      return TRange(-FLT_MAX, t);

    //V is parallel to plane and in front halfspace (empty set)
    else if(d1 > 0.0f)
      return TRange(FLT_MAX, FLT_MAX);

    //V is parallel to plane and in rear halfspace
    else
      return TRange(-FLT_MAX, FLT_MAX);
  }

  void GeometryLibrary::buildKDTree()
  {
    sceneKdTree = new KDTree(staticSceneTris, dbgDraw, device);
    sceneKdTree->build();
  }

  GeometryLibrary::GeometryLibrary(DebugDraw* dbg, ID3D10Device* dev) : sceneKdTree(NULL), 
                                                                        dbgDraw(dbg),
                                                                        device(dev)
  {
  }

  GeometryLibrary::~GeometryLibrary()
  {
    if(sceneKdTree)
      delete sceneKdTree;

    if(sceneDBVH)
      delete sceneDBVH;
  }

  TRange GeometryLibrary::intTRanges( const TRange& t1, const TRange& t2 )
  {
    return TRange( max(t1.tMin, t2.tMin), min(t1.tMax, t2.tMax) );
  }

  KDTree* GeometryLibrary::getKDTree()
  {
    return sceneKdTree;
  }

  void GeometryLibrary::buildDBVH()
  {
    sceneDBVH = new DBVH(sceneObjects, dbgDraw, device);
    sceneDBVH->build();
  }

  DBVH* GeometryLibrary::getDBVH()
  {
    return sceneDBVH;
  }

  void GeometryLibrary::addObjectToScene( const Object& obj )
  {
    sceneObjects.push_back(obj);
  }

  void GeometryLibrary::splitBox( AABB& l, AABB& r, const AABB& p, float pos, SplitAxis axis )
  {
    //They all start out as the same box
    l = r = p;

    //Grab the min and the max of the parent box, as we'll need that for splitting on any axis
    Pt pMax = p.center + p.extents;
    Pt pMin = p.center - p.extents;

    //Modify extents and center of the box 
    if(axis == XAxis)
    {
      //The extents need to be half of the position
      //if we split at 4, we should be able to get to both 0 and 4 with - and + respectively
      l.extents.x = abs(pos - pMin.x) / 2.0f;

      //And the right box is whatever is left over (half of that of course)
      r.extents.x = abs(pMax.x - pos) / 2.0f;

      //The center of the left and right boxes is the midpoint between the max and min points of each box
      l.center.x = (pMin.x + pos) / 2.0f;
      r.center.x = (pMax.x + pos) / 2.0f;
    }

    //If we split on y, we use y coordinates and the height of the box
    else if(axis == YAxis)
    {
      //The extents need to be half of the position
      //if we split at 4, we should be able to get to both 0 and 4 with - and + respectively
      l.extents.y = abs(pos - pMin.y) / 2.0f;

      //And the right box is whatever is left over (half of that of course)
      r.extents.y = abs(pMax.y - pos) / 2.0f;

      //The center of the left box is half the split point (only on y)
      l.center.y = (pMin.y + pos) / 2.0f;
      r.center.y = (pMax.y + pos) / 2.0f;
    }

    //Z is depth and z coordinates
    else
    {
      //The extents need to be half of the position
      //if we split at 4, we should be able to get to both 0 and 4 with - and + respectively
      l.extents.z = abs(pos - pMin.z) / 2.0f;

      //And the right box is whatever is left over (half of that of course)
      r.extents.z = abs(pMax.z - pos) / 2.0f;

      //The center of the left box is half the split point (only on z)
      l.center.z = (pMin.z + pos) / 2.0f;
      r.center.z = (pMax.z + pos) / 2.0f;
    }
  }

  static bool SortTrisX( const Triangle& t1, const Triangle& t2 )
  {
    return min(min(t1.p1.x, t1.p2.x), t1.p3.x) < min(min(t2.p1.x, t2.p2.x), t2.p3.x);
  }

  static bool SortTrisY( const Triangle& t1, const Triangle& t2 )
  {
    return min(min(t1.p1.y, t1.p2.y), t1.p3.y) < min(min(t2.p1.y, t2.p2.y), t2.p3.y);
  }

  static bool SortTrisZ( const Triangle& t1, const Triangle& t2 )
  {
    return min(min(t1.p1.z, t1.p2.z), t1.p3.z) < min(min(t2.p1.z, t2.p2.z), t2.p3.z);
  }

  float GeometryLibrary::minPos( const Triangle& tri, SplitAxis axis )
  {
    if(axis == XAxis)
      return min(min(tri.p1.x, tri.p2.x), tri.p3.x);
    else if(axis == YAxis)
      return min(min(tri.p1.y, tri.p2.y), tri.p3.y);
    else
      return min(min(tri.p1.z, tri.p2.z), tri.p3.z);
  }

  float GeometryLibrary::maxPos( const Triangle& tri, SplitAxis axis )
  {
    if(axis == XAxis)
      return max(max(tri.p1.x, tri.p2.x), tri.p3.x);
    else if(axis == YAxis)
      return max(max(tri.p1.y, tri.p2.y), tri.p3.y);
    else
      return max(max(tri.p1.z, tri.p2.z), tri.p3.z);
  }

  AABB GeometryLibrary::buildAABBFromTriList( TriangleList& triList )
  {
    AABB box;

    //Find min and max positions of each axis in the list.
    Pt min, max;

    //y
    std::sort(triList.begin(), triList.end(), SortTrisY);
    min.y = minPos(triList[0], YAxis);
    max.y = maxPos(triList[triList.size() - 1], YAxis);

    //z
    std::sort(triList.begin(), triList.end(), SortTrisZ);
    min.z = minPos(triList[0], ZAxis);
    max.z = maxPos(triList[triList.size() - 1], ZAxis);

    //x
    std::sort(triList.begin(), triList.end(), SortTrisX);
    min.x = minPos(triList[0], XAxis);
    max.x = maxPos(triList[triList.size() - 1], XAxis);

    //Now we construct the AABB of the list
    float width, height, depth;
    width  = abs(max.x - min.x);
    height = abs(max.y - min.y);
    depth  = abs(max.z - min.z);

    box.extents.x = width / 2.0f;
    box.extents.y = height / 2.0f;
    box.extents.z = depth / 2.0f;

    //Use the box's extent and minimum point to find the center.
    box.center = min + box.extents;

    return box;
  }

  float GeometryLibrary::perimeter( const AABB& box )
  {
    return 8.0f * (box.extents.x + box.extents.y + box.extents.z);
  }

  AABB GeometryLibrary::mergeAABBs( const AABB& b1, const AABB& b2 )
  {
    //box we're building
    AABB box;

    //calculate min and max points of each bounding box
    Pt min1, min2, max1, max2, newMin, newMax;

    min1 = b1.center - b1.extents;
    max1 = b1.center + b1.extents;

    min2 = b2.center - b2.extents;
    max2 = b2.center + b2.extents;

    //Find min and max on all three axes
    newMin.x = min(min1.x, min2.x);
    newMin.y = min(min1.y, min2.y);
    newMin.z = min(min1.z, min2.z);

    newMax.x = max(max1.x, max2.x);
    newMax.y = max(max1.y, max2.y);
    newMax.z = max(max1.z, max2.z);

    //Extents are just width/height/depth / 2
    box.extents.x = abs(newMax.x  - newMin.x) * 0.5f;
    box.extents.y = abs(newMax.y  - newMin.y) * 0.5f;
    box.extents.z = abs(newMax.z  - newMin.z) * 0.5f;

    //Center is just min + extents
    box.center = newMin + box.extents;

    return box;
  }

  MPortal GeometryLibrary::initialPortal( Object& obj1, Object& obj2 )
  {
    MPortal m;

    //Center = obj2.center - obj1.center
    m.c = obj2.boundingBox.center - obj1.boundingBox.center;

    //first support vector is -center (from center to origin)
    vec3 sv1(-m.c);
    sv1.Normalize();
    Pt s1, s2;
    s1 = supportFn(-sv1, obj1);
    s2 = supportFn(sv1, obj2);
    m.p1 = s2 - s1;

    //Second support vector is (pp1 - 0) X (c - 0)
    vec3 sv2 = m.p1.Cross(m.c);
    sv2.Normalize();
    s1 = supportFn(-sv2, obj1);
    s2 = supportFn(sv2, obj2);
    m.p2 = s2 - s1;

    //Third support vector is (pp2 - c) X (pp1 - c)
    vec3 sv3 = (m.p2 - m.c).Cross((m.p1 - m.c));
    sv3.Normalize();
    s1 = supportFn(-sv3, obj1);
    s2 = supportFn(sv3, obj2);
    m.p3 = s2 - s1;

    return m;
  }

  Pt GeometryLibrary::supportFn( vec3& sv, Object& obj )
  {
    //Most postive vert in model space
    Pt furthestPt;
    D3DXMATRIX* mtw = NULL;

    //loop through all object's meshes
    for(unsigned i = 0; i < obj.meshes.size(); ++i)
    {
      //Transform sv into model's local space
      Mesh *m = obj.meshes[i];

      //Use the inverse of its mtw
      D3DXMATRIX invMtw;
      D3DXMatrixInverse(&invMtw, 0, &m->modToW);

      mat4 wtm(invMtw);

      wtm.Transpose();

      vec3 svm = wtm.MultVec(sv);

      //Dot vector with all mesh's vertices, save the most positive vertex
      float dval = -FLT_MAX;

      for(unsigned j = 0; j < m->verts.size(); ++j)
      {
        float dot = svm.Dot(m->verts[j].pos);
        
        if(dot > dval)
        {
          dval = dot;
          furthestPt = m->verts[j].pos;
          mtw = &m->modToW;
        }
      }
    }
    
    if(mtw)
    {
      //Return that vertex in world space
      mat4 finalmtw(*mtw);
      finalmtw.Transpose();
      return finalmtw.MultPt(furthestPt);
    }
    else 
      return furthestPt;
  }

  
  void GeometryLibrary::discovery( MPortal& portal, Object& obj1, Object& obj2 )
  {
    static unsigned discoverLoops = 25;
    float dot;
    vec3 faceNorm;
    unsigned i = 0;
    //Only try this x times, otherwise, we don't have an intersection
    //After all three faces pass, we still have a possible collision
    for(; i < discoverLoops; ++i)
    {
      //Try face 1 (c, p1, p2)
      faceNorm = (portal.p2 - portal.c).Cross((portal.p1 - portal.c));
      dot = faceNorm.Dot(portal.p3 - portal.c);
      if(dot > 0)
        faceNorm = -faceNorm;

      //Face one pass/fail (if the origin is in front of this face, we need a better portal
      dot = (-faceNorm).Dot(portal.c);
      if(dot > 0)
      {
        vec3 sv = faceNorm;
        sv.Normalize();
        Pt s1, s2;
        s1 = supportFn(-sv, obj1);
        s2 = supportFn(sv,  obj2);
        portal.p3 = s2 - s1;
        continue;
      }

      //Try face 2 (c, p1, p3)
      faceNorm = (portal.p3 - portal.c).Cross((portal.p1 - portal.c));
      dot = faceNorm.Dot(portal.p2 - portal.c);
      if(dot > 0)
        faceNorm = -faceNorm;

      //Face two pass/fail (if the origin is in front of this face, we need a better portal
      dot = (-faceNorm).Dot(portal.c);
      if(dot > 0)
      {
        vec3 sv = faceNorm;
        sv.Normalize();
        Pt s1, s2;
        s1 = supportFn(-sv, obj1);
        s2 = supportFn(sv,  obj2);
        portal.p2 = s2 - s1;
        continue;
      }

      //Try face 3 (c, p2, p3)
      faceNorm = (portal.p3 - portal.c).Cross((portal.p2 - portal.c));
      dot = faceNorm.Dot(portal.p1 - portal.c);
      if(dot > 0)
        faceNorm = -faceNorm;

      //Face three pass/fail (if the origin is in front of this face, we need a better portal
      dot = (-faceNorm).Dot(portal.c);
      if(dot > 0)
      {
        vec3 sv = faceNorm;
        sv.Normalize();
        Pt s1, s2;
        s1 = supportFn(-sv, obj1);
        s2 = supportFn(sv,  obj2);
        portal.p1 = s2 - s1;
      }
      else
        break;
    }
  }

  void GeometryLibrary::refinement( MPortal& portal, Object& obj1, Object& obj2 )
  {
    static unsigned refineLoops = 25;
    bool collision = false;
    
    for(unsigned i = 0; i < refineLoops; ++i)
    {
      //Is the origin behind the front facing plane?
      //Simply dot the c to origin vector with a p1 to origin vector, if it's positive, then outside, negative is inside
      float dot = (-portal.c).Dot(-portal.p1);
      if(dot > 0)
      {
        //Normal of the front of the portal, used for finding the next point
        vec3 frontNorm = (portal.p3 - portal.p1).Cross(portal.p2 - portal.p1);
        if(frontNorm.Dot((portal.p1 - portal.c)) > 0)
          frontNorm = -frontNorm;

        //Find the new point using the normal as a support vector        
        vec3 sv = frontNorm;
        sv.Normalize();
        Pt s1, s2, newPoint;
        s1 = supportFn(-sv, obj1);
        s2 = supportFn(sv,  obj2);
        newPoint = s2 - s1;

        //Figure out which point to replace with new point, then replace it.
        //So whichever side this thing is on, we replace the opposite point
        vec3 faceNorm = (portal.p2 - portal.c).Cross((portal.p1 - portal.c));
        dot = faceNorm.Dot(portal.p3 - portal.c);
        if(dot > 0)
          faceNorm = -faceNorm;

        if((-faceNorm).Dot(newPoint) > 0)
        {
          portal.p1 = newPoint;
          continue;
        }

        //face 2
        faceNorm = (portal.p3 - portal.c).Cross((portal.p1 - portal.c));
        dot = faceNorm.Dot(portal.p2 - portal.c);
        if(dot > 0)
          faceNorm = -faceNorm;

        if((-faceNorm).Dot(newPoint) > 0)
        {
          portal.p2 = newPoint;
          continue;
        }

        portal.p3 = newPoint;      
      }
      else
      {
        collision = true;
        break;
      }
    }

    //If we got through refinement with a collision, mark that shit!
    if(collision)
    {
      dbgDraw->drawAABB(obj1.boundingBox, vec4(1.0f, 0.0f, 0.0f, 1.0f));
      dbgDraw->drawAABB(obj2.boundingBox, vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }
  }

  Intersection::Intersection( const Pt& _p, float _t, bool inter ): p(_p), t(_t), intersecting(inter)
  {
  }


  Plane::Plane( const Triangle& tri /*= Triangle() */ )
  {
    q = tri.p1;
    n = (tri.p2 - tri.p1).Cross(tri.p3 - tri.p1);
  }

  Plane::Plane( const Pt& _q, const Vc& _n )
    : q(_q), n(_n)
  {

  }

  Plane::Plane( const Pt& _q, const Vc& _v1, const Vc& _v2 )
    : v1(_v1), v2(_v2), q(_q)
  {
  }

  Line::Line( const Pt& _p1 /*= Pt(0.0f,0.0f,0.0f)*/, const Pt& _p2 /*= Pt(0.0f,0.0f,0.0f)*/ )
    : p1(_p1), p2(_p2), p(_p1), v(_p2 - _p1)
  {
    v.Normalize();
  }


  TRange::TRange( float min /*= 0.0f*/, float max /*= 0.0f*/ )
    : tMin(min), tMax(max)
  {

  }


  AABB::AABB( Pt& c /*= Pt(0.0f,0.0f,0.0f)*/, Vc& e /*= Vc(0.5f, 0.5f, 0.5f)*/ )
    :center(c), extents(e)
  {

  }


  MPortal::MPortal( Pt& center, Pt& pp1, Pt& pp2, Pt& pp3 ) : c(center), p1(pp1), p2(pp2), p3(pp3)
  {
  }

}
