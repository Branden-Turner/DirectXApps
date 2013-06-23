#include "precompiled.h"
#include <stack>

namespace cs350
{
  DBVH::DBVH( ObjectList& objList, DebugDraw* dbg, ID3D10Device* dev ) : sceneObjects(objList),
                                                                         dbgDraw(dbg),
                                                                         device(dev),
                                                                         minObjectsPerNode(1), 
                                                                         debug(true),
                                                                         maxDrawDepth(0),
                                                                         selectedObject(0)
  {
  }

  DBVH::~DBVH()
  {
    cleanUpTree(tree);
  }

  DBVH& DBVH::operator=( const DBVH& rhs )
  {
    sceneObjects = rhs.sceneObjects;
    build();
    return *this;
  }

  void DBVH::build()
  {
    tree = buildTree(sceneObjects, NULL);
    for(unsigned i = 0; i < sceneObjects[0].meshes.size(); ++i)
      sceneObjects[0].meshes[i]->setActive();
  }

  void DBVH::draw()
  {
    dbgDraw->drawAABB(sceneObjects[selectedObject].boundingBox, vec4(1.0f,1.0f,1.0f,1.0f));
    drawTree(tree, 0);
  }

  static float findMeanPos( ObjectList& objList, SplitAxis axis )
  {
    float splitPos = 0.0f;

    for(unsigned i = 0; i < objList.size(); ++i)
    {
      switch(axis)
      {
        case XAxis:
        splitPos += objList[i].boundingBox.center.x;
        break;

        case YAxis:
        splitPos += objList[i].boundingBox.center.y;
        break;

        case ZAxis:
        splitPos += objList[i].boundingBox.center.z;
        break;
      }
    }

    splitPos /= static_cast<float>(objList.size());

    return splitPos;
  }

  static SplitAxis findSplitAxis( AABB& p )
  {
    float pick;

    pick = max(max(p.extents.x, p.extents.y), p.extents.z);

    if(pick == p.extents.x)
      return XAxis;
    else if(pick == p.extents.y)
      return YAxis;
    else
      return ZAxis;
  }


  void DBVH::splitObjList( ObjectList& l, ObjectList& r, const ObjectList& p, SplitAxis axis, float pos )
  {
    //Go through the parent list and add each object to the right list, left or right
    for(unsigned i = 0; i < p.size(); ++i)
    {
      if( axis == XAxis )
      {
        if( p[i].boundingBox.center.x < pos )
          l.push_back(p[i]);
        else
          r.push_back(p[i]);
      }
      else if( axis == YAxis )
      {
        if( p[i].boundingBox.center.y < pos )
          l.push_back(p[i]);
        else
          r.push_back(p[i]);
      }
      else
      {
        if( p[i].boundingBox.center.z < pos )
          l.push_back(p[i]);
        else
          r.push_back(p[i]);
      }
    }
  }


  AABB DBVH::createAABBFromObjList( const ObjectList& objList )
  {
    AABB box;
    float minX, minY, minZ, maxX, maxY, maxZ;

    minX = minY = minZ = FLT_MAX;
    maxX = maxY = maxZ = -FLT_MAX;

    //Find the minimum and maximum x y and z points
    for(unsigned i = 0; i < objList.size(); ++i)
    {
      const Object& obj = objList[i];

      Pt minPt = obj.boundingBox.center - obj.boundingBox.extents;
      Pt maxPt = obj.boundingBox.center + obj.boundingBox.extents;

      if(minPt.x < minX)
        minX = minPt.x;

      if(maxPt.x > maxX)
        maxX = maxPt.x;

      if(minPt.y < minY)
        minY = minPt.y;

      if(maxPt.y > maxY)
        maxY = maxPt.y;

      if(minPt.z < minZ)
        minZ = minPt.z;

      if(maxPt.z > maxZ)
        maxZ = maxPt.z;
    }
    
    //Extents are just width/height/depth / 2
    box.extents.x = abs(maxX - minX) * 0.5f;
    box.extents.y = abs(maxY - minY) * 0.5f;
    box.extents.z = abs(maxZ - minZ) * 0.5f;

    //Center is just the min point plus the extents
    Pt minPt(minX, minY, minZ);

    box.center = minPt + box.extents;

    return box;
  }



