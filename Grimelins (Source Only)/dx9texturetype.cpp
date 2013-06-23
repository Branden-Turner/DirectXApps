/*! ====================================================================================================================
  @file dx9texture.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.13.2011

  @brief
    Implementation for DirectX9 Texture Wrapper

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "dx9texturetype.h"

namespace Grimelins {

DX9Texture::DX9Texture( std::string const& file )
  : mFilename(file),
    mFilePath(file),
    mTextureID(-1),
    mTexture(NULL),
    mAnimated(false),
    mNumFrames(0),
    mFrameTime(0.0f)
{
}

DX9Texture::~DX9Texture( void )
{
}

DX9CubeTexture::DX9CubeTexture( std::string const& file )
  : mFilename(file),
    mFilePath(file),
    mTextureID(-1),
    mTexture(NULL)
{
}

DX9CubeTexture::~DX9CubeTexture( void )
{
}

} // namespace Virtus