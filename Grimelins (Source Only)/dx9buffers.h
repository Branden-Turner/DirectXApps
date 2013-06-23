/*! ====================================================================================================================
  @file dx9buffers.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.09.2011

  @brief
    Simple wrapper around stupid Directx9 Vertex buffers.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9BUFFERS_H
#define VIRTUS_DX9BUFFERS_H

#include "dx9.h"

namespace Grimelins {

    class DX9VertexBuffer
    {
    public:
      DX9VertexBuffer( void );
      DX9VertexBuffer( u32 size, u32 usage, u32 id, DXPool pool ); 
      ~DX9VertexBuffer( void );

      template<typename T>
      void Lock( T& pntr, DWORD flags = 0, u32 offset = 0, u32 size = 0 ) { HR( (*mBuffer)->Lock(offset,size,(void**)&pntr,flags) ); }
      void Unlock(void) { HR( (*mBuffer)->Unlock() ); }

      DXVertexBufferHndl mBuffer;
      DXPool mPool;
      u32 mSize;
      u32 mUsage;
      u32 mID;
    };

    class DX9IndexBuffer
    {
    public:
      DX9IndexBuffer( void );
      DX9IndexBuffer( u32 size, u32 usage, u32 id, DXPool pool, DXFormat format ); 
      ~DX9IndexBuffer( void );

      template<typename T>
      void Lock( T& pntr, u32 offset = 0, u32 size = 0, DWORD flags = 0 ) { HR( (*mBuffer)->Lock(offset,size,(void**)&pntr, flags) ); }
      void Unlock(void) { HR( (*mBuffer)->Unlock() ); }

      DXIndexBufferHndl mBuffer;
      DXPool mPool;
      DXFormat mFormat;
      u32 mSize;
      u32 mUsage;
      u32 mID;
    };

    typedef DX9IndexBuffer*  DX9IndexBufferPtr;
    typedef DX9VertexBuffer* DX9VertexBufferPtr;
} // namespace Virtus

#endif // Include Guard
