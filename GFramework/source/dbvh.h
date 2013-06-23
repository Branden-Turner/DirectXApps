#ifndef DBVH_H
#define DBVH_H

namespace cs350
{
  class DebugDraw;

  struct DBVHNode
  {
    DBVHNode();
    AABB boundingBox;
    bool leaf;

    ObjectList objects;

    DBVHNode* left;
    DBVHNode* right;
    DBVHNode* parent;
  };

  class DBVH
  {
    public:
      DBVH(ObjectList& objList, DebugDraw* dbg, ID3D10Device* dev);
      ~DBVH();
      DBVH& operator=(const DBVH& rhs);
      void build();
      void draw();
      void setTech(ID3D10EffectTechnique* t);
      void toggleDebug();
      void setDrawDepth(unsigned depth);
      unsigned getDrawDepth();
      int height();
      void changeSelectedObject();
      Object& selectedObj();
      Object& otherObj();
      void selectedObjectMoved();

    private:
      DBVHNode* buildTree( ObjectList& objList, DBVHNode* parent );
      void splitObjList( ObjectList& l, ObjectList& r, const ObjectList& p, SplitAxis axis, float pos );
      AABB createAABBFromObjList( const ObjectList& objList );
      void drawTree( DBVHNode* node, unsigned depth );
      int heightOfSubTree( DBVHNode* subTree );
      void insertNode( DBVHNode* node );
      AABB combineNodeAABBs( const DBVHNode* n1, const DBVHNode* n2 );
      DBVHNode* removeNode( DBVHNode* node );
      void balanceNode( DBVHNode* node );
      void rotateLeft( DBVHNode* subTree );
      void rotateRight( DBVHNode* subTree );
      void makeAABBs( DBVHNode* tree );
      void balanceSubTree( DBVHNode* subTree );
      void cleanUpTree( DBVHNode* tree ); 
      void getNodeFromObj( const Object& obj, DBVHNode* tree );
      

    private:
      ObjectList sceneObjects;
      DebugDraw* dbgDraw;
      ID3D10Device* device;
      DBVHNode* tree;
      unsigned minObjectsPerNode;
      bool debug;
      ID3D10EffectTechnique*  tech;
      unsigned maxDrawDepth;
      unsigned selectedObject;
      DBVHNode* selectedNode;
  };
}
#endif