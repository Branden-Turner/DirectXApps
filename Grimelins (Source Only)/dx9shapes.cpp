/*! ====================================================================================================================
  @file shapes.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.26.2011

  @brief
    Implementation for shape library

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "dx9shapes.h"
#include "dx9buffers.h"
#include "dx9resourcemanager.h"

namespace Grimelins {


Shape::Shape( ShapeType type, u32 tris, u32 vert, u16 indx, DXVertexDeclPtr decl )
  : mVBuffer(NULL),
    mIBuffer(NULL),
    mDecl(decl),
    mType(type),
    mNumTris(tris),
    mNumVerts(vert),
    mNumIndices(indx)
{
}


Shape::~Shape( void )
{

}


DX9Shapes::DX9Shapes( DX9ResourceManager* rm )
  : mRM(rm)
{
  std::memset( mShapes, 0, sizeof(mShapes) );

  Build2DQuad();
  BuildSkyBox();

  BuildPlane(1,1);
  BuildCube(1,1);
  BuildSphere(16,16);
  BuildTorus(16,16);
  BuildCylinder(1,16);
  BuildCone(16,16);
}

DX9Shapes::~DX9Shapes( void )
{
  for(u32 i = 0; i < Shapes_MAX; ++i)
    SafeDelete(mShapes[i]);
}

void DX9Shapes::BuildTorus( u32 stacks, u32 slices )
{
  Verts vtx;
  Idxs idx;

  f32 R = 0.35f;
  f32 r = 0.15f;

  for( u32 yrow = 0; yrow < stacks; ++yrow )
  {
    f32 yone = SCAST(f32, yrow) / SCAST(f32, stacks);
    f32 ytwo = SCAST(f32, yrow + 1) / SCAST(f32, stacks);

    f32 beta1 = (yone * PITWO);
    f32 beta2 = (ytwo * PITWO);

    f32 cosb1 = cosf(beta1);
    f32 sinb1 = sinf(beta1);
    f32 cosb2 = cosf(beta2);
    f32 sinb2 = sinf(beta2);

    for( u32 xrow  = 0; xrow <= slices; ++xrow )
    {
      f32 xone = SCAST(f32, xrow) / SCAST(f32, slices);
      f32 alph1 = xone * PITWO;

      f32 cosa1 = cosf(alph1);
      f32 sina1 = sinf(alph1);

      Vertex_XYZ_NRM_UV v1, v2;

      v1.pos = vec3( R * cosa1 + r * cosa1 * cosb1, r * sinb1, R * sina1 + r * sina1 * cosb1 );
      v1.nrm = v1.pos - vec3( R * cosa1, 0.0f, R * sina1 );
      v1.tex = vec2(xone, yone);
      v1.nrm.Normalize();

      v2.pos = vec3( R * cosa1 + r * cosa1 * cosb2, r * sinb2, R * sina1 + r * sina1 * cosb2 );
      v2.nrm = v2.pos - vec3( R * cosa1, 0.0f, R * sina1 );
      v2.tex = vec2(xone, ytwo);
      v2.nrm.Normalize();

      vtx.push_back(v1);
      vtx.push_back(v2);
    }
  }

  BuildIndexBuffer( stacks, slices, idx );
  AddShape( ShapeTorus, vtx, idx);
}

void DX9Shapes::BuildSphere( u32 stacks, u32 slices )
{
  Verts vtx;
  Idxs idx;

  f32 radi = 0.5f;

  for( u32 yrow = 0; yrow < stacks; ++yrow )
  {
    f32 yone = SCAST(f32, yrow) / SCAST(f32, stacks);
    f32 ytwo = SCAST(f32, yrow + 1) / SCAST(f32, stacks);

    f32 beta1 = halfPI - (yone * PI);
    f32 beta2 = halfPI - (ytwo * PI);

    f32 cosb1 = cosf(beta1);
    f32 sinb1 = sinf(beta1);
    f32 cosb2 = cosf(beta2);
    f32 sinb2 = sinf(beta2);

    for( u32 xrow  = 0; xrow <= slices; ++xrow )
    {
      f32 xone = SCAST(f32, xrow) / SCAST(f32, slices);
      f32 alph1 = xone * PITWO;

      f32 cosa1 = cosf(alph1);
      f32 sina1 = sinf(alph1);

      Vertex_XYZ_NRM_UV v1, v2;

      v1.pos = vec3( radi * sina1 * cosb1, radi * sinb1, radi * cosa1 * cosb1 );
      v1.nrm = v1.pos;
      v1.tex = vec2(xone, yone);
      v1.nrm.Normalize();

      v2.pos = vec3( radi * sina1 * cosb2, radi * sinb2, radi * cosa1 * cosb2 );
      v2.nrm = v2.pos;
      v2.tex = vec2(xone, ytwo);
      v2.nrm.Normalize();

      vtx.push_back(v1);
      vtx.push_back(v2);
    }
  }

  BuildIndexBuffer( stacks, slices, idx );
  AddShape( ShapeSphere, vtx, idx );
}


void DX9Shapes::BuildCone( u32 stacks, u32 slices )
{
  Verts vtx;
  Idxs idx;

  f32 radi = 0.5f;

  for( u32 yrow = 0; yrow < stacks; ++yrow )
  {
    f32 yone = SCAST(f32, yrow) / SCAST(f32, stacks);
    f32 ytwo = SCAST(f32, yrow + 1) / SCAST(f32, stacks);

    for( u32 xrow  = 0; xrow <= slices; ++xrow )
    {
      f32 xone = SCAST(f32, xrow) / SCAST(f32, slices);
      f32 alph1 = xone * PITWO;

      f32 cosa1 = cosf(alph1);
      f32 sina1 = sinf(alph1);

      Vertex_XYZ_NRM_UV v1, v2;

      v1.pos = vec3( radi * sina1 * yone, 0.5f - yone, radi * cosa1 * yone );
      v1.nrm = vec3( radi * sina1, 0.5f - yone, radi * cosa1 ) - vec3( 0.0f, 0.5f - yone, 0.0f );;
      v1.tex = vec2(xone, yone);
      v1.nrm.Normalize();

      v2.pos = vec3( radi * sina1 * ytwo, 0.5f - ytwo, radi * cosa1 * ytwo );
      v2.nrm = vec3( radi * sina1, 0.5f - ytwo, radi * cosa1 ) - vec3( 0.0f, 0.5f - ytwo, 0.0f );;
      v2.tex = vec2(xone, ytwo);
      v2.nrm.Normalize();

      vtx.push_back(v1);
      vtx.push_back(v2);
    }
  }

  BuildIndexBuffer( stacks, slices, idx );

  Verts topV;

  // Now we have the cylinder wrapping around, we need to add the top/bottom caps.

  Vertex_XYZ_NRM_UV tPnt;

  tPnt.pos = vec3(0.0f, -0.5f, 0.0f);
  tPnt.nrm = vec3(0.0f, -1.0f, 0.0f);
  tPnt.tex = vec2(0.5f, 0.5f);

  u32 lastVertRowIndex = ( (stacks - 1) * ( (slices + 1) * 2 ) ) + 1;

  u32 lastIndex = vtx.size();
  vtx.push_back(tPnt);

  for( u32 i = 0; i < slices; ++i)
  {
    u32 topIndex = lastVertRowIndex + i * 2;

    Vertex_XYZ_NRM_UV tmp = vtx[topIndex];
    tmp.nrm = vec3(0.0f, -1.0f, 0.0f);
    vtx.push_back(tmp);

    idx.push_back( lastIndex );
    idx.push_back( lastIndex + ( (i + 1) % slices) + 1 );
    idx.push_back( lastIndex + i + 1 );


    u32 index = lastIndex + i + 1;
    vtx[index].tex = vec2( vtx[index].pos.x + 0.5f, vtx[index].pos.z + 0.5f );
  }

  AddShape( ShapeCone, vtx, idx );
}


void DX9Shapes::BuildCylinder( u32 stacks, u32 slices )
{
  Verts vtx;
  Idxs idx;

  f32 radi = 0.5f;

  for( u32 yrow = 0; yrow < stacks; ++yrow )
  {
    f32 yone = SCAST(f32, yrow) / SCAST(f32, stacks);
    f32 ytwo = SCAST(f32, yrow + 1) / SCAST(f32, stacks);

    for( u32 xrow  = 0; xrow <= slices; ++xrow )
    {
      f32 xone = SCAST(f32, xrow) / SCAST(f32, slices);
      f32 alph1 = xone * PITWO;

      f32 cosa1 = cosf(alph1);
      f32 sina1 = sinf(alph1);

      Vertex_XYZ_NRM_UV v1, v2;

      v1.pos = vec3( radi * sina1, 0.5f - yone, radi * cosa1 );
      v1.nrm = v1.pos - vec3( 0.0f, 0.5f - yone, 0.0f );;
      v1.tex = vec2(xone, yone);
      v1.nrm.Normalize();

      v2.pos = vec3( radi * sina1, 0.5f - ytwo, radi * cosa1 );
      v2.nrm = v2.pos - vec3( 0.0f, 0.5f - ytwo, 0.0f );;
      v2.tex = vec2(xone, ytwo);
      v2.nrm.Normalize();

      vtx.push_back(v1);
      vtx.push_back(v2);
    }
  }

  BuildIndexBuffer( stacks, slices, idx );

  Verts topV;

  // Now we have the cylinder wrapping around, we need to add the top/bottom caps.

  Vertex_XYZ_NRM_UV tPnt[2];

  tPnt[0].pos = vec3(0.0f, 0.5f, 0.0f);
  tPnt[0].nrm = vec3(0.0f, 1.0f, 0.0f);
  tPnt[0].tex = vec2(0.5f, 0.5f);

  tPnt[1].pos = vec3(0.0f, -0.5f, 0.0f);
  tPnt[1].nrm = vec3(0.0f, -1.0f, 0.0f);
  tPnt[1].tex = vec2(0.5f, 0.5f);

  u32 lastVertRowIndex = 0; 
  f32 norm = 1.0f;

  for( u32 endCap = 0; endCap < 2; ++endCap)
  {
    u32 lastIndex = vtx.size();
    vtx.push_back(tPnt[endCap]);

      for( u32 i = 0; i < slices; ++i)
      {
        u32 topIndex = lastVertRowIndex + i * 2;

        Vertex_XYZ_NRM_UV tmp = vtx[topIndex];
        tmp.nrm = vec3(0.0f, norm, 0.0f);
        vtx.push_back(tmp);

        u32 secIndex = (endCap == 0) ? i + 1 : ( (i + 1) % slices) + 1;
        u32 thrIndex = (endCap == 1) ? i + 1 : ( (i + 1) % slices) + 1;

        idx.push_back( lastIndex );
        idx.push_back( lastIndex + secIndex );
        idx.push_back( lastIndex + thrIndex );

        u32 index = lastIndex + i + 1;
        vtx[index].tex = vec2( vtx[index].pos.x + 0.5f, vtx[index].pos.z + 0.5f );
      }

    norm *= -1.0f;
    lastVertRowIndex = ( (stacks - 1) * ( (slices + 1) * 2 ) ) + 1;
  }

  AddShape( ShapeCylinder, vtx, idx );
}

void DX9Shapes::BuildCube( u32 stacks, u32 slices )
{
  Verts vtx;
  Idxs idx;

  for( u32 yrow = 0; yrow < stacks; ++yrow )
  {
    f32 yone = SCAST(f32, yrow) / SCAST(f32, stacks);
    f32 ytwo = SCAST(f32, yrow + 1) / SCAST(f32, stacks);

    for( u32 xrow  = 0; xrow <= slices; ++xrow )
    {
      f32 xone = SCAST(f32, xrow) / SCAST(f32, slices);

      Vertex_XYZ_NRM_UV v1, v2;

      v1.pos = vec3(xone - 0.5f, 0.5f - yone, 0.0f);
      v1.nrm = vec3(0.0f, 0.0f, 1.0f);
      v1.tex = vec2(xone, yone);

      v2.pos = vec3(xone - 0.5f, 0.5f - ytwo, 0.0f);
      v2.nrm = vec3(0.0f, 0.0f, 1.0f);
      v2.tex = vec2(xone, ytwo);

      vtx.push_back(v1);
      vtx.push_back(v2);
    }
  }

  BuildIndexBuffer( stacks, slices, idx );

  u32 numVertex = vtx.size();
  u16 numIndices = idx.size();

  Matrix4 tmpMat = Matrix4::Translation(0.5f, 0.0f, 0.0f) * Matrix4::RotationY(halfPI);

  for(u32 i = 0; i < numVertex; ++i )
  {
    Vertex_XYZ_NRM_UV tmp;

    tmp.pos = tmpMat.MulPnt( vtx[i].pos );
    tmp.nrm = tmpMat.MulVec( vtx[i].nrm );
    tmp.tex = vtx[i].tex;

    vtx.push_back( tmp );
  }
  
  for(u32 i = 0; i < numIndices; ++i ) 
  {
    idx.push_back( idx[i] + numVertex);
  }

  tmpMat = Matrix4::Translation(0.0f, 0.0f, -0.5f) * Matrix4::RotationY(halfPI * 2.0f);

  for(u32 i = 0; i < numVertex; ++i )
  {
    Vertex_XYZ_NRM_UV tmp;

    tmp.pos = tmpMat.MulPnt( vtx[i].pos );
    tmp.nrm = tmpMat.MulVec( vtx[i].nrm );
    tmp.tex = vtx[i].tex;

    vtx.push_back( tmp );
  }

  for(u32 i = 0; i < numIndices; ++i ) 
  {
    idx.push_back( idx[i] + numVertex * 2);
  }

  tmpMat = Matrix4::Translation(-0.5f, 0.0f, 0.0f) * Matrix4::RotationY(halfPI * 3.0f);

  for(u32 i = 0; i < numVertex; ++i )
  {
    Vertex_XYZ_NRM_UV tmp;

    tmp.pos = tmpMat.MulPnt( vtx[i].pos );
    tmp.nrm = tmpMat.MulVec( vtx[i].nrm );
    tmp.tex = vtx[i].tex;

    vtx.push_back( tmp );
  }

  for(u32 i = 0; i < numIndices; ++i ) 
  {
    idx.push_back( idx[i] + numVertex * 3);
  }

  tmpMat = Matrix4::Translation(0.0f, 0.5f, 0.0f) * Matrix4::RotationX(halfPI * 3.0f);

  for(u32 i = 0; i < numVertex; ++i )
  {
    Vertex_XYZ_NRM_UV tmp;

    tmp.pos = tmpMat.MulPnt( vtx[i].pos );
    tmp.nrm = tmpMat.MulVec( vtx[i].nrm );
    tmp.tex = vtx[i].tex;

    vtx.push_back( tmp );
  }

  for(u32 i = 0; i < numIndices; ++i ) 
  {
    idx.push_back( idx[i] + numVertex * 4);
  }

  tmpMat = Matrix4::Translation(0.0f, -0.5f, 0.0f) * Matrix4::RotationX(halfPI);

  for(u32 i = 0; i < numVertex; ++i )
  {
    Vertex_XYZ_NRM_UV tmp;

    tmp.pos = tmpMat.MulPnt( vtx[i].pos );
    tmp.nrm = tmpMat.MulVec( vtx[i].nrm );
    tmp.tex = vtx[i].tex;

    vtx.push_back( tmp );
  }

  for(u32 i = 0; i < numIndices; ++i ) 
  {
    idx.push_back( idx[i] + numVertex * 5);
  }

  tmpMat = Matrix4::Translation(0.0f, 0.0f, 0.5f);

  for(u32 i = 0; i < numVertex; ++i )
  {
    vtx[i].pos = tmpMat.MulPnt( vtx[i].pos );
    vtx[i].nrm = tmpMat.MulVec( vtx[i].nrm );
  }

  AddShape( ShapeCube, vtx, idx );
}

void DX9Shapes::BuildPlane( u32 stacks, u32 slices )
{
  Verts vtx;
  Idxs idx;

  for( u32 yrow = 0; yrow < stacks; ++yrow )
  {
    f32 yone = SCAST(f32, yrow) / SCAST(f32, stacks);
    f32 ytwo = SCAST(f32, yrow + 1) / SCAST(f32, stacks);

    for( u32 xrow  = 0; xrow <= slices; ++xrow )
    {
      f32 xone = SCAST(f32, xrow) / SCAST(f32, slices);

      Vertex_XYZ_NRM_UV v1, v2;

      v1.pos = vec3(xone - 0.5f, 0.5f - yone, 0.0f);
      v1.nrm = vec3(0.0f, 0.0f, 1.0f);
      v1.tex = vec2(xone, yone);

      v2.pos = vec3(xone - 0.5f, 0.5f - ytwo, 0.0f);
      v2.nrm = vec3(0.0f, 0.0f, 1.0f);
      v2.tex = vec2(xone, ytwo);

      vtx.push_back(v1);
      vtx.push_back(v2);
    }
  }

  BuildIndexBuffer( stacks, slices, idx );
  AddShape( ShapePlane, vtx, idx );
}

void DX9Shapes::BuildSkyBox( void )
{
  ShapePtr shape = new Shape(ShapeSkybox, 12, 8, 36, Vertex_XYZ::Decl);

  shape->mVBuffer = mRM->GetVertexBuffer( sizeof(Vertex_XYZ) * 8, D3DUSAGE_WRITEONLY );
  shape->mIBuffer = mRM->GetIndexBuffer( sizeof(u16) * 36, D3DUSAGE_WRITEONLY );

  Vertex_XYZ* vert = NULL;
  shape->mVBuffer->Lock(vert);

  vert[0].pos = vec3( 0.5f,  0.5f, -0.5f);
  vert[1].pos = vec3(-0.5f,  0.5f, -0.5f);
  vert[2].pos = vec3(-0.5f, -0.5f, -0.5f);
  vert[3].pos = vec3( 0.5f, -0.5f, -0.5f);

  vert[4].pos = vec3( 0.5f,  0.5f,  0.5f);
  vert[5].pos = vec3(-0.5f,  0.5f,  0.5f);
  vert[6].pos = vec3(-0.5f, -0.5f,  0.5f);
  vert[7].pos = vec3( 0.5f, -0.5f,  0.5f);

  shape->mVBuffer->Unlock();

  u16* index = NULL;
  shape->mIBuffer->Lock(index);

  index[0] = 1;
  index[1] = 3;
  index[2] = 0;

  index[3] = 1;
  index[4] = 2;
  index[5] = 3;

  index[6] = 4;
  index[7] = 1;
  index[8] = 0;

  index[9]  = 4;
  index[10] = 5;
  index[11] = 1;

  index[12] = 5;
  index[13] = 4;
  index[14] = 7;

  index[15] = 5;
  index[16] = 7;
  index[17] = 6;

  index[18] = 1;
  index[19] = 6;
  index[20] = 5;

  index[21] = 1;
  index[22] = 2;
  index[23] = 6;

  index[24] = 0;
  index[25] = 3;
  index[26] = 7;

  index[27] = 0;
  index[28] = 7;
  index[29] = 4;

  index[30] = 2;
  index[31] = 6;
  index[32] = 7;

  index[33] = 2;
  index[34] = 7;
  index[35] = 3;

  shape->mIBuffer->Unlock();

  mShapes[ShapeSkybox] = shape;
}

void DX9Shapes::Build2DQuad( void )
{
  ShapePtr shape = new Shape(ShapeQuad2D, 2, 4, 6, Vertex_XY_UV::Decl);

  shape->mVBuffer = mRM->GetVertexBuffer( sizeof(Vertex_XY_UV) * 4, D3DUSAGE_WRITEONLY );
  shape->mIBuffer = mRM->GetIndexBuffer( sizeof(u16) * 6, D3DUSAGE_WRITEONLY );

  // Load up the quad's 4 vertexes.
  shape->vtx2D.resize(4);
  shape->vtx2D[0].pos = vec2(-0.5f,  0.5f);
  shape->vtx2D[0].tex = vec2( 0.0f,  0.0f);
  shape->vtx2D[1].pos = vec2(-0.5f, -0.5f);
  shape->vtx2D[1].tex = vec2( 0.0f,  1.0f);
  shape->vtx2D[2].pos = vec2( 0.5f,  0.5f);
  shape->vtx2D[2].tex = vec2( 1.0f,  0.0f);
  shape->vtx2D[3].pos = vec2( 0.5f, -0.5f);
  shape->vtx2D[3].tex = vec2( 1.0f,  1.0f);

  Vertex_XY_UV* vert = NULL;
  shape->mVBuffer->Lock(vert);

  std::memcpy(vert, &shape->vtx2D[0], sizeof(Vertex_XY_UV) * 4);

  shape->mVBuffer->Unlock();

  // Load up the quad's index buffer.
  u16* index = NULL;
  shape->mIBuffer->Lock(index);

  index[0] = 1;
  index[1] = 0;
  index[2] = 2;
  index[3] = 3;
  index[4] = 1;
  index[5] = 2;

  shape->mIBuffer->Unlock();

  mShapes[ShapeQuad2D] = shape;
}


void DX9Shapes::BuildIndexBuffer( u32 stacks, u32 slices, Idxs& idx )
{
  for(u32 i = 0; i < stacks; ++i)
  {
    for(u32 j = 0; j < slices; ++j)
    {
      u32 stride = (slices + 1) * 2;
      u32 offset = j * 2;

      idx.push_back( i * stride + offset + 2 );
      idx.push_back( i * stride + offset + 0 );
      idx.push_back( i * stride + offset + 1 );

      idx.push_back( i * stride + offset + 2 );
      idx.push_back( i * stride + offset + 1 );
      idx.push_back( i * stride + offset + 3 );
    }
  }
}

void DX9Shapes::AddShape( ShapeType type, Verts const& vertexes, Idxs const& indices, DXVertexDeclPtr decl )
{
  ShapePtr shape = new Shape(type, indices.size() / 3, vertexes.size(), indices.size(), decl);

  u32 vBuffSize = sizeof(Vertex_XYZ_NRM_UV) * shape->mNumVerts;
  u32 iBuffSize = sizeof(u16) * shape->mNumIndices;

  shape->mVBuffer = mRM->GetVertexBuffer( vBuffSize, D3DUSAGE_WRITEONLY );
  shape->mIBuffer = mRM->GetIndexBuffer( iBuffSize, D3DUSAGE_WRITEONLY );

  shape->vtx3D = vertexes;

  void* vbp = NULL;
  shape->mVBuffer->Lock(vbp);

  std::memcpy(vbp, &shape->vtx3D[0], vBuffSize);

  shape->mVBuffer->Unlock();

  void* ibp = NULL;
  shape->mIBuffer->Lock(ibp);

  std::memcpy(ibp, &indices[0], iBuffSize);

  shape->mIBuffer->Unlock();

  mShapes[type] = shape;
}

ShapePtr DX9Shapes::GetShape( ShapeType type )
{
  return mShapes[type];
}
} // namespace Virtus