/*! ====================================================================================================================
  @file dx9vertextype.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.11.2011

  @brief
    DirectX Vertex Typesf

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "dx9vertextype.h"

namespace Grimelins {

DXVertexDeclPtr Vertex_XYZ::Decl = NULL;
DXVertexDeclPtr Vertex_XYZ_UV::Decl = NULL;
DXVertexDeclPtr Vertex_XYZ_NRM_UV::Decl = NULL;

DXVertexDeclPtr Vertex_XY::Decl = NULL;
DXVertexDeclPtr Vertex_XY_UV::Decl = NULL;

DXVertexDeclPtr Vertex_Mesh::Decl = NULL;
DXVertexDeclPtr Vertex_Mesh_Anim::Decl = NULL;

DXVertexDeclPtr TextInstance::Decl = NULL;

void InitializeVertexDeclarations( DXDevice* dev )
{
  //////////////////////////////////////////////////////////////////////////
  // 3D Vertexes
  //////////////////////////////////////////////////////////////////////////
  HR( dev->CreateVertexDeclaration( Vertex_XYZ_Elements,        &Vertex_XYZ::Decl        ) );
  HR( dev->CreateVertexDeclaration( Vertex_XYZ_UV_Elements,     &Vertex_XYZ_UV::Decl     ) );
  HR( dev->CreateVertexDeclaration( Vertex_XYZ_NRM_UV_Elements, &Vertex_XYZ_NRM_UV::Decl ) );

  //////////////////////////////////////////////////////////////////////////
  // 2D Vertexes
  //////////////////////////////////////////////////////////////////////////

  HR( dev->CreateVertexDeclaration( Vertex_XY_Elements,         &Vertex_XY::Decl         ) );
  HR( dev->CreateVertexDeclaration( Vertex_XY_UV_Elements,      &Vertex_XY_UV::Decl      ) );

  //////////////////////////////////////////////////////////////////////////
  // Mesh Vertexes
  //////////////////////////////////////////////////////////////////////////

  HR( dev->CreateVertexDeclaration( Vertex_Mesh_Elements,       &Vertex_Mesh::Decl       ) );
  HR( dev->CreateVertexDeclaration( Vertex_Mesh_Anim_Elements,  &Vertex_Mesh_Anim::Decl  ) );

  //////////////////////////////////////////////////////////////////////////
  // Instance Vertexes
  //////////////////////////////////////////////////////////////////////////

  HR( dev->CreateVertexDeclaration( TextInstance_Elements,      &TextInstance::Decl      ) );
}

void ReleaseVertexDeclarations( void )
{
  SafeRelease(TextInstance::Decl);

  SafeRelease(Vertex_Mesh_Anim::Decl);
  SafeRelease(Vertex_Mesh::Decl);

  SafeRelease(Vertex_XY_UV::Decl);
  SafeRelease(Vertex_XY::Decl);

  SafeRelease(Vertex_XYZ_NRM_UV::Decl);
  SafeRelease(Vertex_XYZ_UV::Decl);
  SafeRelease(Vertex_XYZ::Decl);
}

} // namespace Virtus