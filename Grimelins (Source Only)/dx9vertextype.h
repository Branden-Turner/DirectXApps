/*! ====================================================================================================================
  @file dx9vertextype.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.11.2011

  @brief
    DirectX vertex types

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9VERTEXTYPE_H
#define VIRTUS_DX9VERTEXTYPE_H

#include "dx9.h"

namespace Grimelins {
    
    void InitializeVertexDeclarations( DXDevice* dev );
    void ReleaseVertexDeclarations( void );

    //////////////////////////////////////////////////////////////////////////
    // Vertex with 3D Position
    //////////////////////////////////////////////////////////////////////////

    #pragma pack(1)
    struct Vertex_XYZ
    {
      vec3 pos;
      static DXVertexDeclPtr Decl;
    };
    #pragma pack()

      // Stream, Offset, Type, Method, Usage, UsageIndex
    static DXVertexElem Vertex_XYZ_Elements[] = 
    {
      {0, offsetof(Vertex_XYZ, pos), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      D3DDECL_END()
    };

    //////////////////////////////////////////////////////////////////////////
    // Vertex with 3D Position and 2D Texture Coordinates
    //////////////////////////////////////////////////////////////////////////

    #pragma pack(1)
    struct Vertex_XYZ_UV
    {
      vec3 pos;
      vec2 tex;
      enum FVF
      {
        FVF_FLAGS = D3DFVF_XYZ | D3DFVF_DIFFUSE
      };
      static DXVertexDeclPtr Decl;
    };
    #pragma pack()

    // Stream, Offset, Type, Method, Usage, UsageIndex
    static DXVertexElem Vertex_XYZ_UV_Elements[] = 
    {
      {0, offsetof(Vertex_XYZ_UV, pos), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      {0, offsetof(Vertex_XYZ_UV, tex), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
      D3DDECL_END()
    };


    //////////////////////////////////////////////////////////////////////////
    // Vertex with 3D Position, 3D Normal, and 2D Texture Coordinates
    //////////////////////////////////////////////////////////////////////////

    #pragma pack(1)
    struct Vertex_XYZ_NRM_UV
    {
      vec3 pos;
      vec3 nrm;
      vec2 tex;
      static DXVertexDeclPtr Decl;
    };
    #pragma pack()

    // Stream, Offset, Type, Method, Usage, UsageIndex
    static DXVertexElem Vertex_XYZ_NRM_UV_Elements[] = 
    {
      {0, offsetof(Vertex_XYZ_NRM_UV, pos), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      {0, offsetof(Vertex_XYZ_NRM_UV, nrm), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
      {0, offsetof(Vertex_XYZ_NRM_UV, tex), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
      D3DDECL_END()
    };

    //////////////////////////////////////////////////////////////////////////
    // Vertex with 2D Position
    //////////////////////////////////////////////////////////////////////////

    #pragma pack(1)
    struct Vertex_XY
    {
      vec2 pos;
      static DXVertexDeclPtr Decl;
    };
    #pragma pack()

    // Stream, Offset, Type, Method, Usage, UsageIndex
    static DXVertexElem Vertex_XY_Elements[] = 
    {
      {0, offsetof(Vertex_XY, pos), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      D3DDECL_END()
    };

    //////////////////////////////////////////////////////////////////////////
    // Vertex with 2D Postion and 2D Texture Coordinates
    //////////////////////////////////////////////////////////////////////////

    #pragma pack(1)
    struct Vertex_XY_UV
    {
      vec2 pos;
      vec2 tex;
      static DXVertexDeclPtr Decl;
    };
    #pragma pack()

    // Stream, Offset, Type, Method, Usage, UsageIndex
    static DXVertexElem Vertex_XY_UV_Elements[] = 
    {
      {0, offsetof(Vertex_XY_UV, pos), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      {0, offsetof(Vertex_XY_UV, tex), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
      D3DDECL_END()
    };

    //////////////////////////////////////////////////////////////////////////
    // Vertexes for meshes
    //////////////////////////////////////////////////////////////////////////
    #pragma pack(1)
    struct Vertex_Mesh
    {
      vec3 pos_;
      vec3 nrm_;
      vec2 uv_;
      vec3 tan_;
      vec3 bitan_;
      static DXVertexDeclPtr Decl;
    };
    #pragma pack()

    // Stream, Offset, Type, Method, Usage, UsageIndex
    static DXVertexElem Vertex_Mesh_Elements[] = 
    {
      {0, offsetof(Vertex_Mesh, pos_), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      {0, offsetof(Vertex_Mesh, nrm_), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
      {0, offsetof(Vertex_Mesh, uv_), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
      {0, offsetof(Vertex_Mesh, tan_), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
      {0, offsetof(Vertex_Mesh, bitan_), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
      D3DDECL_END()
    };

    #pragma pack(1)
    struct Vertex_Mesh_Anim
    {
      vec3 pos_;
      vec3 nrm_;
      vec2 uv_;
      vec3 tan_;
      vec3 bitan_;
      u8 boneIndices_ [4];
      f32 boneWeights_ [4];
      static DXVertexDeclPtr Decl;
    };
    #pragma pack()

    // Stream, Offset, Type, Method, Usage, UsageIndex
    static DXVertexElem Vertex_Mesh_Anim_Elements[] = 
    {
      {0, offsetof(Vertex_Mesh_Anim, pos_), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      {0, offsetof(Vertex_Mesh_Anim, nrm_), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
      {0, offsetof(Vertex_Mesh_Anim, uv_), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
      {0, offsetof(Vertex_Mesh_Anim, tan_), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
      {0, offsetof(Vertex_Mesh_Anim, bitan_), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
      {0, offsetof(Vertex_Mesh_Anim, boneIndices_), D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
      {0, offsetof(Vertex_Mesh_Anim, boneWeights_), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
      D3DDECL_END()
    };


    //////////////////////////////////////////////////////////////////////////
    // Text Instance class... Is jawesome.
    //////////////////////////////////////////////////////////////////////////

    #pragma pack(1)
    struct TextInstance
    {
      vec4 pos_sca;
      vec4 tex_off;
      static DXVertexDeclPtr Decl;
    };
    #pragma pack()

    // Stream, Offset, Type, Method, Usage, UsageIndex
    static DXVertexElem TextInstance_Elements[] = 
    {
      {0, offsetof(Vertex_XY_UV, pos),     D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      {0, offsetof(Vertex_XY_UV, tex),     D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
      {1, offsetof(TextInstance, pos_sca), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
      {1, offsetof(TextInstance, tex_off), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
      D3DDECL_END()
    };


} // namespace Virtus
#endif // Include Guard