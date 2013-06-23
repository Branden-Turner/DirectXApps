/*! ====================================================================================================================
  @file dx9rendertargets.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.17.2011

  @brief
    Some DirectX 9 Render Targets

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9RENDERTARGETS_H
#define VIRTUS_DX9RENDERTARGETS_H

#include "dx9.h"

namespace Grimelins
{
    class DX9RenderTarget
    {
    public:
      DX9RenderTarget(DXSurface* surface, u32 flags, u32 color, u32 stencil, f32 depth);
      ~DX9RenderTarget(void);

    public:
      DXSurface* mSurface;
      DXSurfaceDesc mSurfaceDesc;

      std::string mName;
      u32 mWidth;
      u32 mHeight;

      u32 mBufferFlags;
      u32 mStencilValue;
      u32 mColorValue;
      f32 mDepthValue;
    };
}// namespace Virtus

#endif
