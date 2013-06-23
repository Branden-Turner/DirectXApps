/*! ====================================================================================================================
  @file geometry.h

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.05.2012

  @brief 
    Geometry library

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#include "vector3.h"

namespace Grimelins {


typedef Vector3 Point;


struct Line
{
  Point Start;
  Vector3 Direction;
};

struct LineSegment
{
  Point Start;
  Vector3 Extend;
};

struct Ray
{
  Point Start;
  Vector3 Dir;
};

struct Plane
{
  Point P;
  Vector3 Dir1;
  Vector3 Dir2;
};

struct Triangle
{
  Point point[3];
};

struct Sphere
{
  Point Center;
  float Radius;
};

struct AABB_V2
{
  Point Center;
  Vector3 Extend;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Projection

Point projection(Point const& point, Line const& line);
Point projection(Line const& line, Point const& point);

Point projection(Point const& point, Plane const& plane);
Point projection(Plane const& plane, Point const& point);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Distance

float distance(Point const& p1, Point const& p2);

float distance(Point const& p, LineSegment const& ls);
float distance(LineSegment const& ls, Point const& p);

float distance(Point const& p, Ray const& ray);
float distance(Ray const& ray, Point const& p);

float distance(Point const& p, Line const& line);
float distance(Line const& line, Point const& p);

float distance(Point const& p, Plane const& plane);
float distance(Plane const& plane, Point const& p);

float distance(Point const& p, Triangle const& triangle);
float distance(Triangle const& triangle, Point const& p);

float distance(Point const& p, Sphere const& sphere);
float distance(Sphere const& sphere, Point const& p);

float distance(Point const& p, AABB_V2 const& aabb);
float distance(AABB_V2 const& aabb, Point const& p);

float distance(Line const& l1, Line const& l2);

float distance(Line const& line, Sphere const& sphere);
float distance(Sphere const& sphere, Line const& line);

float distance(LineSegment const& ls1, LineSegment const& ls2);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Containment

bool isContained(Point const& point, LineSegment const& linesegment);
bool isContained(LineSegment const& linesegment, Point const& point);

bool isContained(Point const& point, Ray const& ray);
bool isContained(Ray const& ray, Point const& point);

bool isContained(Point const& point, Line const& line);
bool isContained(Line const& line, Point const& point);

bool isContained(Point const& point, Plane const& plane);
bool isContained(Plane const& plane, Point const& point);

bool isContained(Point const& point, Triangle const& triangle);
bool isContained(Triangle const& triangle, Point const& point);

bool isContained(Point const& point, Sphere const& sphere);
bool isContained(Sphere const& sphere, Point const& point);

bool isContained(Point const& point, AABB_V2 const& aabb);
bool isContained(AABB_V2 const& aabb, Point const& point);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Intersection

struct IntersectionPoint // Intersections for primitives with directions
{
  bool  exists;
  Point point;
  float time;

  IntersectionPoint(void) : exists(false), time(INFINITY<float>()) {}
};

struct IntersectionLine
{
  bool exists;
  //TODO: add line

  IntersectionLine(void) : exists(false) {}
};

struct IntersectionVolume
{
  bool exists;
  //TODO: add volume

  IntersectionVolume(void) : exists(false) {}
};


IntersectionPoint intersection(Line const& line, Plane const& plane);
IntersectionPoint intersection(Plane const& plane, Line const& line);

IntersectionPoint intersection(LineSegment const& ls, Plane const& plane);
IntersectionPoint intersection(Plane const& plane, LineSegment const& ls);

IntersectionPoint intersection(LineSegment const& ls, Triangle const& triangle);
IntersectionPoint intersection(Triangle const& triangle, LineSegment const& ls);

IntersectionPoint intersection(Ray const& ray, Plane const& plane);
IntersectionPoint intersection(Plane const& plane, Ray const& ray);

IntersectionPoint intersection(Ray const& ray, Triangle const& triangle);
IntersectionPoint intersection(Triangle const& triangle, Ray const& ray);

IntersectionPoint intersection(Ray const& ray, Sphere const& sphere);
IntersectionPoint intersection(Sphere const& sphere, Ray const& ray);

IntersectionPoint intersection(Ray const& ray, AABB_V2 const& aabb);
IntersectionPoint intersection(AABB_V2 const& aabb, Ray const& ray);

IntersectionVolume intersection(Sphere const& sphere1, Sphere const& sphere2);

IntersectionLine intersection(Triangle const& t1, Triangle const& t2);

IntersectionVolume intersection(AABB_V2 const& aabb1, AABB_V2 const& aabb2);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Angle

float angle(Line const& l1, Line const& l2);

float angle(Line const& line, Plane const& plane);
float angle(Plane const& plane, Line const& line);

float angle(Plane const& pl1, Plane const& pl2);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parallelism

bool isParallel(Line const& l1, Line const& l2);

bool isParallel(Line const& line, Plane const& plane);
bool isParallel(Plane const& plane, Line const& line);

bool isParallel(Plane const& pl1, Plane const& pl2);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Perpendicularity

bool isPerpendicular(Line const& l1, Line const& l2);

bool isPerpendicular(Line const& line, Plane const& plane);
bool isPerpendicular(Plane const& plane, Line const& line);

bool isPerpendicular(Plane const& pl1, Plane const& pl2);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Coplanar

bool isCoplanar(Line const& line, Plane const& plane);
bool isCoplanar(Plane const& plane, Line const& line);

bool isCoplanar(Plane const& pl1, Plane const& pl2);

} // namespace Grimelins
