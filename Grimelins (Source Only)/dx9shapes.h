/*! ====================================================================================================================
  @file shapes.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.26.2011

  @brief
    Basic shape library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9SHAPES_H
#define VIRTUS_DX9SHAPES_H

#include "dx9.h"
#include "dx9buffers.h"
#include "dx9vertextype.h"
#include "shapes.h"

namespace Grimelins {

    class DX9ResourceManager;

    class Shape
    {
    public:
      Shape( ShapeType type, u32 tris, u32 vert, u16 indx, DXVertexDeclPtr decl );
      ~Shape( void );

      DX9VertexBufferPtr mVBuffer;
      DX9IndexBufferPtr  mIBuffer;
      DXVertexDeclPtr    mDecl;

      ShapeType mType;

      u32 mNumTris;
      u32 mNumVerts;
      u16 mNumIndices;

      std::vector<Vertex_XYZ_NRM_UV> vtx3D;
      std::vector<Vertex_XY_UV> vtx2D;

    private:
      Shape( void );

    }; // class Shape

    typedef Shape* ShapePtr;

    class DX9Shapes 
    {
    public:

      typedef std::vector<Vertex_XYZ_NRM_UV> Verts;
      typedef std::vector<u16> Idxs;

      DX9Shapes( DX9ResourceManager* rm );
      ~DX9Shapes( void );

      ShapePtr GetShape( ShapeType type );

    private:
      DX9Shapes( void );

      void BuildPlane(    u32 stacks, u32 slices );
      void BuildCube(     u32 stacks, u32 slices );
      void BuildCylinder( u32 stacks, u32 slices );
      void BuildCone(     u32 stacks, u32 slices );
      void BuildSphere(   u32 stacks, u32 slices );
      void BuildTorus(    u32 stacks, u32 slices );

      void Build2DQuad( void );
      void BuildSkyBox( void );

      void BuildIndexBuffer( u32 stacks, u32 slices, Idxs& idx );
      void AddShape( ShapeType type, Verts const& vertexes, Idxs const& indices, DXVertexDeclPtr decl = Vertex_XYZ_NRM_UV::Decl );

      ShapePtr mShapes[Shapes_MAX];
      DX9ResourceManager* mRM;

    }; // class DX9Shapes
} // namespace Virtus
#endif // Include Guard