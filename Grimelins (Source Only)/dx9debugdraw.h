/*! ====================================================================================================================
  @file dx9debugdraw.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.18.2011

  @brief
    Simple debug-drawerizer

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9DEBUGDRAW_H
#define VIRTUS_DX9DEBUGDRAW_H

#include "dx9.h"
#include "dx9shaders.h"
#include "color.h"

namespace Grimelins {

      // 1024 lines per batch.
    const u32 DebugLineBuffSize = 2048 * sizeof(vec3);

    class DX9Device;
    class DX9PixelShader;
    class DX9VertexShader;
    class DX9VertexBuffer;

    class DX9DebugDraw
    {
    public:
      DX9DebugDraw( DX9Device* device );
      ~DX9DebugDraw( void );

      void AddLine( vec3 const& pnta, vec3 const& pntb, Color col = Green );
      void PrepareDraw( DefaultVSData const& data );
      void Draw( void );

    private:
      void SendBatch( void );

      typedef std::vector<vec3> Lines;
      typedef std::map< Color, Lines > LineData;
      typedef LineData::const_iterator LineDataConstIter;
      typedef LineData::iterator LineDataIter;

    private:
      DX9Device*       mDevice;
      DX9PixelShader*  mPShader;
      DX9VertexShader* mVShader;
      DX9VertexBuffer* mBuffer;
      LineData         mLines;
    };

} // namespace Virtus

#endif // Include Guard
