/*! ====================================================================================================================
  @file dx9quad.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.11.2011

  @brief
    Quad types... Unit, NDC Space, and Billboarded.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9QUAD_H
#define VIRTUS_DX9QUAD_H

#include "dx9.h"
#include "dx9buffers.h"

namespace Grimelins {

    class DX9Quad
    {
    public:
      DX9Quad( void );
      ~DX9Quad( void );

      PROPERTY_BASIC(DX9VertexBufferPtr, vbuffer, mVBuffer);
      PROPERTY_BASIC(DX9IndexBufferPtr,  ibuffer, mIBuffer);

    private:
      DX9VertexBufferPtr mVBuffer;
      DX9IndexBufferPtr  mIBuffer;
    };

} // namespace Virtus




#endif // Include Guard