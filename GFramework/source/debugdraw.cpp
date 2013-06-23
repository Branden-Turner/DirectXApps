#include "precompiled.h"

namespace cs350
{


  DebugDraw::DebugDraw( ID3D10Device* dev ) : device(dev), maxVerts(1000000), lineLength(500.0f)
  {
    getVertBuffer();
  }

  void DebugDraw::drawLine( Line l, vec4& clr /*= vec4(1.0f, 1.0f, 1.0f, 1.0f) */ )
  {
    //Draw from the point to our specified line length along the direction vector
    addLine(l.p, l.v * lineLength, clr);
  }

  void DebugDraw::drawAABB( AABB& b, vec4& clr /*= vec4(1.0f, 1.0f, 1.0f, 1.0f) */ )
  {
    //Generate the eight points of the AABB
    Pt p1, p2, p3, p4, p5, p6, p7, p8;

    //P1 is min, 
    p1 = b.center - b.extents;

    //up to p2, 
    p2 = p1 + vec3(0, (b.extents.y * 2), 0);

    //then right to p3, 
    p3 = p2 + vec3((b.extents.x * 2), 0, 0);

    //then down to p4
    p4 = p3 - vec3(0, (b.extents.y * 2), 0);

    //back to p5
    p5 = p4 + vec3(0, 0, (b.extents.z * 2));

    //left to p6
    p6 = p5 - vec3((b.extents.x * 2), 0, 0);

    //up to p7
    p7 = p6 + vec3(0, (b.extents.y * 2), 0);

    //then right to p8
    p8 = p7 + vec3((b.extents.x * 2), 0, 0);

    //Now draw lines between all of them of the color passed in.
    addLine(p1, p2, clr);
    addLine(p2, p3, clr);
    addLine(p3, p4, clr);
    addLine(p4, p5, clr);
    addLine(p5, p6, clr);
    addLine(p6, p7, clr);
    addLine(p7, p8, clr);
    addLine(p1, p6, clr);
    addLine(p2, p7, clr);
    addLine(p3, p8, clr);
    addLine(p1, p4, clr);
    addLine(p5, p8, clr);
  }

  void DebugDraw::drawPlane( Plane p, vec4& clr /*= vec4(1.0f, 1.0f, 1.0f, 1.0f) */ )
  {
    //Generate the four points we'll use as a plane.
    Pt p1, p2, p3, p4;

    p1 = p.q;
    p2 = p.q + p.v1;
    p3 = p.q + p.v2;
    p4 = p2  + p.v2;

    addLine(p1, p2, clr);
    addLine(p1, p3, clr);
    addLine(p3, p4, clr);
    addLine(p4, p2, clr);
  }


  void DebugDraw::drawPortal( MPortal& m )
  {
    static vec4 pClr = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    static vec4 rClr = vec4(1.0f, 0.0f, 0.0f, 1.0f);

    addLine(m.c, m.p1, pClr);
    addLine(m.c, m.p2, pClr);
    addLine(m.c, m.p3, pClr);
    addLine(m.p1, m.p2, pClr);
    addLine(m.p1, m.p3, pClr);
    addLine(m.p2, m.p3, pClr);

    addLine(m.c, vec3(0,0,0), rClr);
  }


  void DebugDraw::addLine( Pt p1, Pt p2, vec4& clr )
  {
    Vertex_Pos_Clr_Nrm_Tex v1, v2;

    v1.clr = v2.clr = clr;
    v1.pos = p1;
    v2.pos = p2;

    verts.push_back(v2);
    verts.push_back(v1);
    
  }

  void DebugDraw::prepDraw()
  {
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
  }

  void DebugDraw::draw()
  {
    UINT stride = sizeof(Vertex_Pos_Clr_Nrm_Tex);
    UINT offset = 0;

    //Updates the vertex buffer with the appropriate data.
    if(verts.size()) 
      device->UpdateSubresource(vertBuffer, 0, NULL, &verts[0], verts.size() * stride, 0);

    device->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);
    device->Draw(verts.size(), 0);

    //Clears out the verticies for future draw calls, since they will happen every frame.
    verts.clear();
  }

  void DebugDraw::getVertBuffer()
  {
    verts.reserve(maxVerts);
    verts.resize(maxVerts);
    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(Vertex_Pos_Clr_Nrm_Tex) * maxVerts;
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


}