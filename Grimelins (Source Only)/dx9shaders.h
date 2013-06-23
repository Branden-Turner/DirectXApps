/*! ====================================================================================================================
  @file dx9shaders.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.17.2011

  @brief
    DirectX9 Shader abstraction

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9SHADERS_H
#define VIRTUS_DX9SHADERS_H

#include "dx9.h"

namespace Grimelins {

    class Application::FilePath;

    ///////////////////////////////////////
    // DX9 Shader Base Class
    ///////////////////////////////////////

    class DX9Shader
    {

    public:
      DX9Shader(std::string const& file);
      virtual ~DX9Shader(void);

      void LoadFile(void);

      Application::FilePath mFilePath;
      std::string           mFilename;
      u32              mShaderFlags;
      u32              mShaderID;
      DXBuffer*        mDXShaderBuffer;
      DXTable*         mDXConstantTable;
    };

    ///////////////////////////////////////
    // DX9 Pixel Shader
    ///////////////////////////////////////

    class DX9PixelShader : public DX9Shader
    {

    public:
      DX9PixelShader(std::string const& file);
      virtual ~DX9PixelShader(void);

      DXPixelShaderPtr mShader;
    };

    ///////////////////////////////////////
    // DX9 Vertex Shader
    ///////////////////////////////////////

    class DX9VertexShader : public DX9Shader
    {

    public:
      DX9VertexShader(std::string const& file);
      virtual ~DX9VertexShader(void);

      DXVertexShaderPtr mShader;
    };

}// namespace Virtus

#include "shaderdata.h"

#endif
