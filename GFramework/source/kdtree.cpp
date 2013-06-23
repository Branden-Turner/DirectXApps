#include "precompiled.h"

namespace cs350
{

  KDTree::KDTree( std::vector<Triangle>& triList, DebugDraw* dbg, ID3D10Device* dev ) : staticSceneTris(triList), 
                                                                                        maxDepth(5), 
                                                                                        dbgDraw(dbg), 
                                                                                        maxDrawDepth(0), 
                                                                                        device(dev)
  {
    tree = NULL;
    getVertBuffer();
  }

  KDTree::~KDTree()
  {
    cleanUpTree(tree);
  }

  static void checkPos( float axisPos, unsigned& left, unsigned& right, unsigned& cop, float splitPos )
  {
    if(approxEqual(axisPos, splitPos))
      ++cop;
    else if( axisPos < splitPos)
      ++left;
    else 
      ++right;
  }

  static void CountVertPositions( const Triangle& tri, unsigned& left, unsigned& right, unsigned& cop, SplitAxis axis, float pos )
  {
    float p1, p2, p3;

    switch(axis)
    {
      case XAxis:
        p1 = tri.p1.x;
        p2 = tri.p2.x;
        p3 = tri.p3.x;
        checkPos(p1, left, right, cop, pos);
        checkPos(p2, left, right, cop, pos);
        checkPos(p3, left, right, cop, pos);

      break;

      case YAxis:
        p1 = tri.p1.y;
        p2 = tri.p2.y;
        p3 = tri.p3.y;
        checkPos(p1, left, right, cop, pos);
        checkPos(p2, left, right, cop, pos);
        checkPos(p3, left, right, cop, pos);
      break;

      case ZAxis:
        p1 = tri.p1.z;
        p2 = tri.p2.z;
        p3 = tri.p3.z;
        checkPos(p1, left, right, cop, pos);
        checkPos(p2, left, right, cop, pos);
        checkPos(p3, left, right, cop, pos);
      break;
    }
  }

  void KDTree::splitTriList( TriangleList& l, TriangleList& r, TriangleList& p, float pos )
  {
    curAxis = splitAxis;
    unsigned left, right, coplanar, side;

    //Go through every triangle in the triangle list
    for(unsigned i = 0; i < p.size(); ++i)
    {
      Triangle& tri = p[i];
      left = right = coplanar = 0;
      CountVertPositions(tri, left, right, coplanar, splitAxis, pos);

      side = max(left, max(right, coplanar));

      if(side == left)
        l.push_back(tri);
      else if(side == right)
        r.push_back(tri);
      else if(side == coplanar)
        l.push_back(tri);
    }
  }

  KDNode* KDTree::buildTree( TriangleList& triList, AABB& box, KDNode* parent, unsigned depth )
  {
    //Choose a minimum plane based off the triangle list
    findMinPlane(triList, box);

    //If the minimum cost is more than not splitting, or we're at a max depth,
    //Create a leaf node out of what's left
    if(minCost > triList.size() || depth == maxDepth)
    {
      KDNode* node = new KDNode(box, triList, splitAxis, splitPos, parent);
      node->childLeft = node->childRight = NULL;
      return node;
    }

    //The current axis we care about is the axis we're going to split on.
    curAxis = splitAxis;

    //Make AABB l and r out of the current AABB
    AABB bl, br;
    splitBoxAtPos(bl, br, box, splitPos);

    //Make two new triangle lists out of the current triangle list
    TriangleList tl, tr;
    splitTriList(tl, tr, triList, splitPos);

    //Create a new node for storing the aabb info and the triangle list, as well as dividing plane info
    KDNode* node = new KDNode(box, triList, splitAxis, splitPos, parent);

    node->childLeft = buildTree(tl, bl, node, depth + 1);
    node->childRight = buildTree(tr, br, node, depth + 1);
    return node;
  }

  void KDTree::build()
  {
    //Kill the tree and rebuild fresh if we need to.
    cleanUpTree(tree);

    //Build an AABB from the scene (this will also sort the scene's tris along x.)
    sceneBox = GeometryLibrary::buildAABBFromTriList(staticSceneTris);

    tree = buildTree(staticSceneTris, sceneBox, NULL);
  }

  KDTree& KDTree::operator=( const KDTree& rhs )
  {
    staticSceneTris = rhs.staticSceneTris;
    build();
    return *this;
  }



