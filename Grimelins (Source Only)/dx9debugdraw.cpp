/*! ====================================================================================================================
  @file dx9debugdraw.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.18.2011

  @brief
    Simple debug draw system  

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "dx9debugdraw.h"

#include "dx9device.h"
#include "dx9shaders.h"
#include "dx9buffers.h"
#include "dx9vertextype.h"
#include "dx9resourcemanager.h"

namespace Grimelins {

DX9DebugDraw::DX9DebugDraw( DX9Device* device )
  : mDevice(device),
    mPShader(NULL),
    mVShader(NULL),
    mBuffer(NULL)
{
  //for(u32 i = 0; i < MaxColors; ++i)
  //  mLines[(Color)i].reserve(256);

  mPShader = mDevice->GetRM()->GetPixelShader("dx9debugline.ps");
  mVShader = mDevice->GetRM()->GetVertexShader("dx9debugline.vs");

  mBuffer = mDevice->GetRM()->GetVertexBuffer( DebugLineBuffSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DPOOL_DEFAULT );
}

DX9DebugDraw::~DX9DebugDraw( void )
{
}

void DX9DebugDraw::AddLine( vec3 const& pnta, vec3 const& pntb, Color col /*= Color::Green */ )
{
  mLines[col].push_back(pnta);
  mLines[col].push_back(pntb);
}

void DX9DebugDraw::PrepareDraw( DefaultVSData const& data )
{
  mDevice->SetShader( mVShader );
  mDevice->SetShader( mPShader );
  mDevice->SetDecl( Vertex_XYZ::Decl );
  mDevice->SetVSData( &data, sizeof(DefaultVSData) / 16 );
  mDevice->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void DX9DebugDraw::Draw( void )
{
  u32 RemainingBuffer = DebugLineBuffSize;

  for( LineDataIter ib = mLines.begin(), ie = mLines.end(); ib != ie; ++ib )
  {
    Lines& lines = ib->second;

    u32 SizeToCopy = lines.size() * sizeof(vec3);
    
      // No lines to draw, skip it
    if( SizeToCopy == 0 )
      continue;

      // Set the pixel shader color
    mDevice->SetPSData( &GetColorV(ib->first), 1, offsetof(DebugLinePSData, Color) / 16 );

    u1 multipleBatch = false;
    u32 Index = 0;

    do
    {
      multipleBatch = false;
      u32 DataCopy;
      
      if(SizeToCopy > RemainingBuffer) 
      {
        multipleBatch = true;
        SizeToCopy -= RemainingBuffer;
        DataCopy = RemainingBuffer;
      }
      else
      {
        DataCopy = SizeToCopy;
      }

      vec3* vec;
      mBuffer->Lock(vec, D3DLOCK_DISCARD);
      std::memcpy(vec, &lines[Index], DataCopy);
      mBuffer->Unlock();

      mDevice->SetBuffer( mBuffer, sizeof(vec3) );
      mDevice->DrawLines( ( ( DataCopy / sizeof(vec3) ) / 2 ) );

      Index += DataCopy / sizeof(vec3);

    } while(multipleBatch);
  }

  for( LineDataIter ib = mLines.begin(), ie = mLines.end(); ib != ie; ++ib )
  {
    Lines& lines = ib->second;
    lines.resize(0);
  }
}

} // namespace Virtus