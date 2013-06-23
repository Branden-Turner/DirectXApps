/*! ====================================================================================================================
  @file geometry.cpp

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.05.2012

  @brief 
    Geometry library

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "lamath.h"
#include "vector3.h"
#include "geometry.h"

namespace Grimelins {


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Projection


Point projection(Point const& point, Line const& line)
{
  Vector3 con = point - line.Start;
  float proj = con.Dot(line.Direction.Normalized());
  return line.Start + line.Direction.Normalized() * proj;
}

Point projection(Line const& line, Point const& point)
{
  return projection(point, line);
}


Point projection(Point const& point, Plane const& plane)
{
  Vector3 n = plane.Dir1.Cross(plane.Dir2).Normalized();
  return point - n * (point - plane.P).Dot(n);
}

Point projection(Plane const& plane, Point const& point)
{
  return projection(point, plane);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Distance

float distance(Point const& p1, Point const& p2)
{
  return (p1 - p2).Magnitude();
}


float distance(Point const& p, LineSegment const& ls)
{
  Line l;
  l.Start = ls.Start;
  l.Direction = ls.Extend.Normalized();

  Point pp = projection(p, l);
  float t = (pp - ls.Start).Dot(ls.Extend / ls.Extend.MagnitudeSq());
  t = CLAMPED(0.0f, 1.0f, t);
  
  Point ppp = ls.Start + (ls.Extend) * t;
  return distance(ppp, p);
}

float distance(LineSegment const& ls, Point const& p)
{
  return distance(p, ls);
}


float distance(Point const& p, Ray const& ray)
{
  Line L;
  L.Start = ray.Start;
  L.Direction = ray.Dir.Normalized();

  Point pp = projection(p, L);

  float t = (pp - L.Start).Dot(L.Direction / L.Direction.MagnitudeSq());

  if (t < 0.0f)
    t = 0.0f;

  pp = L.Start + (L.Direction) * t;
  return distance(p, pp);
}

float distance(Ray const& ray, Point const& p)
{
  return distance(p, ray);
}


float distance(Point const& p, Line const& line)
{
  Point pp = projection(p, line);
  return distance(p, pp);
}

float distance(Line const& line, Point const& p)
{
  return distance(p, line);
}


float distance(Point const& p, Plane const& plane)
{
  Point PP = projection(p, plane);
  return distance(p, PP);
}

float distance(Plane const& plane, Point const& p)
{
  return distance(p, plane);
}


float distance(Point const& p, Triangle const& triangle)
{
  Plane pl;
  pl.P = triangle.point[1];
  pl.Dir1 = (triangle.point[0] - triangle.point[1]).Normalized();
  pl.Dir2 = (triangle.point[3] - triangle.point[2]).Normalized();

  Point pp = projection(p, pl);

  LineSegment edge[3];

  for (int i = 0; i < 3; ++i)
  {
    edge[i].Start = triangle.point[i];
    edge[i].Extend = triangle.point[(i+1)%3] - triangle.point[i];
  }

  float distances[3] = {0.0f};
  bool inside = true;

  for (int i = 0; i < 3; ++i)
  {
    float proj = (pp - triangle.point[i]).Dot(edge[i].Extend.Normalized());

    if (proj > 0.0f)
    {
      LineSegment e = { triangle.point[i], edge[i].Extend };
      distances[i] = distance(pp, e);
    }
    else 
      inside = false;
  }

  if (inside) // Point inside triangle
  {
    return distance(pp, p);
  }
  else
  {
    float minVal = distances[0];
    int minI = 0;

    for (int i = 1; i < 3; ++i)
    {
      if (distances[i-1] < distances[i])
      {
        minVal = distances[i];
        minI = i;
      }
    }

    return distance(p, edge[minI]);
  }

}

float distance(Triangle const& triangle, Point const& p)
{
  return distance(p, triangle);
}


float distance(Point const& p, Sphere const& sphere)
{
  return (sphere.Center - p).Magnitude() - sphere.Radius;
}

float distance(Sphere const& sphere, Point const& p)
{
  return distance(p, sphere);
}


float distance(Point const& p, AABB_V2 const& aabb)
{
  Point Pmin = aabb.Center - aabb.Extend;
  Point Pmax = aabb.Center + aabb.Extend;
  Point pp;
  pp.x = CLAMPED(Pmin.x, Pmax.x, p.x);
  pp.y = CLAMPED(Pmin.y, Pmax.y, p.y);
  pp.z = CLAMPED(Pmin.z, Pmax.z, p.z);
  
  return distance(p, pp);
}

float distance(AABB_V2 const& aabb, Point const& p)
{
  return distance(p, aabb);
}


float distance(Line const& l1, Line const& l2)
{
  float a = l1.Direction.Dot(l1.Direction);
  float b = l1.Direction.Dot(l2.Direction);
  float c = l2.Direction.Dot(l2.Direction);
  float d = l1.Direction.Dot(l1.Start - l2.Start);
  float e = l2.Direction.Dot(l1.Start - l2.Start);

  float denom = a * c - b * b;
  float s, t;

  if (fequal(denom, 0.0f))
  {
    s = 0.0f;
    t = d / b;
  }
  else
  {
    s = (b * e - c * d) / denom;
    t = (a * e - b * d) / denom;
  }

  Point p1 = l1.Start + l1.Direction * s;
  Point p2 = l2.Start + l2.Direction * t;

  return distance(p1, p2);
}


float distance(Line const& line, Sphere const& sphere)
{
  return distance(sphere.Center, line) - sphere.Radius;
}

float distance(Sphere const& sphere, Line const& line)
{
  return distance(line, sphere);
}


float distance(LineSegment const& ls1, LineSegment const& ls2)
{
  float a = ls1.Extend.Dot(ls1.Extend);
  float b = ls1.Extend.Dot(ls2.Extend);
  float c = ls2.Extend.Dot(ls2.Extend);
  float d = ls1.Extend.Dot(ls1.Start - ls2.Start);
  float e = ls2.Extend.Dot(ls1.Start - ls2.Start);

  float denom = a * c - b * b;
  float s, t;

  if (fequal(denom, 0.0f))
  {
    s = 0.0f;
  }
  else
  {
    s = (b * e - c * d) / denom;
  }

  s = CLAMPED(0.0f, 1.0f, s);
  t = (e + s * b) / c;

  if (t < 0.0f)
  {
    t = 0.0f;
    s = -d / a;
    s = CLAMPED(0.0f, 1.0f, s);
  }
  else if (t > 1.0f)
  {
    t = 1.0f;
    s = (b - d) / a;
    s = CLAMPED(0.0f, 1.0f, s);
  }

  Point p1 = ls1.Start + ls1.Extend * s;
  Point p2 = ls2.Start + ls2.Extend * t;

  return distance(p1, p2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Containment

bool isContained(Point const& point, LineSegment const& linesegment)
{
  Line l;
  l.Start = linesegment.Start;;
  l.Direction = linesegment.Extend.Normalized();

  if (isContained(point, l))
  {
    float t = (point - linesegment.Start).Dot(linesegment.Extend);
    t /= linesegment.Extend.MagnitudeSq();

    return (0.0f < t && t < 1.0f);
  }

  return false;
}

bool isContained(LineSegment const& linesegment, Point const& point)
{
  return isContained(point, linesegment);
}


bool isContained(Point const& point, Ray const& ray)
{
  Line l;
  l.Start = ray.Start;
  l.Direction = ray.Dir;

  if (isContained(point, l))
  {
    return (point - ray.Start).Dot(ray.Dir) > 0.0f;
  }

  return false;
}

bool isContained(Ray const& ray, Point const& point)
{
  return isContained(point, ray);
}


bool isContained(Point const& point, Line const& line)
{
  static Vector3 zero(0.0f, 0.0f, 0.0f);
  Vector3 cross = (point - line.Start).Cross(line.Direction);
  
  return cross == zero;
}

bool isContained(Line const& line, Point const& point)
{
  return isContained(point, line);
}


bool isContained(Point const& point, Plane const& plane)
{
  return fequal((point - plane.P).Dot(plane.Dir1.Cross(plane.Dir2)), 0.0f);
}

bool isContained(Plane const& plane, Point const& point)
{
  return isContained(point, plane);
}


bool isContained(Point const& point, Triangle const& triangle)
{
  Vector3 v1 = triangle.point[1] - triangle.point[0];
  Vector3 v2 = triangle.point[2] - triangle.point[0];

  Plane pl;
  pl.P = triangle.point[0];
  pl.Dir1 = v1;
  pl.Dir2 = v2;

#if defined(DEBUG) | defined(_DEBUG) // make sure edges aren't parallel (must not happen for triangle)
  Vector3 cross = pl.Dir1.Cross(pl.Dir2);
  static Vector3 zero(0.0f, 0.0f, 0.0f);
  assert(cross != zero);
#endif

  if (isContained(point, pl))
  {
    float a = v1.Dot(point - triangle.point[0]);
    float b = v2.Dot(point - triangle.point[0]);
    float c = v1.MagnitudeSq();
    float d = v2.MagnitudeSq();
    float e = v1.Dot(v2);
    float denom = c * d - e * e;
    
//     float s1 = (d * a - e * b) / denom;
//     float t1 = (-e * a + c * b) / denom;

    float s = ((d * a) - (e * b)) / denom;
    float t = ((c * b) - (e * a)) / denom;
      
    return (0.0f < s && s < 1.0f) && (0.0f < t && t < 1.0f) && (0.0f < 1 - s - t) && (1 - s - t < 1.0f);
  }

  return false;
}

bool isContained(Triangle const& triangle, Point const& point)
{
  return isContained(point, triangle);
}


bool isContained(Point const& point, Sphere const& sphere)
{
  return (sphere.Center - point).MagnitudeSq() <= sphere.Radius * sphere.Radius;
}

bool isContained(Sphere const& sphere, Point const& point)
{
  return isContained(point, sphere);
}


bool isContained(Point const& point, AABB_V2 const& aabb)
{
  Point Pmax = aabb.Center + aabb.Extend;
  Point Pmin = aabb.Center - aabb.Extend;

  return (Pmin.x <= point.x && point.x <= Pmax.x) &&
         (Pmin.y <= point.y && point.y <= Pmax.y) &&
         (Pmin.z <= point.z && point.z <= Pmax.z);
}

bool isContained(AABB_V2 const& aabb, Point const& point)
{
  return isContained(point, aabb);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Intersection

IntersectionPoint intersection(Line const& line, Plane const& plane)
{
  IntersectionPoint ret;

  if (!isParallel(line, plane))
  {
    ret.exists = true;

    Vector3 n = plane.Dir1.Cross(plane.Dir2).Normalize();
    ret.time = (n.Dot(plane.P) - n.Dot(line.Start)) / n.Dot(line.Direction);

    ret.point = line.Start + line.Direction * ret.time;
  }

  return ret;
}

IntersectionPoint intersection(Plane const& plane, Line const& line)
{
  return intersection(line, plane);
}


IntersectionPoint intersection(LineSegment const& ls, Plane const& plane)
{
  IntersectionPoint ret;
  Line l = { ls.Start, ls.Extend.Normalized() };

  if (!isParallel(l, plane))
  {
    Vector3 n = plane.Dir1.Cross(plane.Dir2).Normalize();
    ret.time = (n.Dot(plane.P) - n.Dot(ls.Start)) / (n.Dot(ls.Extend));
    
    if (0.0f <= ret.time && ret.time <= 1.0f)
    {
      ret.exists = true;
      ret.point = ls.Start + ls.Extend * ret.time;
    }
  }

  return ret;
}

IntersectionPoint intersection(Plane const& plane, LineSegment const& ls)
{
  return intersection(ls, plane);
}


IntersectionPoint intersection(LineSegment const& ls, Triangle const& triangle)
{
  IntersectionPoint ret;
  
  Plane pl;
  pl.P = triangle.point[0];
  pl.Dir1 = triangle.point[1] - triangle.point[0];
  pl.Dir2 = triangle.point[2] - triangle.point[0];
  
  Line l = { ls.Start, ls.Extend.Normalized() };

  if (!isParallel(l, pl))
  {
    Vector3 plN = pl.Dir1.Cross(pl.Dir2).Normalize();
    ret.time = (-plN.Dot(triangle.point[0]) - plN.Dot(ls.Start)) / plN.Dot(ls.Extend);

    if (0.0f <= ret.time && ret.time <= 1.0f)
    {
      ret.point = ls.Start + ls.Extend * ret.time;
      ret.exists = isContained(ret.point, triangle);
    }
  }

  return ret;
}

IntersectionPoint intersection(Triangle const& triangle, LineSegment const& ls)
{
  return intersection(ls, triangle);
}


IntersectionPoint intersection(Ray const& ray, Plane const& plane)
{
  IntersectionPoint ret;
  Line l = { ray.Start, ray.Dir };

  if (!isParallel(l, plane))
  {
    ret = intersection(l, plane);

    if (0.0f <= ret.time)
    {
      ret.point = ray.Start + ray.Dir * ret.time;
      ret.exists = true;
    }
  }

  return ret;
}

IntersectionPoint intersection(Plane const& plane, Ray const& ray)
{
  return intersection(ray, plane);
}


IntersectionPoint intersection(Ray const& ray, Triangle const& triangle)
{
  IntersectionPoint ret;
  Plane pl;
  pl.P = triangle.point[0];
  pl.Dir1 = triangle.point[1] - triangle.point[0];
  pl.Dir2 = triangle.point[2] - triangle.point[0];

  Line l = { ray.Start, ray.Dir };

  if (!isParallel(l, pl))
  {
    Vector3 plN = pl.Dir1.Cross(pl.Dir2).Normalize();
    ret.time = (-plN.Dot(pl.P) - plN.Dot(ray.Start)) / plN.Dot(ray.Dir);

    if (0.0f <= ret.time)
    {
      ret.point = ray.Start + ray.Dir * ret.time;
      ret.exists = isContained(ret.point, triangle);
    }
  }

  return ret;
}

IntersectionPoint intersection(Triangle const& triangle, Ray const& ray)
{
  return intersection(ray, triangle);
}


IntersectionPoint intersection(Ray const& ray, Sphere const& sphere)
{
  IntersectionPoint ret;

  float a = ray.Dir.MagnitudeSq();
  float b = 2 * ray.Dir.Dot(ray.Start - sphere.Center);
  float c = (ray.Start - sphere.Center).MagnitudeSq() - sphere.Radius * sphere.Radius;

  float discr = b * b - 4 * a * c;

  if (discr < 0.0f)
    return ret;
  else if (discr == 0.0f)
  {
    ret.time = -b / (2 * a);

    if (0.0f <= ret.time)
    {
      ret.exists = true;
      ret.point = ray.Start + ray.Dir * ret.time;
    }
  }
  else
  {
    ret.time = (-b - sqrt(discr)) / (2 * a); // Only get entry intersection

    if (0.0f <= ret.time)
    {
      ret.exists = true;
      ret.point = ray.Start + ray.Dir * ret.time;
    }
  }

  return ret;
}

IntersectionPoint intersection(Sphere const& sphere, Ray const& ray)
{
  return intersection(ray, sphere);
}


class ClosestIntersection
{
public:
  ClosestIntersection(Point p) : mP(p) {}
  bool operator()(IntersectionPoint const& i1, IntersectionPoint const& i2)
  {
    return distance(mP, i1.point) < distance(mP, i2.point);
  }

private:
  Point mP;
};

IntersectionPoint intersection(Ray const& ray, AABB_V2 const& aabb)
{
  IntersectionPoint ret;

  Vector3 x(1, 0, 0);
  Vector3 y(0, 1, 0);
  Vector3 z(0, 0, 1);
  Point pMax = aabb.Center + aabb.Extend;
  Point pMin = aabb.Center - aabb.Extend;

  Plane planes[6];
  planes[0].P = pMax;
  planes[0].Dir1 = x;
  planes[0].Dir2 = y;

  planes[1].P = pMax;
  planes[1].Dir1 = z;
  planes[1].Dir2 = y;

  planes[2].P = pMax;
  planes[2].Dir1 = x;
  planes[2].Dir2 = z;

  planes[3].P = pMin;
  planes[3].Dir1 = x;
  planes[3].Dir2 = y;

  planes[4].P = pMin;
  planes[4].Dir1 = z;
  planes[4].Dir2 = y;

  planes[5].P = pMin;
  planes[5].Dir1 = x;
  planes[5].Dir2 = z;

  std::vector<IntersectionPoint> possibilities;

  for (int i = 0; i < 6; ++i)
  {
    ret = intersection(ray, planes[i]);

    if (ret.exists && fequal(distance(ret.point, aabb), 0.0f))
      possibilities.push_back(ret);
  }

  if (possibilities.empty())
    return IntersectionPoint();
  else
    return *std::min_element(possibilities.begin(), possibilities.end(), ClosestIntersection(ray.Start));
}

IntersectionPoint intersection(AABB_V2 const& aabb, Ray const& ray)
{
  return intersection(ray, aabb);
}


IntersectionVolume intersection(Sphere const& sphere1, Sphere const& sphere2)
{
  IntersectionVolume ret;
  Vector3 dir = sphere1.Center - sphere2.Center;
  ret.exists = dir.MagnitudeSq() - sphere1.Radius * sphere1.Radius - sphere2.Radius * sphere2.Radius <= 0.0f;

  return ret;
}


IntersectionLine intersection(Triangle const& t1, Triangle const& t2)
{
  IntersectionLine ret;
  
  LineSegment ls1;
  ls1.Start = t1.point[0];
  ls1.Extend = t1.point[1] - t1.point[0];
  ret.exists = intersection(ls1, t2).exists;
  if (ret.exists)
    return ret;

  LineSegment ls2;
  ls2.Start = t1.point[1];
  ls2.Extend = t1.point[2] - t1.point[1];
  ret.exists = intersection(ls2, t2).exists;
  if (ret.exists)
    return ret;

  LineSegment ls3;
  ls3.Start = t1.point[2];
  ls3.Extend = t1.point[0] - t1.point[2];
  ret.exists = intersection(ls3, t2).exists;
  if (ret.exists)
    return ret;

  LineSegment ls4;
  ls4.Start = t2.point[0];
  ls4.Extend = t2.point[1] - t2.point[0];
  ret.exists = intersection(ls4, t1).exists;
  if (ret.exists)
    return ret;

  LineSegment ls5;
  ls5.Start = t2.point[1];
  ls5.Extend = t2.point[2] - t2.point[1];
  ret.exists = intersection(ls5, t1).exists;
  if (ret.exists)
    return ret;

  LineSegment ls6;
  ls6.Start = t2.point[2];
  ls6.Extend = t2.point[0] - t2.point[2];
  ret.exists = intersection(ls6, t1).exists;
  if (ret.exists)
    return ret;

  return ret; // false
}


IntersectionVolume intersection(AABB_V2 const& aabb1, AABB_V2 const& aabb2)
{
  IntersectionVolume ret;

  Vector3 cDist = aabb2.Center - aabb1.Center;
  Vector3 eSum = aabb1.Extend + aabb2.Extend;

  ret.exists = cDist.x < eSum.x && cDist.y < eSum.y && cDist.z < eSum.z;

  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Angle

float angle(Line const& l1, Line const& l2)
{
  return std::acos(l1.Direction.Normalized().Dot(l2.Direction.Normalized()));
}


float angle(Line const& line, Plane const& plane)
{
  return -(PI / 2 - std::acos(line.Direction.Normalized().Dot(plane.Dir1.Cross(plane.Dir2).Normalized())));
}

float angle(Plane const& plane, Line const& line)
{
  return angle(line, plane);
}


float angle(Plane const& pl1, Plane const& pl2)
{
  return std::acos(pl1.Dir1.Cross(pl1.Dir2).Normalized().Dot(pl2.Dir1.Cross(pl2.Dir2).Normalized()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parallelism

bool isParallel(Line const& l1, Line const& l2)
{
  return fequal(l1.Direction.Normalized().Dot(l2.Direction.Normalized()), 1.0f);
}


bool isParallel(Line const& line, Plane const& plane)
{
  Vector3 n = plane.Dir1.Cross(plane.Dir2);
  float proj = line.Direction.Dot(n);
  return fequal(proj, 0.0f);
}

bool isParallel(Plane const& plane, Line const& line)
{
  return isParallel(line, plane);
}


bool isParallel(Plane const& pl1, Plane const& pl2)
{
  Vector3 n1 = pl1.Dir1.Cross(pl1.Dir2).Normalized();
  Vector3 n2 = pl2.Dir1.Cross(pl2.Dir2).Normalized();
  return fequal(n1.Dot(n2), 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Perpendicularity

bool isPerpendicular(Line const& l1, Line const& l2)
{
  return fequal(l1.Direction.Dot(l2.Direction), 0.0f);
}


bool isPerpendicular(Line const& line, Plane const& plane)
{
  Vector3 n = plane.Dir1.Cross(plane.Dir2).Normalized();
  float proj = n.Dot(line.Direction.Normalized());
  return fequal(fabs(proj), 1.0f);
}

bool isPerpendicular(Plane const& plane, Line const& line)
{
  return isPerpendicular(line, plane);
}


bool isPerpendicular(Plane const& pl1, Plane const& pl2)
{
  Vector3 n1 = pl1.Dir1.Cross(pl1.Dir2);
  Vector3 n2 = pl2.Dir1.Cross(pl2.Dir2);
  return fequal(n1.Dot(n2), 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Coplanar

bool isCoplanar(Line const& line, Plane const& plane)
{
  return isParallel(line, plane) && isContained(line.Start, plane);
}

bool isCoplanar(Plane const& plane, Line const& line)
{
  return isCoplanar(line, plane);
}


bool isCoplanar(Plane const& pl1, Plane const& pl2)
{
  return isParallel(pl1, pl2) && (isContained(pl1.P, pl2) || isContained(pl2.P, pl1));
}

} // namespace Grimelins
