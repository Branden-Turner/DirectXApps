/*! ====================================================================================================================
  @file dx9rendertargets.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.17.2011

  @brief
    Implementation for DirectX9 Render Targets

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

#include "precompiled.h"
#include "dx9rendertargets.h"

namespace Grimelins {

DX9RenderTarget::DX9RenderTarget(DXSurface* surface, u32 flags, u32 color, u32 stencil, f32 depth)
  : mSurface(surface),
    mName("DX9RenderTarget"),
    mWidth(0),
    mHeight(0)
{
  mBufferFlags = flags;
  mColorValue = color;
  mStencilValue = stencil;
  mDepthValue = depth;

  ZeroMemory( &mSurfaceDesc, sizeof(DXSurfaceDesc) );

  mSurface->GetDesc( &mSurfaceDesc );
  mWidth = mSurfaceDesc.Width;
  mHeight = mSurfaceDesc.Height;
}

DX9RenderTarget::~DX9RenderTarget(void)
{
  SafeRelease(mSurface);
}

}// namespace Virtus
