/*! ====================================================================================================================
  @file dx9shaders.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.17.2011

  @brief
    DirectX9 Shader abstraction

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "dx9shaders.h"

namespace Grimelins {

DX9Shader::DX9Shader( std::string const& file )
  : mFilePath(file),
    mShaderFlags(0),
    mShaderID(-1),
    mDXShaderBuffer(NULL),
    mDXConstantTable(NULL)
{
  LoadFile();
}

DX9Shader::~DX9Shader( void )
{
  SafeRelease(mDXShaderBuffer);
  SafeRelease(mDXConstantTable);
}

void DX9Shader::LoadFile( void )
{
  std::string path = CONFIG_PATH_SHADER + mFilePath.Original();

  #if (VIRTUS_CONFIG_DEBUG_MODE)
    mShaderFlags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
  #else
    mShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
  #endif

  DXBuffer* mErrors = NULL;

  std::string version( mFilePath.Extension() );
  version += "_3_0";

  HRX(D3DXCompileShaderFromFile(
    path.c_str(),
    0,
    0,
    "main",
    version.c_str(),
    mShaderFlags,
    &mDXShaderBuffer,
    &mErrors,
    &mDXConstantTable),
    mErrors
    );
}


DX9PixelShader::DX9PixelShader( std::string const& file )
  : DX9Shader(file),
    mShader(NULL)
{
}

DX9PixelShader::~DX9PixelShader( void )
{
  SafeRelease(mShader);
}

DX9VertexShader::DX9VertexShader( std::string const& file )
  : DX9Shader(file),
    mShader(NULL)
{

}

DX9VertexShader::~DX9VertexShader( void )
{
  SafeRelease(mShader);
}

} // namespace Virtus