  void KDTree::clipTriToParentBox( Triangle& tri, AABB& pBox )
  {
    //construct pmin and pmax out of the box's center and extents
    Pt pmin, pmax;

    pmin = pBox.center - pBox.extents;
    pmax = pBox.center + pBox.extents;

    //Clamp all three points of the triangle to the AABB
    clamp(tri.p1.x, pmin.x, pmax.x);
    clamp(tri.p1.y, pmin.y, pmax.y);
    clamp(tri.p1.z, pmin.z, pmax.z);

    clamp(tri.p2.x, pmin.x, pmax.x);
    clamp(tri.p2.y, pmin.y, pmax.y);
    clamp(tri.p2.z, pmin.z, pmax.z);

    clamp(tri.p3.x, pmin.x, pmax.x);
    clamp(tri.p3.y, pmin.y, pmax.y);
    clamp(tri.p3.z, pmin.z, pmax.z);
  }

  void KDTree::generateEventList( TriangleList& triList, AABB& parentBox )
  {
    //Start with an empty event list
    eventList.clear();

    //Go through all the triangles passed in.
    for(unsigned i = 0; i < triList.size(); ++i)
    {
      //Clip the triangle to the parent box
      Triangle tri = triList[i];
      clipTriToParentBox(tri, parentBox);

      //And the box be the AABB of this event
      AABB* pBox = &parentBox;

      //Grab the minimum and maximum points of the triangle as per this split axis
      float pMin = GeometryLibrary::minPos(tri, curAxis);
      float pMax = GeometryLibrary::maxPos(tri, curAxis);

      //If the maximum and minimum points are the same, we have a coplanar event
      if(pMax == pMin)
        eventList.push_back(KDEvent(Coplanar, &tri, pMin, pBox));
      //Otherwise start at the minimum and end at the maximum on this axis
      else
      {
        eventList.push_back(KDEvent(Starting, &tri, pMin, pBox));
        eventList.push_back(KDEvent(Ending, &tri, pMax, pBox));
      }
    }

    //Sort according to the operator < provided so that the events are in ascending order
    std::sort(eventList.begin(), eventList.end());
  }

  void KDTree::findMinPlane(TriangleList& triList, AABB& parentBox)
  {
    unsigned Nl, Nr, Nc, Vs, Ve, Vc;
    Vs = Ve = Vc = 0;

    //Set minimum cost to infinite
    minCost = FLT_MAX;

    //For every possible axis,
    for(unsigned i = 0; i < NumAxes; ++i)
    {
      //Generate the event list for this axis
      curAxis = (SplitAxis)i;
      generateEventList(triList, parentBox);

      //All of the triangles are to our right.
      //These will remain through iterations on positions
      Nl = 0;
      Nr = triList.size();
      Nc = 0;

      //For every position
      for(unsigned j = 0; j < eventList.size(); )
      {
        float curPos = eventList[j].eventPos;
        Vs = Ve = Vc = 0;
        //Count the triangles starting, ending, and coplanar with this position
        //This will advance j to the next event position
        countTrisOfEvent(Vs, Ve, Vc, j, curPos);

        Nc  = Vc;
        Nr -= Ve + Vc;

        //The last index is the positions we care about.
        calcCostOfPlane(Nl, Nr, Nc, parentBox, curPos);

        Nl += Vs + Vc;

        //Update the lowest possible plane
        if(curCost < minCost)
        {
          minCost = curCost;

          //Save what axis and position we're going to cut at so far...
          splitPos = curPos;
          splitAxis = curAxis;
        }
      }
    }
  }

  void KDTree::countTrisOfEvent( unsigned& Vs, unsigned& Ve, unsigned& Vc, unsigned& index, float pos )
  {
    //Accumulate all of the events of this position
    for(; index < eventList.size(); ++index)
    {
      //Stop as soon as we get to a new position in the events list
      if(eventList[index].eventPos != pos)
        break;

      switch(eventList[index].type)
      {
        case Coplanar:
          ++Vc;
        break;

        case Ending:
          ++Ve;
        break;

        case Starting:
          ++Vs;
        break;
      }
    }
  }


  void KDTree::splitBoxAtPos( AABB& l, AABB& r, const AABB& p, float pos )
  {
    GeometryLibrary::splitBox(l, r, p, pos, curAxis);
  }


  float KDTree::SurfaceArea( const AABB& box )
  {
    Pt pMax = box.center + box.extents;
    Pt pMin = box.center - box.extents;

    //Get the width height and depth of the box
    float width = abs(pMax.x - pMin.x);
    float height = abs(pMax.y - pMin.y);
    float depth = abs(pMax.z - pMin.z);

    //Now give back the surface area
    return 2 * (width * height + width * depth + depth * height);
  }

