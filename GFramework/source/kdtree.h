#ifndef KDTREE_H
#define KDTREE_H

namespace cs350
{
  class DebugDraw;

  struct KDNode
  {
    KDNode(AABB& box, TriangleList& tris, SplitAxis axis, float pos, KDNode* p);

    //Pointers to other nodes (left, right, and parent)
    KDNode* childLeft;
    KDNode*childRight;
    KDNode* parent;

    //All the triangles of a particular node 
    AABB boundingBox;

    //Keep track of the triangles this node contains as well as splitting axis and position
    TriangleList triangles;
    SplitAxis splitAxis;
    float splitPos;    
  };

  enum KDEventType
  {
    Starting,
    Coplanar,
    Ending,
    KDTypeNum
  };

  struct KDEvent
  {
    KDEvent( KDEventType ty, Triangle* tri, float pos, AABB* box );

    bool operator<(const KDEvent& rhs);
    KDEventType type;
    Triangle*   eventTri;

    //Position on the particular axis that this event takes place at.
    float       eventPos;

    AABB* parentBox;
  };

  class KDTree
  {
    public:
      KDTree(std::vector<Triangle>& triList, DebugDraw* dbg, ID3D10Device* dev);
      ~KDTree();
      KDTree& operator=(const KDTree& rhs);
      void build();
      void draw();
      unsigned getMaxDrawDepth();
      void setMaxDrawDepth(unsigned depth);
      unsigned trianglesDrawn();
      void drawTris(Frustrum& frustrum);
      void buildTriBuffer(KDNode* tree, Frustrum& frust);

    private:
      void generateEventList(TriangleList& triList, AABB& parentBox);
      void clipTriToParentBox(Triangle& tri, AABB& pBox);
      void findMinPlane(TriangleList& triList, AABB& parentBox);
      void countTrisOfEvent(unsigned& Vs, unsigned& Ve, unsigned& Vc, unsigned& index, float pos);
      void calcCostOfPlane(unsigned Nl, unsigned Nr, unsigned Nc, AABB& parentBox, float pos);
      void splitBoxAtPos(AABB& l, AABB& r, const AABB& p, float pos);
      float SurfaceArea(const AABB& box);
      float MinPos(const Triangle& tri, SplitAxis axis);
      float MaxPos(const Triangle& tri, SplitAxis axis);
      KDNode* buildTree(TriangleList& triList, AABB& box, KDNode* parent, unsigned depth = 0);
      void cleanUpTree(KDNode* tree);
      void splitTriList(TriangleList& l, TriangleList& r, TriangleList& p, float pos);
      void drawTree(KDNode* tree, unsigned depth);
      void getVertBuffer();


      //This is what we'll build the tree from. (A list of worldspace triangles generated at init time)
      std::vector<Triangle>& staticSceneTris;

      //Just your basic binary tree, but better because its K dimensional!
      KDNode* tree;

      //We only need one event list for the entirety of the tree since we discard it every iteration
      std::vector<KDEvent> eventList;

      //This is the axis we're checking for splitting against
      SplitAxis curAxis;

      //Current and minimum costs used for finding planes.
      float curCost;
      float minCost;

      //This is the axis we're going to split on
      SplitAxis splitAxis;
      float splitPos;

      AABB sceneBox;
      unsigned maxDepth;
      DebugDraw* dbgDraw;
      unsigned maxDrawDepth;
      std::vector<Triangle> trianglesToDraw;
      std::vector<Vertex_Pos_Clr_Nrm_Tex> verts;
      ID3D10Buffer* vertBuffer;
      ID3D10Device* device;
      bool addLeft;
  };
}
#endif