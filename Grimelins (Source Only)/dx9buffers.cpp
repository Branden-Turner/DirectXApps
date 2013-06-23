/*! ====================================================================================================================
  @file dx9buffers.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.09.2011

  @brief
    Wrapper around annoying DirectX buffer interface.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "dx9buffers.h"

namespace Grimelins {

DX9VertexBuffer::DX9VertexBuffer( void )
  : mBuffer(NULL),
    mPool(D3DPOOL_FORCE_DWORD),
    mSize(0),
    mUsage(0),
    mID(-1)
{
}

DX9VertexBuffer::DX9VertexBuffer( u32 size, u32 usage, u32 id, DXPool pool )
  : mBuffer(NULL),
    mPool(pool),
    mSize(size),
    mUsage(usage),
    mID(id)
{
}

DX9VertexBuffer::~DX9VertexBuffer( void )
{
}

DX9IndexBuffer::DX9IndexBuffer( void )
  : mBuffer(NULL),
    mPool(D3DPOOL_FORCE_DWORD),
    mSize(0),
    mUsage(0),
    mID(-1)
{
}

DX9IndexBuffer::DX9IndexBuffer( u32 size, u32 usage, u32 id, DXPool pool, DXFormat format )
  : mBuffer(NULL),
    mPool(pool),
    mFormat(format),
    mSize(size),
    mUsage(usage),
    mID(id)
{
}

DX9IndexBuffer::~DX9IndexBuffer( void )
{
}

} // namespace Virtus