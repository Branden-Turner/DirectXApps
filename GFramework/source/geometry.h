#ifndef GEOM_H
#define GEOM_H

namespace cs350
{
  //Forward declarations
  struct Object;
  class KDTree;
  class DBVH;
  class DebugDraw;
  typedef vec3 Pt;
  typedef vec3 Vc;

  typedef std::vector<Object> ObjectList;

  enum SplitAxis
  {
    XAxis,
    YAxis,
    ZAxis,
    NumAxes
  };

  //A line can be a segment represented by two points, or a ray, or just a line
  struct Line
  {
    Line(const Pt& _p1 = Pt(0.0f,0.0f,0.0f), const Pt& _p2 = Pt(0.0f,0.0f,0.0f));
    
    Pt p1, p2, p;
    Vc v;
  };

  typedef Line Ray;
  typedef Line LineSegment;

  //Can be defined just like a plane
  struct Triangle
  {
    Triangle(const vec3& vone = vec3(0.0f,0.0f,0.0f), const vec3& vtwo = vec3(0.0f,0.0f,0.0f), const vec3& vthree = vec3(0.0f,0.0f,0.0f));
    Triangle(const Vertex_Pos_Clr_Nrm_Tex& v1, const Vertex_Pos_Clr_Nrm_Tex& v2, const Vertex_Pos_Clr_Nrm_Tex& v3);

    Pt p1; Pt p2; Pt p3;
    Vc v1; Vc v2;
    Pt q; Vc n;
    Vertex_Pos_Clr_Nrm_Tex vert1, vert2, vert3;
   
  };

  typedef std::vector<Triangle> TriangleList;

  //3 points, 2 vectors, or a point and a normal
  //it also has a set of equation constants
  struct Plane
  {
    Plane( const Triangle& tri = Triangle() );
    Plane( const Pt& _q, const Vc& _n );
    Plane( const Pt& _q, const Vc& _v1, const Vc& _v2);

    Pt p1; Pt p2; Pt p3;
    Vc v1; Vc v2;
    Pt q; Vc n;
   

    vec4 eqn;
  };

  struct Sphere
  {
    Pt center;
    float radius;
  };

  struct AABB
  {
    AABB(Pt& c = Pt(0.0f,0.0f,0.0f), Vc& e = Vc(0.5f, 0.5f, 0.5f));
    Pt center;
    vec3 extents;
  };

  struct TRange
  {
    TRange(float min = 0.0f, float max = 0.0f);
    float tMin, tMax;
  };

  struct Intersection
  {
    Intersection(const Pt& _p = Pt(0.0f,0.0f,0.0f), float _t = -1.0f, bool inter = false);
    Pt p;
    float t;
    bool intersecting;
  };

  struct MPortal
  {
    MPortal(Pt& center = Pt(0,0,0), Pt& pp1 = Pt(0,0,0), Pt& pp2 = Pt(0,0,0), Pt& pp3 = Pt(0,0,0));
    Pt c, p1, p2, p3;
  };

  class GeometryLibrary
  {
    public:
      GeometryLibrary(DebugDraw* dbg, ID3D10Device* dev);
      ~GeometryLibrary();
      void addTriToScene(const Triangle& tri);
      void addObjectToScene( const Object& obj );
      void buildKDTree();
      KDTree* getKDTree();

      void buildDBVH();
      DBVH* getDBVH();

      //     Projection
      Pt projPtToLine( const Pt& p, const Line& l);
      Pt projPtToPlane( const Pt& p, const Plane& pl);

      //     Distance
      float distPts( const Pt& p1, const Pt& p2);
      float distPtLs( const Pt& p, const LineSegment& ls);
      float distPtRay( const Pt& p, const Ray& r);
      float distPtLine( const Pt& p, const Line& l);
      float distPtPlane( const Pt& p, const Plane& pl);
      float distPtTri( const Pt& p, const Triangle& tri);
      float distPtSphere( const Pt& p, const Sphere& sp);
      float distPtAABB( const Pt& p, const AABB& box);
      float distLines( const Line& l1, const Line& l2 );
      float distLineSphere( const Line& l1, const Sphere& sp );
      float distLineSegs( const LineSegment& l1, const LineSegment& l2 );

      //     Containment
      bool contLineSeg( const Pt& p, const LineSegment& ls );
      bool contRay( const Pt& p, const Ray& r );
      bool contLine( const Pt& p, const Line& l );
      bool contPlane( const Pt& p, const Plane& pl );
      bool contTri( const Pt& p, const Triangle& tri );
      bool contSphere( const Pt& p, const Sphere& sp );
      bool contAABB( const Pt& p, const AABB& box );

      //     Intersection
      Intersection intSegPlane( const LineSegment& ls, const Plane& pl );
      Intersection intSegTri( const LineSegment& ls, const Triangle& tri );
      Intersection intRayPlane( const Ray& r, const Plane& pl );
      Intersection intRayTri( const Ray& r, const Triangle& tri );
      Intersection intRaySphere( const Ray& r, const Sphere& sp );
      Intersection intRayAABB( const Ray& r, const AABB& box );
      Intersection intLinePlane( const Line& l, const Plane& pl );
      Intersection intSpheres( const Sphere& sp1, const Sphere& sp2 );
      Intersection intTris( const Triangle& t1, const Triangle& t2 );
      Intersection intAABBs( const AABB& box1, const AABB& box2 );


      //     Angle Between
      float angleLines( const Line& l1, const Line& l2 );
      float angleLinePlane( const Line& l, const Plane& pl );
      float anglePlanes( const Plane& pl1, const Plane& pl2 );


      //     Parallelism
      bool parallelLines( const Line& l1, const Line& l2 );
      bool parallelLinePlane( const Line& l, const Plane& pl );
      bool parallelPlanes( const Plane& pl1, const Plane& pl2 );


      //     Perpendicularity
      bool perpLines( const Line& l1, const Line& l2 );
      bool perpLinePlane( const Line& l, const Plane& pl );
      bool perpPlanes( const Plane& pl1, const Plane& pl2 );


      //     Coplanar
      bool copLinePlane( const Line& l, const Plane& pl );
      bool copPlanes( const Plane& pl1, const Plane& pl2 );

      //      AABB maths
      static float minPos( const Triangle& tri, SplitAxis axis );
      static float maxPos( const Triangle& tri, SplitAxis axis );
      static void splitBox( AABB& l, AABB& r, const AABB& p, float pos, SplitAxis axis );
      static AABB buildAABBFromTriList( TriangleList& triList );
      static float perimeter( const AABB& box );
      static AABB mergeAABBs( const AABB& b1, const AABB& b2 );

      //      MPR
      MPortal initialPortal(Object& obj1, Object& obj2);
      Pt      supportFn(vec3& sv, Object& obj);
      void    discovery(MPortal& portal, Object& obj1, Object& obj2);
      void    refinement(MPortal& portal, Object& obj1, Object& obj2);
      
    private:
      TRange cutRayWithHalfSpace(const Ray& l, const Plane& halfspace);
      TRange intTRanges(const TRange& t1, const TRange& t2);
      TriangleList staticSceneTris;
      ObjectList sceneObjects;
      KDTree* sceneKdTree;
      DBVH* sceneDBVH;
      DebugDraw* dbgDraw;
      ID3D10Device* device;
  };


}

#endif