  void KDTree::calcCostOfPlane( unsigned Nl, unsigned Nr, unsigned Nc, AABB& parentBox, float pos )
  {
    //Split this AABB into two boxes at this point in the axis.
    AABB l, r;

    //Split the box at this plane
    splitBoxAtPos(l, r, parentBox, pos);

    //Calculate the surface area of the left, right, and parent boxes
    float SAP, SAR, SAL;

    SAP = SurfaceArea(parentBox);
    SAL = SurfaceArea(l);
    SAR = SurfaceArea(r);

    //The current cost of this plane is now going to be the minimum of adding the coplanar 
    //triangles to each child box
    float lHeuristic, rHeuristic;

    lHeuristic = (SAL / SAP);
    rHeuristic = (SAR / SAP);

    float coLeft =  lHeuristic * (Nl + Nc) + rHeuristic * Nr;
    float coRight = lHeuristic * Nl + rHeuristic * (Nr + Nc);

    curCost = min( coLeft, coRight );
  }

  void KDTree::cleanUpTree(KDNode* tree)
  {
    if(tree)
    {
      cleanUpTree(tree->childLeft);
      cleanUpTree(tree->childRight);

      delete tree;

      tree = NULL;
    }
  }

  void KDTree::draw()
  {
    drawTree(tree, 0);
  }

  void KDTree::drawTree( KDNode* tree, unsigned depth )
  {
    if(depth == maxDrawDepth)
      return;

    if(tree)
    {
      dbgDraw->drawAABB(tree->boundingBox);
      drawTree(tree->childLeft, depth + 1);
      drawTree(tree->childRight, depth + 1);
    }
  }

  unsigned KDTree::getMaxDrawDepth()
  {
    return maxDrawDepth;
  }

  void KDTree::setMaxDrawDepth(unsigned depth)
  {

      maxDrawDepth = depth;
  }

  unsigned KDTree::trianglesDrawn()
  {
    return trianglesToDraw.size();
  }


  void KDTree::buildTriBuffer( KDNode* tree, Frustrum& frust )
  {
    //First, is our tree good?
    if(tree)
    {
       //Should we cull this box?
       if(frust.checkInside(tree->boundingBox))
       {
        //If we're at a leaf node, we copy over all the data from this leaf's triangle list to our buffer
        if(!tree->childLeft && !tree->childRight)
        {
          for(unsigned i = 0; i < tree->triangles.size(); ++i)
            trianglesToDraw.push_back(tree->triangles[i]);
        }
        //Otherwise we keep traversing the tree.
        else
        {
          buildTriBuffer(tree->childLeft, frust);
          buildTriBuffer(tree->childRight, frust);
        }
      }
    }
  }

  void KDTree::drawTris( Frustrum& frustrum )
  {
    //Clear out our buffers per frame
    trianglesToDraw.clear();
    verts.clear();

    //recurse through the tree, adding the leafs we can see to the triangles to draw list
    buildTriBuffer(tree, frustrum);

    //Now that we have all the triangles we should be drawing, turn them into our vertex buffer
    for(unsigned i = 0; i < trianglesToDraw.size(); ++i)
    {
      verts.push_back(trianglesToDraw[i].vert1);
      verts.push_back(trianglesToDraw[i].vert2);
      verts.push_back(trianglesToDraw[i].vert3);
    }

    //Now actually draw everything we want as triangles
    UINT stride = sizeof(Vertex_Pos_Clr_Nrm_Tex);
    UINT offset = 0;

    //Updates the vertex buffer with the appropriate data.
    if(verts.size()) 
      device->UpdateSubresource(vertBuffer, 0, NULL, &verts[0], verts.size(), 0);

    device->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);
    device->Draw(verts.size(), 0);
  }

  void KDTree::getVertBuffer()
  {
    verts.reserve(staticSceneTris.size() * 3);
    verts.resize(staticSceneTris.size() * 3);
    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(Vertex_Pos_Clr_Nrm_Tex) * staticSceneTris.size() * 3;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &verts[0];
    vinitData.SysMemPitch =  0;
    vinitData.SysMemSlicePitch = 0;
    HR(device->CreateBuffer(&vbd, &vinitData, &vertBuffer));
    verts.clear();
  }

  KDEvent::KDEvent( KDEventType ty, Triangle* tri, float pos, AABB* box  ) : type(ty), 
                                                                             eventTri(tri),
                                                                             eventPos(pos),
                                                                             parentBox(box)
  {
  }

  bool KDEvent::operator<( const KDEvent& rhs )
  {
    //First based on where they happened
    if(eventPos < rhs.eventPos)
      return true;
    else if(eventPos > rhs.eventPos)
      return false;

    //and if they're the same
    else
      //Ending < Coplanar < Starting
      return type < rhs.type;
  }


  KDNode::KDNode( AABB& box, TriangleList& tris, SplitAxis axis, float pos, KDNode* p )
    : parent(p), boundingBox(box), triangles(tris), splitAxis(axis), splitPos(pos)
  {
    childLeft = childRight = NULL;
  }

}