  DBVHNode* DBVH::buildTree( ObjectList& objList, DBVHNode* parent )
  {
    //If we have objects, we have no node
    if(objList.empty())
      return NULL;

    //Create node to be filled out
    DBVHNode* node = new DBVHNode();

    //Set up parent linkage
    node->parent = parent;

    //Create this particular node's bounding box based off it's object list
    node->boundingBox = createAABBFromObjList(objList);

    //If we've split to our required amount, we just make this a leaf node.
    if(objList.size() <= minObjectsPerNode)
    {
      node->leaf = true;
      node->objects = objList;
      node->left = node->right = NULL;
      return node;
    }

    node->leaf = false;

    //pick a splitting axis (largest axis of parent box)
    SplitAxis splitAxis = findSplitAxis( node->boundingBox );

    //pick the position to split at (mean of the positions of the centers of each box)
    float splitPos = findMeanPos(objList, splitAxis);

    //Create two new objects lists based on the splitting position/axis
    ObjectList l, r;

    splitObjList(l, r, objList, splitAxis, splitPos);

    //Distribute objects if we have overlapping centers
    if(l.empty())
    {
      l.push_back(r[r.size() - 1]);
      r.pop_back();
    }
    else if(r.empty())
    {
      r.push_back(l[l.size() - 1]);
      l.pop_back();
    }

    node->left  = buildTree(l, node);
    node->right = buildTree(r, node);

    return node;
  }

  void DBVH::drawTree( DBVHNode* node, unsigned depth )
  {
    if(node)
    {
//       if(debug && ((depth == maxDrawDepth) || (depth < maxDrawDepth && node->leaf)))
//           dbgDraw->drawAABB(node->boundingBox, vec4(1.0f, 0.0f, 0.0f, 1.0f));

      if(node->leaf)
      {
        for(unsigned i = 0; i < node->objects.size(); ++i)
        {
          for(unsigned j = 0; j < node->objects[i].meshes.size(); ++j)
          {
            D3D10_TECHNIQUE_DESC techDesc;
            tech->GetDesc( &techDesc );
            for(UINT p = 0; p < techDesc.Passes; ++p)
            {
              node->objects[i].meshes[j]->setMtw();
              tech->GetPassByIndex( p )->Apply(0);  
              node->objects[i].meshes[j]->draw();
            }
          }
        }
      }

      drawTree(node->left, depth + 1);
      drawTree(node->right, depth + 1);
    }
  }

  void DBVH::setTech( ID3D10EffectTechnique* t )
  {
    tech = t;
  }

  void DBVH::toggleDebug()
  {
    debug = !debug;
  }

  void DBVH::setDrawDepth( unsigned depth )
  {
    maxDrawDepth = depth;
  }

  unsigned DBVH::getDrawDepth()
  {
    return maxDrawDepth;
  }

  int DBVH::height()
  {
    return heightOfSubTree(tree);
  }

  int DBVH::heightOfSubTree( DBVHNode* subTree )
  {
    if( subTree->leaf )
      return 0;

    return 1 + max(heightOfSubTree(subTree->left), heightOfSubTree(subTree->right));
  }

  AABB DBVH::combineNodeAABBs( const DBVHNode* n1, const DBVHNode* n2 )
  {
    return GeometryLibrary::mergeAABBs(n1->boundingBox, n2->boundingBox);
  }

