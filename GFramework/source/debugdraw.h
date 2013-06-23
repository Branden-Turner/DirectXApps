#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

namespace cs350
{
  class DebugDraw
  {
    public:
      DebugDraw(ID3D10Device* dev);
      ~DebugDraw();

      void drawLine( Line l, vec4& clr = vec4(1.0f, 1.0f, 1.0f, 1.0f) );
      void drawAABB( AABB& b, vec4& clr = vec4(1.0f, 1.0f, 1.0f, 1.0f) );
      void drawPlane( Plane p, vec4& clr = vec4(1.0f, 1.0f, 1.0f, 1.0f) );
      void drawPortal( MPortal& m );
      

    friend class DXAPP;
    private:
      void addLine(Pt p1, Pt p2, vec4& clr);
      void prepDraw();
      void draw();
      void getVertBuffer();

      ID3D10Device* device;
      ID3D10EffectMatrixVariable* camProj;
      ID3D10Buffer* vertBuffer;
      std::vector<Vertex_Pos_Clr_Nrm_Tex> verts;
      unsigned maxVerts;
      float lineLength;
  };
}

#endif