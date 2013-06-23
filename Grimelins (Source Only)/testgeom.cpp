/*! ====================================================================================================================
  @file testgeom.cpp

  @author Maximilian Manndorff
  @par Email maximilian.manndorff@@digipen.edu
  @par Login maximilian.manndorff
  @par ID 50006909
  @par Course CS350
  @par Project 1
  @date Feb.07.2012

  @brief 
    Test of geometry lib

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *

===================================================================================================================== */
#include "precompiled.h"
#include "testgeom.h"
#include "math.h"
#include "geometry.h"

using namespace Grimelins;

void TestAll(void)
{
  Tester suite;
  suite.AddTest(TestProjPointLine());
  suite.AddTest(TestProjPointPlane());
  suite.AddTest(TestDistPointPoint());
  suite.AddTest(TestDistPointLineSegment());
  suite.AddTest(TestDistPointRay());
  suite.AddTest(TestDistPointLine());
  suite.AddTest(TestDistPointPlane());
  suite.AddTest(TestDistPointTriangle());
  suite.AddTest(TestDistPointSphere());
  suite.AddTest(TestDistPointAABB());
  suite.AddTest(TestDistLineLine());
  suite.AddTest(TestDistLineSphere());
  suite.AddTest(TestDistLineSegLineSeg());
  suite.AddTest(TestContainedPointLineSeg());
  suite.AddTest(TestContainedPointRay());
  suite.AddTest(TestContainedPointLine());
  suite.AddTest(TestContainedPointPlane());
  suite.AddTest(TestContainedPointTriangle());
  suite.AddTest(TestContainedPointSphere());
  suite.AddTest(TestContainedPointAABB());
  suite.AddTest(TestIntersectionLinePLane());
  suite.AddTest(TestIntersectionLineSegmentPlane());
  suite.AddTest(TestIntersectionLineSegmentTriangle());
  suite.AddTest(TestIntersectionRayPlane());
  suite.AddTest(TestIntersectionRayTriangle());
  suite.AddTest(TestIntersectionRaySphere());
  suite.AddTest(TestIntersectionRayAABB());
  suite.AddTest(TestIntersectionSphereSphere());
  suite.AddTest(TestIntersectionTriangleTriangle());
  suite.AddTest(TestIntersectionAabbAabb());
  suite.AddTest(TestAngleLineLine());
  suite.AddTest(TestAngleLinePlane());
  suite.AddTest(TestAnglePlanePlane());
  suite.AddTest(TestParallelLineLine());
  suite.AddTest(TestParallelLinePlane());
  suite.AddTest(TestParallelPlanePlane());
  suite.AddTest(TestPerpendicularLineLine());
  suite.AddTest(TestPerpendicularLinePlane());
  suite.AddTest(TestPerpendicularPlanePlane());
  suite.AddTest(TestCoplanarLinePlane());
  suite.AddTest(TestCoplanarPlanePlane());


  suite.TestAll(std::ofstream("test.txt"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// projPointLine

std::string TestProjPointLine::GetName(void) const
{
  return "projPointLine";
}

bool TestProjPointLine::RunTest(void)
{
  Point p(0,1,0);
  Line l;
  l.Start = Point(0,0,0);
  l.Direction = Vector3(1,1,0);

  Point pp = projection(p, l);

  return pp == Vector3(0.5f, 0.5f, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// projPointPlane

std::string TestProjPointPlane::GetName(void) const
{
  return "projPointPlane";
}

bool TestProjPointPlane::RunTest(void)
{
  Point p(0, 0, 0);
  Plane pl;
  pl.P = Point(0, -1, 0);
  pl.Dir1 = Vector3(1, 0, 0);
  pl.Dir2 = Vector3(0, 0, 1);

  Point pp = projection(p, pl);

  return pp == Point(0, -1, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distPointPoint

std::string TestDistPointPoint::GetName(void) const
{
  return "distPointPoint";
}

bool TestDistPointPoint::RunTest(void)
{
  Point p1(0, 0, 0);
  Point p2(0, 1, 0);

  return fequal(distance(p1, p2), 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distPointLineSegment

std::string TestDistPointLineSegment::GetName(void) const
{
  return "distPointLineSegment";
}

bool TestDistPointLineSegment::RunTest(void)
{
  Point p(0, 0, 0);

  LineSegment ls;
  ls.Start = Point(0, 0, 0);
  ls.Extend = Vector3(2, 2, 0);

  float dist1 = distance(p, ls);
  
  p = Point(0, 1, 0);
  float dist2 = distance(p, ls);

  p = Point(0, -1, 0);
  float dist3 = distance(p, ls);

  return fequal(dist1, 0.0f) && fequal(dist2, std::sqrt(2.0f) * 0.5f) && fequal(dist3, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distPointRay

std::string TestDistPointRay::GetName(void) const
{
  return "distPointRay";
}

bool TestDistPointRay::RunTest(void)
{
  Ray r;
  r.Start = Point(0,0,0);
  r.Dir = Vector3(1, 1, 0);

  Point p(0, 0, 0);
  float dist1 = distance(p, r);

  p = Point(0, 1, 0);
  float dist2 = distance(p, r); // sqrt(2) / 2

  p = Point(0, -1, 0);
  float dist3 = distance(p, r);

  p = Point(0, 1, 0);
  r.Start = Point(1, 0, 0);
  r.Dir = Vector3(0, 1, 0);
  float dist4 = distance(p, r); // 1

  float dist2_correct = std::sqrt(2.0f) * 0.5f;
  return fequal(dist1, 0.0f) && fequal(dist2, dist2_correct) && fequal(dist3, 1.0f) && fequal(dist4, 1.0f);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distPointLine

std::string TestDistPointLine::GetName(void) const
{
  return "distPointLine";
}

bool TestDistPointLine::RunTest(void)
{
  Line l;
  l.Start = Point(0, 0, 0);
  l.Direction = Vector3(1, 1, 0);

  Point p(0, 1, 0);
  float dist1 = distance(p, l);

  return fequal(dist1, std::sqrt(2.0f) * 0.5f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distPointPlane

std::string TestDistPointPlane::GetName(void) const
{
  return "distPointPlane";
}

bool TestDistPointPlane::RunTest(void)
{
  Plane pl;
  pl.P = Point(0, -1, 0);
  pl.Dir1 = Vector3(1, 0, 0);
  pl.Dir2 = Vector3(0, 0, 1);

  Point p(0, 0, 0);

  return fequal(distance(p, pl), 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distPointTriangle

std::string TestDistPointTriangle::GetName(void) const
{
  return "distPointTriangle";
}

bool TestDistPointTriangle::RunTest(void)
{
  Triangle t =
  {
    Vector3(0, 0, 0), 
    Vector3(1, 0, 0), 
    Vector3(0, 1, 0)
  };
  
  Point p(0, 0, 0);
  float dist1 = distance(p, t);

  p = Point(0, 2, 0);
  float dist2 = distance(p, t);

  p = Point(0.5, 0.5, 0);
  float dist3 = distance(p, t);

  return fequal(dist1, std::sqrt(2.0f) * 0.5f) && fequal(dist2, 1.0f) && fequal(dist3, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distPointSphere

std::string TestDistPointSphere::GetName(void) const
{
  return "distPointSphere";
}

bool TestDistPointSphere::RunTest(void)
{
  Sphere s;
  s.Center = Point(0, 0, 0);
  s.Radius = 1.0f;

  Point p(0, 0, 0);
  float dist1 = distance(p, s);

  p = Point(0, 1, 0);
  float dist2 = distance(p, s);

  p = Point(0, 5, 0);
  float dist3 = distance(p, s);

  return fequal(dist1, -1.0f) && fequal(dist2, 0.0f) && fequal(dist3, 4.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distPointAABB

std::string TestDistPointAABB::GetName(void) const
{
  return "distPointAABB";
}

bool TestDistPointAABB::RunTest(void)
{
  AABB_V2 b;
  b.Center = Point(0, 0, 0);
  b.Extend = Vector3(0.5, 0.5, 0.5);

  Point p(0, 0, 0);
  float dist1 = distance(p, b);

  p = Point(0, 0.5, 0);
  float dist2 = distance(p, b);

  p = Point(0, 4.5, 0);
  float dist3 = distance(p, b);

  return fequal(dist1, 0.0f) && fequal(dist2, 0.0f) && fequal(dist3, 4.0f);  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distLineLine

std::string TestDistLineLine::GetName(void) const
{
  return "distLineLine";
}

bool TestDistLineLine::RunTest(void)
{
  Line l1, l2;

  l1.Start = Point(0, 0, 0);
  l1.Direction = Vector3(0, 1, 0);
  l2.Start = Point(1, 0, 0);
  l2.Direction = Vector3(0, 1, 0);
  float dist1 = distance(l1, l2);

  l1.Direction = Vector3(1, 0, 0);
  float dist2 = distance(l1, l2);

  l1.Start = Vector3(0, 0, 1);
  float dist3 = distance(l1, l2);

  return fequal(dist1, 1.0f) && fequal(dist2, 0.0f) && fequal(dist3, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distLineSphere

std::string TestDistLineSphere::GetName(void) const
{
  return "distLineSphere";
}

bool TestDistLineSphere::RunTest(void)
{
  Sphere s;
  s.Center = Point(0, 0, 0);
  s.Radius = 1.0f;

  Line l;
  l.Start = Point(0, -2, 0);
  l.Direction = Vector3(0, 1, 0);
  float dist1 = distance(l, s);

  l.Start = Point(1, -2, 0);
  float dist2 = distance(l, s);

  l.Start = Point(5, -2, 0);
  float dist3 = distance(l, s);

  return fequal(dist1, -1.0f) && fequal(dist2, 0.0f) && fequal(dist3, 4.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// distLineSegLineSeg

std::string TestDistLineSegLineSeg::GetName(void) const
{
  return "distLineSegLineSeg";
}

bool TestDistLineSegLineSeg::RunTest(void)
{
  LineSegment ls1, ls2;
   
  ls1.Start = Point(0, 0, 0);
  ls1.Extend = Vector3(0, 1, 0);
  ls2.Start = Point(1, 0, 0);
  ls2.Extend = Vector3(0, 1, 0);
  float dist1 = distance(ls1, ls2);

  ls2.Start = Point(0, 2, 0);
  float dist2 = distance(ls1, ls2);

  ls2.Start = Point(0, 1, 0);
  ls2.Extend = Vector3(1, 0, 0);
  float dist3 = distance(ls1, ls2);

  return fequal(dist1, 1.0f) && fequal(dist2, 1.0f) && fequal(dist3, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// containedPointLineSeg


std::string TestContainedPointLineSeg::GetName(void) const
{
  return "containedPointLineSeg";
}

bool TestContainedPointLineSeg::RunTest(void)
{
  Point p(0, 1, 0);
  LineSegment ls;
  ls.Start = Point(0, 0, 0);
  ls.Extend = Vector3(0, 2, 0);
  bool ret1 = isContained(p, ls);

  ls.Extend = Vector3(2, 2, 0);
  bool ret2 = isContained(p, ls);

  p = Point(1, 1, 0);
  bool ret3 = isContained(p, ls);

  return (ret1 && !ret2 && ret3);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// containedPointRay

std::string TestContainedPointRay::GetName(void) const
{
return "containedPointRay";
}

bool TestContainedPointRay::RunTest(void)
{
  Point p(0, 1, 0);
  Ray r;
  r.Start = Point(0, 0, 0);
  r.Dir = Vector3(0, 0.1, 0);
  bool ret1 = isContained(p, r);

  r.Dir = Vector3(0.1, 0.1, 0);
  bool ret2 = isContained(p, r);

  p = Point(2, 2, 0);
  bool ret3 = isContained(p, r);

  p = -p;
  bool ret4 = isContained(p, r);

  return (ret1 && !ret2 && ret3 && !ret4);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// containedPointLine

std::string TestContainedPointLine::GetName(void) const
{
  return "containedPointLine";
}

bool TestContainedPointLine::RunTest(void)
{
  Point p(0, 1, 0);
  Line l;
  l.Start = Point(0, 0, 0);
  l.Direction = Vector3(0, 0.1, 0);
  bool ret1 = isContained(p, l);

  p = -p;
  bool ret2 = isContained(p, l);

  p = Point(1, 1, 0);
  bool ret3 = isContained(p, l);

  p = -p;
  l.Direction = Vector3(0.1, 0.1, 0);
  bool ret4 = isContained(p, l);

  return (ret1 && ret2 && !ret3 && ret4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// containedPointPlane

std::string TestContainedPointPlane::GetName(void) const
{
  return "containedPointPlane";
}

bool TestContainedPointPlane::RunTest(void)
{
  Point p(1, 1, 0);
  Plane pl;
  pl.P = Point(0, 0, 0);
  pl.Dir1 = Vector3(1, 0, 0);
  pl.Dir2 = Vector3(0, 1, 0);
  bool ret1 = isContained(p, pl); // true

  p = Point(0, 0, 1);
  bool ret2 = isContained(p, pl); // false

  p = Point(10, 10, 0);
  bool ret3 = isContained(p, pl); // true

  pl.Dir1 = Vector3(1, 1, 0);
  pl.Dir2 = Vector3(0, 0, 1);
  bool ret4 = isContained(p, pl); // true

  return (ret1 && !ret2 && ret3 && ret4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// containedPointTriangle

std::string TestContainedPointTriangle::GetName(void) const
{
  return "containedPointTriangle";
}

bool TestContainedPointTriangle::RunTest(void)
{
  Point p(0, 0, 0);
  Triangle t;
  t.point[0] = Point(0, 1, 0);
  t.point[1] = Point(1, 0, 0);
  t.point[2] = Point(-1, -1, 0);
  bool ret1 = isContained(p, t); // true;

  p = Point(-1, -1, 0);
  bool ret2 = isContained(p, t); // false

  p = Point(-1, -1, 1);
  bool ret3 = isContained(p, t); // false;

  p = Point(1, 1, 0);
  bool ret4 = isContained(p, t); // false

  return (ret1 && !ret2 && !ret3 && !ret4);   
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// containedPointSphere

std::string TestContainedPointSphere::GetName(void) const
{
  return "containedPointSphere";
}

bool TestContainedPointSphere::RunTest(void)
{
  Point p(0, 0, 0);
  Sphere s;
  s.Center = p;
  s.Radius = 1;
  bool ret1 = isContained(p, s); // true

  p = Point(1, 1, 1);
  bool ret2 = isContained(p, s); // false

  p = Point(0.5, 0.5, 1.01);
  bool ret3 = isContained(p, s); // false

  s.Radius = 1.1;
  bool ret4 = isContained(p, s); // true

  p = Point(0.5, 0.5, 0.5);
  bool ret5 = isContained(p, s); // true

  return (ret1 && !ret2 && !ret3 && !ret4 && ret5);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// containedPointAABB

std::string TestContainedPointAABB::GetName(void) const
{
  return "containedPointAABB";
}

bool TestContainedPointAABB::RunTest(void)
{
  Point p(0, 0, 0);
  AABB_V2 bx;
  bx.Center = p;
  bx.Extend = Vector3(1, 1, 0);
  bool ret1 = isContained(p, bx); // false

  p = Point(0, 0, 0.001);
  bool ret2 = isContained(p, bx); // false

  bx.Extend = Vector3(1, 1, 1);
  bool ret3 = isContained(p, bx); // true

  p = Point(1, 1, 1);
  bool ret4 = isContained(p, bx); // true

  p = -p;
  bool ret5 = isContained(p, bx); // true

  p = Point(2, 2, 2);
  bool ret6 = isContained(p, bx); // false

  p = -p;
  bool ret7 = isContained(p, bx); // false

  return (ret1 && !ret2 && ret4 && ret4 && ret5 && !ret6 && !ret7);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectionLinePlane

std::string TestIntersectionLinePLane::GetName(void) const
{
  return "intersectionLinePlane";
}

bool TestIntersectionLinePLane::RunTest(void)
{
  Line l;
  Plane pl;
  l.Start = Point(0, 0, 0);
  l.Direction = Vector3(1, 1, 1);
  pl.P = l.Start;
  pl.Dir1 = Vector3(1, 0, 0);
  pl.Dir2 = Vector3(0, 0, 1);
  IntersectionPoint i1 = intersection(l, pl); // true, I = (0, 0, 0), t = 0

  l.Direction = pl.Dir1;
  IntersectionPoint i2 = intersection(l, pl); // false

  l.Direction = Vector3(0, 1, 0);
  pl.P = Point(0, 1, 0);
  pl.Dir1 = Vector3(0, 0, 1);
  pl.Dir2 = Vector3(1, 0, 0);
  IntersectionPoint i3 = intersection(l, pl); // true, I = (0, 1, 0), t = 1


  return (i1.exists == true  && i1.point == Point(0, 0, 0) && fequal(i1.time, 0.0f) && 
          i2.exists == false && true && fequal(i2.time, INFINITY<float>()) &&
          i3.exists == true  && i3.point == Point(0, 1, 0) && fequal(i3.time, 1.0f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectionLineSegmentPlane

std::string TestIntersectionLineSegmentPlane::GetName(void) const
{
  return "intersectionLineSegmentPlane";
}

bool TestIntersectionLineSegmentPlane::RunTest(void)
{
  LineSegment ls;
  Plane pl;
  ls.Start = Point(0, 0, 0);
  ls.Extend = Vector3(0, 2, 0);
  pl.P = ls.Start;
  pl.Dir1 = Vector3(1, 0, 0);
  pl.Dir2 = Vector3(0, 0, 1);
  IntersectionPoint i1 = intersection(ls, pl); // true, (0, 0, 0), t=0

  pl.P = Point(0, 1, 0);
  IntersectionPoint i2 = intersection(ls, pl); // true, (0, 1, 0), t=0.5

  pl.P = Point(0, 2, 0);
  IntersectionPoint i3 = intersection(ls, pl); // true, (0, 2, 0), t=1

  pl.P = Point(0, 4, 0);
  IntersectionPoint i4 = intersection(ls, pl); // false, t=2

  ls.Start = Point(2, 0, 0);
  ls.Extend = Point(0, 2, 0) - ls.Start;
  pl.P = Point(0, 0, 0);
  pl.Dir1 = Vector3(1, 1, 0);
  pl.Dir2 = Vector3(0, 0, 1);
  IntersectionPoint i5 = intersection(ls, pl); // true, (1, 1, 0), t=0.5

  pl.Dir1 = ls.Extend;
  IntersectionPoint i6 = intersection(ls, pl); // false, t=INFINITY

  return (i1.exists == true && i1.point == Point(0, 0, 0) && fequal(i1.time, 0.0f) &&
          i2.exists == true && i2.point == Point(0, 1, 0) && fequal(i2.time, 0.5f) && 
          i3.exists == true && i3.point == Point(0, 2, 0) && fequal(i3.time, 1.0f) &&
          i4.exists == false && fequal(i4.time, 2) &&
          i5.exists == true && i5.point == Point(1, 1, 0) && fequal(i5.time, 0.5f) &&
          i6.exists == false && fequal(i6.time, INFINITY<float>()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectionLineSegmentTriangle

std::string TestIntersectionLineSegmentTriangle::GetName(void) const
{
  return "intersectionLineSegmentTriangle";
}

bool TestIntersectionLineSegmentTriangle::RunTest(void)
{
  LineSegment ls;
  Triangle t;
  t.point[0] = Point(0, 1, 0);
  t.point[1] = Point(1, -1, 0);
  t.point[2] = Point(-1, -1, 0);
  ls.Start = Point(0, 0, 1);
  ls.Extend = Vector3(0, 0, -2);
  IntersectionPoint i1 = intersection(ls, t); // true, (0, 0, 0), t=0.5

  ls.Extend = Vector3(0, 0, 2);
  IntersectionPoint i2 = intersection(ls, t); // false, t=-0.5

  ls.Extend = (t.point[0] - ls.Start) * 4;
  IntersectionPoint i3 = intersection(ls, t); // false, (0, 1, 0), t=0.25

  ls.Start = Point(0, 0, 1);
  ls.Extend = t.point[1] - t.point[0];
  IntersectionPoint i4 = intersection(ls, t); // false, t=INF

  return (i1.exists == true && i1.point == Point(0, 0, 0) && fequal(i1.time, 0.5f) &&
          i2.exists == false && fequal(i2.time, -0.5f) &&
          i3.exists == false && i3.point == Point(0, 1, 0) && fequal(i3.time, 0.25f) &&
          i4.exists == false && fequal(i4.time, INFINITY<float>()));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectionRayPlane

std::string TestIntersectionRayPlane::GetName(void) const
{
  return "intersectionRayPlane";
}

bool TestIntersectionRayPlane::RunTest(void)
{
  Ray r;
  Plane pl;
  r.Start = Point(0, 0, -100);
  r.Dir = Vector3(0, 0, 1);
  pl.P = Point(0, 0, 0);
  pl.Dir1 = Vector3(1, 0, 0);
  pl.Dir2 = Vector3(0, 1, 0);
  IntersectionPoint i1 = intersection(r, pl); // true, (0, 0, 0), t=100

  r.Dir = (Point(0, 5, 0) - r.Start).Normalized();
  IntersectionPoint i2 = intersection(r, pl); // true, (0, 5, 0), t=(Point(0, 5, 0) - r.Start).Magnitude()

  r.Dir = Vector3(1, 0, 0);
  IntersectionPoint i3 = intersection(r, pl); // false, t=INF

  return (i1.exists == true && i1.point == Point(0, 0, 0) && fequal(i1.time, 100.0f) &&
          i2.exists == true && i2.point == Point(0, 5, 0) && fequal(i2.time, (Point(0, 5, 0) - r.Start).Magnitude()) &&
          i3.exists == false && fequal(i3.time, INFINITY<float>()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectionRayTriangle

std::string TestIntersectionRayTriangle::GetName(void) const
{
  return "intersectionRayTriangle";
}

bool TestIntersectionRayTriangle::RunTest(void)
{
  Ray r;
  Triangle t;
  t.point[0] = Point(0, 1, 0);
  t.point[1] = Point(1, -1, 0);
  t.point[2] = Point(-1, -1, 0);
  r.Start = Point(0, 0, -1);
  r.Dir = Vector3(0, 0, 1);
  IntersectionPoint i1 = intersection(r, t); // true, (0, 0, 0), t=1

  r.Start = Point(0, 0, -100);
  IntersectionPoint i2 = intersection(r, t); // true, (0, 0, 0), t=100

  r.Start = Point(0, 0, 1);
  r.Dir = t.point[0] - r.Start;
  IntersectionPoint i3 = intersection(r, t); // false, (0, 1, 0), t=1

  r.Dir = Vector3(0, 1, 0);
  IntersectionPoint i4 = intersection(r, t); // false, t=INF

  r.Dir = Point(-1.1, -1, 0) - r.Start;
  IntersectionPoint i5 = intersection(r, t); // false, (-1.1, -1, 0), t=1

  return (i1.exists == true && i1.point == Point(0, 0, 0) && fequal(i1.time, 1) &&
          i2.exists == true && i2.point == Point(0, 0, 0) && fequal(i2.time, 100) &&
          i3.exists == false && i3.point == Point(0, 1, 0) && fequal(i3.time, 1) &&
          i4.exists == false && fequal(i4.time, INFINITY<float>()) &&
          i5.exists == false && i5.point == Point(-1.1, -1, 0) && fequal(i5.time, 1));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectionRaySphere

std::string TestIntersectionRaySphere::GetName(void) const
{
  return "intersectionRaySphere";
}

bool TestIntersectionRaySphere::RunTest(void)
{
  Ray r;
  Sphere s;
  s.Center = Point(0, 0, 0);
  s.Radius = 1;
  r.Start = Point(2, 0, 0);
  r.Dir = Vector3(-1, 0, 0);
  IntersectionPoint i1 = intersection(r, s); // true, (1, 0, 0), t=1

  r.Start = Point(1, -1, 0);
  r.Dir = Vector3(0, 1, 0);
  IntersectionPoint i2 = intersection(r, s); // true, (1, 0, 0), t=1

  r.Dir = -r.Dir;
  IntersectionPoint i3 = intersection(r, s); // false, t=-1

  return (i1.exists == true && i1.point == Point(1, 0, 0) && fequal(i1.time, 1) &&
          i2.exists == true && i2.point == Point(1, 0, 0) && fequal(i2.time, 1) &&
          i3.exists == false && fequal(i3.time, -1.0f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectionRayAABB

std::string TestIntersectionRayAABB::GetName(void) const
{
  return "intersectionRayAABB";
}

bool TestIntersectionRayAABB::RunTest(void)
{
  Ray r;
  AABB_V2 bx;
  bx.Center = Point(0, 0, 0);
  bx.Extend = Vector3(1, 1, 1);
  r.Start = Point(0, 0, -10);
  r.Dir = Vector3(0, 0, 1);
  IntersectionPoint i1 = intersection(r, bx); // true (0, 0, -1), t=9

  r.Dir = (Point(1, 1, -1) - r.Start).Normalized();
  IntersectionPoint i2 = intersection(r, bx); // true (1, 1, -1), t=||(1, 1, -1) - (0, 0, -10)||

  r.Dir = Point(1.1, 1, -1) - r.Start;
  IntersectionPoint i3 = intersection(r, bx); // false, t=INF

  return (i1.exists == true && i1.point == Point(0, 0, -1) && fequal(i1.time, 9) &&
          i2.exists == true && i2.point == Point(1, 1, -1) && fequal(i2.time, (Point(1,1,-1)-Point(0,0,-10)).Magnitude()) &&
          i3.exists == false && fequal(i3.time, INFINITY<float>()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectionSphereSphere

std::string TestIntersectionSphereSphere::GetName(void) const
{
  return "intersectionSphereSphere";
}

bool TestIntersectionSphereSphere::RunTest(void)
{
  Sphere s1, s2;
  s1.Center = Point(-1, 0, 0);
  s1.Radius = 1;
  s2.Center = Point(0, 0, 0);
  s2.Radius = 1;
  IntersectionVolume i1 = intersection(s1, s2); // true

  s1.Center = Point(-2, 0, 0);
  IntersectionVolume i2 = intersection(s1, s2); // false

  return i1.exists && !i2.exists;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectionTriangleTriangle

std::string TestIntersectionTriangleTriangle::GetName(void) const
{
  return "intersectionTriangleTriangle";
}

bool TestIntersectionTriangleTriangle::RunTest(void)
{
  Triangle t1;
  Triangle t2;
  t1.point[0] = Point(0, 1, 0);
  t1.point[1] = Point(1, -1, 0);
  t1.point[2] = Point(-1, -1, 0);
  t2.point[0] = Point(0, 0, 1);
  t2.point[1] = Point(0, 1, -1);
  t2.point[2] = Point(0, -1, -1);
  IntersectionLine i1 = intersection(t1, t2); // true

  t2.point[0] = t1.point[0];
  t2.point[1] = t1.point[1];
  t2.point[2] = Point(0, 0, 5);
  IntersectionLine i2 = intersection(t1, t2); // false

  t2.point[0] = Point(0, 1, 1);
  t2.point[1] = Point(1, -1, 1);
  t2.point[2] = Point(-1, -1, 1);
  IntersectionLine i3 = intersection(t1, t2); // false

  return i1.exists && !i2.exists && !i3.exists;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// intersectAABBAABB

std::string TestIntersectionAabbAabb::GetName(void) const
{
  return "intersectAABBAABB";
}

bool TestIntersectionAabbAabb::RunTest(void)
{
  AABB_V2 bx1, bx2;
  bx1.Center = Point(-1, 0, 0);
  bx1.Extend = Vector3(1, 1, 1);
  bx2.Center = Point(1, 0, 0);
  bx2.Extend = Vector3(1, 1, 1);
  IntersectionVolume i1 = intersection(bx1, bx2); // false
  
  bx2.Center = Point(0, 0, 0);
  IntersectionVolume i2 = intersection(bx1, bx2); // true

  bx2.Center = bx1.Center;
  IntersectionVolume i3 = intersection(bx1, bx2); // true

  bx2.Center = Point(1.1f, 0, 0);
  IntersectionVolume i4 = intersection(bx1, bx2); // false

  bx2.Center = Point(2, 2, 2);
  IntersectionVolume i5 = intersection(bx1, bx2); // false
  
  bx2.Center = Point(2.01, 2.01, 2.01);
  IntersectionVolume i6 = intersection(bx1, bx2); // false

  return !i1.exists && i2.exists && i3.exists && !i4.exists && !i5.exists && !i6.exists;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// angleLineLine

std::string TestAngleLineLine::GetName(void) const
{
  return "angleLineLine";
}

bool TestAngleLineLine::RunTest(void)
{
  Line l1, l2;
  l1.Start = Point(0, 0, 0);
  l1.Direction = Vector3(1, 0, 0);
  l2.Start = l1.Start;
  l2.Direction = Vector3(0, 1, 0);
  float a1 = angle(l1, l2); // PI/2

  l2.Direction = Vector3(1, 1, 0);
  float a2 = angle(l1, l2); // PI/4

  l2.Direction = -l2.Direction;
  float a3 = angle(l1, l2); // 3PI/4

  return fequal(a1, PI/2) && fequal(a2, PI/4) && fequal(a3, 3*PI/4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// angleLinePLane

std::string TestAngleLinePlane::GetName(void) const
{
  return "angleLinePLane";
}

bool TestAngleLinePlane::RunTest(void)
{
  Line l;
  Plane pl;
  l.Start = Point(0, 0, 0);
  l.Direction = Vector3(0, 0, 1);
  pl.P = l.Start;
  pl.Dir1 = Vector3(0, 0, -1);
  pl.Dir2 = Vector3(1, 0, 0);
  float a1 = angle(l, pl); // 0

  l.Direction = Vector3(1, 0, 0);
  float a2 = angle(l, pl); // 0

  l.Direction = Vector3(1, 1, 0);
  float a3 = angle(l, pl); // PI/4

  return fequal(a1, 0) && fequal(a2, 0) && fequal(a3, PI/4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// anglePlanePlane

std::string TestAnglePlanePlane::GetName(void) const
{
  return "anglePlanePlane";
}

bool TestAnglePlanePlane::RunTest(void)
{
  Plane pl1, pl2;
  pl1.P = pl2.P = Point(0, 0, 0);
  pl1.Dir1 = pl2.Dir1 = Vector3(1, 0, 0);
  pl1.Dir2 = Vector3(0, 0, -1);
  pl2.Dir2 = Vector3(0, 1, 0);
  float a1 = angle(pl1, pl2); // PI/2 = 1.57079633
  bool ret1 = fequal(a1, 1.57079633f);

  pl2.Dir1 = Vector3(0, 0, -1);
  pl2.Dir2 = Vector3(1, 1, 0);
  float a2 = angle(pl1, pl2); // PI - PI/4 = 2.35619449
  bool ret2 = fequal(a2, 2.35619449f);

  pl1.P = Point(0, -1, 0);
  pl1.Dir1 = pl2.Dir1;
  pl1.Dir2 = pl2.Dir2;
  float a3 = angle(pl1, pl2); // 0
  bool ret3 = -0.001 < a3 && a3 < 0.001;

  return ret1 && ret2 && ret3;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// parallelLineLine

std::string TestParallelLineLine::GetName(void) const
{
  return "parallelLineLine";
}

bool TestParallelLineLine::RunTest(void)
{
  Line l1, l2;
  l1.Start = Point(0, 0, 0);
  l1.Direction = Vector3(0, 0, -1);
  l2.Start = Point(1, 0, 0);
  l2.Direction = Vector3(0, 0, -1);
  bool p1 = isParallel(l1, l2); // true

  l2.Direction = Vector3(1, 0, 0);
  bool p2 = isParallel(l1, l2); // false

  return p1 && !p2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// parallelLinePlane

std::string TestParallelLinePlane::GetName(void) const
{
  return "parallelLinePlane";
}

bool TestParallelLinePlane::RunTest(void)
{
  Line l;
  Plane pl;
  l.Start = Point(1, 0, 0);
  l.Direction = Vector3(0, 0, -1);
  pl.P = Point(0, 0, 0);
  pl.Dir1 = Vector3(0, 1, 0);
  pl.Dir2 = Vector3(0, 0 ,1);
  bool p1 = isParallel(l, pl); // true

  pl.Dir1 = Vector3(1, 0, 0);
  bool p2 = isParallel(l, pl); // true

  l.Direction = Vector3(1, 1, 0);
  bool p3 = isParallel(l, pl); // false

  return p1 && p2 && !p3;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// parallelPlanePlane

std::string TestParallelPlanePlane::GetName(void) const
{
  return "parallelPlanePlane";
}

bool TestParallelPlanePlane::RunTest(void)
{
  Plane pl1, pl2;
  pl1.P = Point(0, 0, 0);
  pl2.P = Point(1, 0, 0);
  pl1.Dir1 = pl2.Dir1 = Vector3(0, 0, -1);
  pl1.Dir2 = pl2.Dir2 = Vector3(0, 1, 0);
  bool p1 = isParallel(pl1, pl2); // true

  pl2.Dir2 = Vector3(1, 0, 0);
  bool p2 = isParallel(pl1, pl2); // false

  return p1 && !p2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// perpendicularLineLine

std::string TestPerpendicularLineLine::GetName(void) const
{
  return "perpendicularLineLine";
}

bool TestPerpendicularLineLine::RunTest(void)
{
  Line l1, l2;
  l1.Start = Point(0, 0, 0);
  l1.Direction = Vector3(0, 0, -1);
  l2.Start = Point(1, 0, 0);
  l2.Direction = Vector3(1, 0, 0);
  bool p1 = isPerpendicular(l1, l2); // true

  l2.Direction = l1.Direction;
  bool p2 = isPerpendicular(l1, l2); // false

  return p1 && !p2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// perpendicularLinePlane

std::string TestPerpendicularLinePlane::GetName(void) const
{
  return "perpendicularLinePlane";
}

bool TestPerpendicularLinePlane::RunTest(void)
{
  Line l;
  Plane pl;
  pl.P = Point(0, 0, 0);
  pl.Dir1 = Vector3(0, 0, -1);
  pl.Dir2 = Vector3(1, 0, 0);
  l.Start = Point(0, 1, 0);
  l.Direction = Vector3(0, 1, 0);
  bool p1 = isPerpendicular(l, pl); // true

  l.Direction = Vector3(1, 0, 0);
  bool p2 = isPerpendicular(l, pl); // false

  return p1 && !p2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// perpendicularPlanePlane

std::string TestPerpendicularPlanePlane::GetName(void) const
{
  return "perpendicularPlanePlane";
}

bool TestPerpendicularPlanePlane::RunTest(void)
{
  Plane pl1, pl2;
  pl1.P = Point(0, 0, 0);
  pl1.Dir1 = Vector3(0, 0, -1);
  pl1.Dir2 = Vector3(1, 0, 0);
  pl2.P = pl1.P;
  pl2.Dir1 = Vector3(1, 0, 0);
  pl2.Dir2 = Vector3(0, 1, 0);
  bool p1 = isPerpendicular(pl1, pl2); // true

  pl2.Dir1 = Vector3(1, 1, 0);
  pl2.Dir2 = Vector3(0, 0, -1);
  bool p2 = isPerpendicular(pl1, pl2); // false

  return p1 && !p2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// coplanarLinePLane

std::string TestCoplanarLinePlane::GetName(void) const
{
  return "coplanarLinePlane";
}

bool TestCoplanarLinePlane::RunTest(void)
{
  Line l;
  Plane pl;
  l.Start = Point(0, 0, 0);
  l.Direction = Vector3(1, 0, 0);
  pl.P = Point(0, 0, 0);
  pl.Dir1 = Vector3(0, 0, -1);
  pl.Dir2 = Vector3(1, 0, 0);
  bool c1 = isCoplanar(l, pl); // true

  l.Start = Point(0, 1, 0);
  bool c2 = isCoplanar(l, pl); // false

  return c1 && !c2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// coplanarPlanePlane

std::string TestCoplanarPlanePlane::GetName(void) const
{
  return "coplanarPlanePlane";
}

bool TestCoplanarPlanePlane::RunTest(void)
{
  Plane pl1, pl2;
  pl1.P = pl2.P = Point(0, 0, 0);
  pl1.Dir1 = pl2.Dir1 = Vector3(0, 0, -1);
  pl1.Dir2 = pl2.Dir2 = Vector3(0, 1, 0);
  bool c1 = isCoplanar(pl1, pl2); // true

  pl1.P = Point(1, 0, 0);
  bool c2 = isCoplanar(pl1, pl2); // false

  return c1 && !c2;
}

