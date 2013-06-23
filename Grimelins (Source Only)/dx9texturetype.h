/*! ====================================================================================================================
  @file dx9texturetype.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.13.2011

  @brief
    Wrapper for DirectX9 Texture Resource

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9TEXTURETYPE_H
#define VIRTUS_DX9TEXTURETYPE_H

#include "dx9.h"

namespace Grimelins {

    class Application::FilePath;

    class DX9Texture
    {
    public:

      DX9Texture( std::string const& file );
      ~DX9Texture( void );

      Application::FilePath mFilePath;
      DXImageInfo   mImageInfo;
      std::string   mFilename;
      u32           mTextureID;
      DXTextureHndl mTexture;

      //If we have an animated texture, we can use these :D 
      u1            mAnimated;
      u32           mNumFrames;
      f32           mFrameTime;
    };

    class DX9CubeTexture
    {
    public:
      DX9CubeTexture( std::string const& file );
      ~DX9CubeTexture( void );

      Application::FilePath mFilePath;
      DXImageInfo       mImageInfo;
      std::string       mFilename;
      u32               mTextureID;
      DXCubeTextureHndl mTexture;

    };
} // namespace Virtus



#endif // Include Guard