  void DBVH::insertNode( DBVHNode* node )
  {
    if(tree == NULL)
    {
      tree = node;
      node->leaf = true;
      return;
    }

    DBVHNode* walker = tree;

    //Cost of left and right after combining nodes
    float costLeft, costRight;
    AABB newBoxLeft, newBoxRight;

    //As long as we haven't reached a leaf node...
    while(!walker->leaf)
    {
      //Calculate costs of each side combined with this node
      newBoxLeft = combineNodeAABBs(node, walker->left);
      newBoxRight = combineNodeAABBs(node, walker->right);

      costLeft = GeometryLibrary::perimeter( newBoxLeft );
      costRight = GeometryLibrary::perimeter( newBoxRight );

      //pick the child with the least cost
      if(costLeft < costRight)
        walker = walker->left;
      else
        walker = walker->right;
    }

    //Create a new node at our arrived position
    DBVHNode* newNode = new DBVHNode();

    //It will take the place of the arrived at node 
    newNode->parent = walker->parent;

    if(newNode->parent->left == walker)
      newNode->parent->left = newNode;
    else
      newNode->parent->right = newNode;

    //With the arrived at node and the inserted node as children
    newNode->left = walker;
    newNode->right = node;

    newNode->leaf = false;

    //Set up parent
    newNode->left->parent = newNode->right->parent = newNode;

    //Node inserted is a leaf node
    node->leaf = true;
    node->left = node->right = walker->left = walker->right = NULL;

    //Now go back up the tree, starting at the new node, and change the bounding volumes.
    walker = newNode;
    while(walker)
    {
      walker->boundingBox = combineNodeAABBs( walker->left, walker->right );
      walker = walker->parent;
    }
  }

  DBVHNode* DBVH::removeNode( DBVHNode* node )
  {
    if(node == tree)
    {
      return tree;
    }

    //This is assumed to always be called on a leaf node.  Don't be retarded.
    //Both the parent of this node, and this node will be deleted

    //Get the parent, grandparent, and sibling of this node
    DBVHNode *parent, *grandParent, *sibling;

    parent = node->parent;
    grandParent = node->parent->parent;
    
    if(node == parent->left)
      sibling = parent->right;
    else
      sibling = parent->left;

    //if the grandparent exists
    if(grandParent)
    {
      //replace the parent with the sibling
      if(grandParent->right == parent)
        grandParent->right = sibling;
      else
        grandParent->left = sibling;

      //hook up the proper parent pointer
      sibling->parent = grandParent;
    }

    //kill the parent and give back the node to the caller
    delete parent;
   
    //Start at the new parent (grandparent) and reset the bounding volumes
    DBVHNode* walker = grandParent;
    while(walker)
    {
      walker->boundingBox = combineNodeAABBs( walker->left, walker->right );
      walker = walker->parent;
    }

    return node;
  }

  void DBVH::balanceNode( DBVHNode* node )
  {
    if( node->leaf || (node->left->leaf && node->right->leaf) )
      return;

    int lHeight, rHeight;

    lHeight = heightOfSubTree(node->left);
    rHeight = heightOfSubTree(node->right);

    if(lHeight > rHeight)
      rotateRight(node);
    else
      rotateLeft(node);
  }

  void DBVH::rotateLeft( DBVHNode* subTree )
  {
    DBVHNode* pivot = subTree;
    DBVHNode* parent = pivot->parent;
    DBVHNode* right = pivot->right;

    //set right node in place of node
    if(parent)
    {
      if(parent->left == pivot)
        parent->left = right;
      else
        parent->right = right;     
    }

    right->parent = parent;

    //right node's left child is the pivot
    right->left = pivot;
    pivot->parent = right;

    //pivot node's right child is the right node's child with the largest depth
    int lHeight, rHeight;

    lHeight = heightOfSubTree(right->left);
    rHeight = heightOfSubTree(right->right);

    if(lHeight > rHeight)
      pivot->right = right->right;
    else 
      pivot->right = right->left;

    pivot->right->parent = pivot;
  }

  void DBVH::rotateRight( DBVHNode* subTree )
  {
    DBVHNode* pivot = subTree;
    DBVHNode* parent = pivot->parent;
    DBVHNode* left = pivot->left;

    //set left node in place of node
    if(parent)
    {
      if(parent->left == pivot)
        parent->left = left;
      else
        parent->right = left;     
    }

    left->parent = parent;

    //left node's right child is the pivot
    left->right = pivot;
    pivot->parent = left;

    //pivot node's left child is the left node's child with the largest depth
    int lHeight, rHeight;

    lHeight = heightOfSubTree(left->left);
    rHeight = heightOfSubTree(left->right);

    if(lHeight > rHeight)
      pivot->left = left->right;
    else 
      pivot->left = left->left;

    pivot->left->parent = pivot;
  }

  void DBVH::makeAABBs( DBVHNode* tree )
  {
    if(tree)
    {
      makeAABBs(tree->left);
      makeAABBs(tree->right);

      if(tree->left && tree->right)
        tree->boundingBox = combineNodeAABBs(tree->left, tree->right);
      else if(tree->left)
        tree->boundingBox = tree->left->boundingBox;
      else if(tree->right)
        tree->boundingBox = tree->right->boundingBox;
    }
  }

  void DBVH::balanceSubTree( DBVHNode* subTree )
  {
    if(subTree)
    {
      //pivot node's left child is the left node's child with the largest depth
      int lHeight, rHeight;

      lHeight = heightOfSubTree(subTree->left);
      rHeight = heightOfSubTree(subTree->right);

      if(abs(lHeight - rHeight) > 1)
        balanceNode(subTree);

      makeAABBs(subTree);
    }
  }

  void DBVH::changeSelectedObject()
  {
    //Set current object inactive
    Object& obj1 = sceneObjects[selectedObject];
    for(unsigned i = 0; i < obj1.meshes.size(); ++i)
      obj1.meshes[i]->setInactive();

    ++selectedObject;
    if(selectedObject == sceneObjects.size())
      selectedObject = 0;

    //Set next object active
    Object& obj2 = sceneObjects[selectedObject];
    for(unsigned i = 0; i < obj2.meshes.size(); ++i)
      obj2.meshes[i]->setActive();
  }

  Object& DBVH::selectedObj()
  {
    return sceneObjects[selectedObject];
  }

  void DBVH::cleanUpTree( DBVHNode* tree )
  {
    if(tree)
    {
      cleanUpTree(tree->left);
      cleanUpTree(tree->right);

      delete tree;

      tree = NULL;
    }
  }

  void DBVH::selectedObjectMoved()
  {
    Object& obj = selectedObj();

    getNodeFromObj(obj, tree);

    //Update the selected node
    for(unsigned i = 0; i < selectedNode->objects.size(); ++i)
      if(selectedNode->objects[i].meshes[0] == obj.meshes[0])
        selectedNode->objects[i].boundingBox = obj.boundingBox;
    
    selectedNode->boundingBox = createAABBFromObjList(selectedNode->objects);

    //remove and insert the selected node
    //insertNode(removeNode(selectedNode));
    //balanceSubTree(tree);
  }

  void DBVH::getNodeFromObj( const Object& obj, DBVHNode* tree )
  {
    if(tree)
    {
      if(tree->leaf)
      {
        for(unsigned i = 0; i < tree->objects.size(); ++i)
        {
          if(tree->objects[i].meshes[0] == obj.meshes[0])
          {
            selectedNode = tree;
            return;
          }
        }
      }

      getNodeFromObj(obj, tree->left);
      getNodeFromObj(obj, tree->right);
    }
  }

  Object& DBVH::otherObj()
  {
    unsigned nextObj = selectedObject + 1;
    unsigned prevObj = selectedObject - 1;

    if(selectedObject == 0)
      return sceneObjects[nextObj];
    else
      return sceneObjects[prevObj];
  }


  DBVHNode::DBVHNode()
  {
    leaf = false;
    left = right = parent = NULL;
  }

}