/*! ====================================================================================================================
  @file dx9resourcemanager.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.24.2011

  @brief
    DirectX9 Resource Manager

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "dx9resourcemanager.h"
#include "dx9buffers.h"
#include "dx9shaders.h"
#include "dx9texturetype.h"
#include "dx9vertextype.h"
#include "dx9quad.h"

namespace Grimelins { 

u32 DX9ResourceManager::mVertexBufferID = 0;
u32 DX9ResourceManager::mIndexBufferID = 0;
u32 DX9ResourceManager::mTextureID = 0;
u32 DX9ResourceManager::mShaderID = 0;
u32 DX9ResourceManager::mCubeTextureID = 0;
u32 DX9ResourceManager::m2DAnimID = 0;

const u32 ReserveSize = 256;

DX9ResourceManager::DX9ResourceManager(DXDevice* dxdevice)
  : mDXDevice(dxdevice)
{
  mShaderData.set_empty_key(std::string(""));

  mDX9VBuffers.reserve(ReserveSize);
  mDX9IBuffers.reserve(ReserveSize);
  mDX9Textures.reserve(ReserveSize);
  mDX9CubeTextures.reserve(ReserveSize);

  mVBuffers.reserve(ReserveSize);
  mIBuffers.reserve(ReserveSize);
  mTextures.reserve(ReserveSize);
  mCubeTextures.reserve(ReserveSize);
}

DX9ResourceManager::~DX9ResourceManager(void)
{
  MapDeleteAndClear(mShaderData);

  DeleteAndClear(mDX9VBuffers);
  DeleteAndClear(mDX9IBuffers);
  DeleteAndClear(mDX9InstanceVBuffers);
  DeleteAndClear(mDX9InstanceIBuffers);
  DeleteAndClear(mDX9Textures);
  DeleteAndClear(mDX9CubeTextures);
  DeleteAndClear(mDX92DAnims);

  ReleaseContainer(mVBuffers);
  ReleaseContainer(mIBuffers);
  ReleaseContainer(mTextures);
  ReleaseContainer(mCubeTextures);
  ReleaseContainer(m2DAnims);
}

DX9CubeTexture* DX9ResourceManager::GetCubeTextureFromFile( std::string const& file )
{
  DX9CubeTexture* dxtex;

  //Search for an already created file
  for(u32 i = 0; i < mDX9CubeTextures.size(); ++i)
  {
    //If you find it, just give the texture ID and texture to this current texture.
    if(file == mDX9CubeTextures[i]->mFilename)
    {
      dxtex = mDX9CubeTextures[i];
      return dxtex;
    }
  }

  dxtex = new DX9CubeTexture( file );

  dxtex->mTextureID = mCubeTextureID++;

  DXCubeTexture* tex = NULL;

  //HRESULT D3DXCreateCubeTextureFromFileEx(
  //  __in   LPDIRECT3DDEVICE9 pDevice,
  //  __in   LPCTSTR pSrcFile,
  //  __in   UINT Size,
  //  __in   UINT MipLevels,
  //  __in   DWORD Usage,
  //  __in   D3DFORMAT Format,
  //  __in   D3DPOOL Pool,
  //  __in   DWORD Filter,
  //  __in   DWORD MipFilter,
  //  __in   D3DCOLOR ColorKey,
  //  __out  D3DXIMAGE_INFO *pSrcInfo,
  //  __out  PALETTEENTRY *pPalette,
  //  __out  LPDIRECT3DCUBETEXTURE9 *ppCubeTexture
  //  );

  HR( D3DXCreateCubeTextureFromFileEx(
    mDXDevice,
    (dxtex->mFilename).c_str(),
    0,
    0,
    0,
    D3DFMT_UNKNOWN,
    D3DPOOL_MANAGED,
    D3DX_DEFAULT,
    D3DX_DEFAULT,
    0,
    &(dxtex->mImageInfo),
    NULL,
    &tex ) );

  mCubeTextures.push_back(tex);

  dxtex->mTexture = &mCubeTextures[dxtex->mTextureID];

  mDX9CubeTextures.push_back(dxtex);

  return dxtex;
}


DX9Texture* DX9ResourceManager::GetSpriteSheetAnimFromFile( std::string const& file )
{
  DX9Texture* dxtex;

  //Grab the pertinent info out of the file
  Text img, frames, frameTime;
  std::ifstream myfile (file.c_str());

  //Grab the data from the anim file
  if(myfile.is_open())
  {
    getline(myfile, img);
    getline(myfile, frames);
    getline(myfile, frameTime);
  }

  Text imgFile(CONFIG_PATH_TEXTURE);

  imgFile.append("anim/");
  imgFile.append(img.c_str());

  //Search for an already created file
  for(u32 i = 0; i < mDX92DAnims.size(); ++i)
  {
    //If you find it, just give the texture ID and texture to this current texture.
    if(imgFile == mDX92DAnims[i]->mFilename)
    {
      dxtex = mDX92DAnims[i];
      return dxtex;
    }
  }

  dxtex = new DX9Texture( imgFile );

  dxtex->mTextureID = m2DAnimID++;

  DXTexture* tex = NULL;

  //HRESULT D3DXCreateTextureFromFileEx(
  //  __in     LPDIRECT3DDEVICE9 pDevice,
  //  __in     LPCTSTR pSrcFile,
  //  __in     UINT Width,
  //  __in     UINT Height,
  //  __in     UINT MipLevels,
  //  __in     DWORD Usage,
  //  __in     D3DFORMAT Format,
  //  __in     D3DPOOL Pool,
  //  __in     DWORD Filter,
  //  __in     DWORD MipFilter,
  //  __in     D3DCOLOR ColorKey,
  //  __inout  D3DXIMAGE_INFO *pSrcInfo,
  //  __out    PALETTEENTRY *pPalette,
  //  __out    LPDIRECT3DTEXTURE9 *ppTexture
  //  );

  HR( D3DXCreateTextureFromFileEx( 
    mDXDevice,
    (dxtex->mFilename).c_str(),
    D3DX_DEFAULT_NONPOW2,
    D3DX_DEFAULT_NONPOW2,
    0,
    0,
    D3DFMT_UNKNOWN,
    D3DPOOL_MANAGED,
    D3DX_DEFAULT,
    D3DX_DEFAULT,
    0,
    &(dxtex->mImageInfo),
    NULL,
    &tex ) );

  m2DAnims.push_back(tex);

  dxtex->mTexture = &m2DAnims[dxtex->mTextureID];
  dxtex->mNumFrames = atoi(frames.c_str());
  dxtex->mFrameTime = SCAST(f32, atof(frameTime.c_str()));

  mDX92DAnims.push_back(dxtex);

  return dxtex;
}


DX9Texture* DX9ResourceManager::GetTextureFromFile( std::string const& file )
{
  DX9Texture* dxtex;

  //Search for an already created file
  for(u32 i = 0; i < mDX9Textures.size(); ++i)
  {
    //If you find it, just give the texture ID and texture to this current texture.
    if(file == mDX9Textures[i]->mFilename)
    {
      dxtex = mDX9Textures[i];
      return dxtex;
    }
  }

  dxtex = new DX9Texture( file );

  dxtex->mTextureID = mTextureID++;

  DXTexture* tex = NULL;

  //HRESULT D3DXCreateTextureFromFileEx(
  //  __in     LPDIRECT3DDEVICE9 pDevice,
  //  __in     LPCTSTR pSrcFile,
  //  __in     UINT Width,
  //  __in     UINT Height,
  //  __in     UINT MipLevels,
  //  __in     DWORD Usage,
  //  __in     D3DFORMAT Format,
  //  __in     D3DPOOL Pool,
  //  __in     DWORD Filter,
  //  __in     DWORD MipFilter,
  //  __in     D3DCOLOR ColorKey,
  //  __inout  D3DXIMAGE_INFO *pSrcInfo,
  //  __out    PALETTEENTRY *pPalette,
  //  __out    LPDIRECT3DTEXTURE9 *ppTexture
  //  );

  HR( D3DXCreateTextureFromFileEx( 
    mDXDevice,
    (dxtex->mFilename).c_str(),
    0,
    0,
    0,
    0,
    D3DFMT_UNKNOWN,
    D3DPOOL_MANAGED,
    D3DX_DEFAULT,
    D3DX_DEFAULT,
    0,
    &(dxtex->mImageInfo),
    NULL,
    &tex ) );

  mTextures.push_back(tex);

  dxtex->mTexture = &mTextures[dxtex->mTextureID];

  mDX9Textures.push_back(dxtex);

  return dxtex;
}

DX9VertexBuffer* DX9ResourceManager::GetVertexBuffer( u32 size, u32 usageFlags, DXPool poolFlags )
{
  DX9VertexBuffer* dxvb = new DX9VertexBuffer(size, usageFlags, mVertexBufferID++, poolFlags);

  DXVertexBuffer* tmp = NULL;
  mVBuffers.push_back(tmp);

  HR( mDXDevice->CreateVertexBuffer(size, usageFlags, 0, poolFlags, &mVBuffers[dxvb->mID], 0) );

  dxvb->mBuffer = &mVBuffers[dxvb->mID];

  if(poolFlags == D3DPOOL_DEFAULT)
    mDX9InstanceVBuffers.push_back(dxvb);
  else
    mDX9VBuffers.push_back(dxvb);

  return dxvb;
}
DX9IndexBuffer* DX9ResourceManager::GetIndexBuffer( u32 size, u32 usageFlags, DXFormat format, DXPool poolFlags )
{
  DX9IndexBuffer* dxib = new DX9IndexBuffer(size, usageFlags, mIndexBufferID++, poolFlags, format);

  DXIndexBuffer* tmp = NULL;
  mIBuffers.push_back(tmp);

  HR( mDXDevice->CreateIndexBuffer(size, usageFlags, format, poolFlags, &mIBuffers[dxib->mID], 0) );

  dxib->mBuffer = &mIBuffers[dxib->mID];

  if(poolFlags == D3DPOOL_DEFAULT)
    mDX9InstanceIBuffers.push_back(dxib);
  else
    mDX9IBuffers.push_back(dxib);

  return dxib;
}


DX9PixelShader* DX9ResourceManager::GetPixelShader( const std::string& filename )
{
  return RCAST( DX9PixelShader*, GetShader(filename) );
}

DX9VertexShader* DX9ResourceManager::GetVertexShader( const std::string& filename )
{
  return RCAST( DX9VertexShader*, GetShader(filename) );
}

DX9Shader* DX9ResourceManager::GetShader( const std::string& filename )
{
  Application::FilePath fp(filename);

  ShaderConstIter shaderIt = mShaderData.find(filename);

  if( shaderIt == mShaderData.end() )
  {
    std::string const& ext = fp.Extension();

    if( ext == "vs" )
    {
      DX9VertexShader* shaderPntr = new DX9VertexShader(filename);

      HR(mDXDevice->CreateVertexShader( 
        RCAST( DWORD*, shaderPntr->mDXShaderBuffer->GetBufferPointer() ),
        &shaderPntr->mShader  ) 
        );

      mShaderData[filename] = shaderPntr;
      return shaderPntr;

    }
    else if ( ext == "ps" )
    {
      DX9PixelShader* shaderPntr = new DX9PixelShader(filename);

      HR(mDXDevice->CreatePixelShader( 
        RCAST( DWORD*, shaderPntr->mDXShaderBuffer->GetBufferPointer() ),
        &shaderPntr->mShader  ) 
        );

      mShaderData[filename] = shaderPntr;
      return shaderPntr;
    }
    else 
      return NULL;
  }
  else 
    return shaderIt->second;
}

void DX9ResourceManager::OnLostDevice( void )
{
  for(u32 i = 0; i < mDX9InstanceVBuffers.size(); ++i)
    SafeRelease(mVBuffers[mDX9InstanceVBuffers[i]->mID]);

  for(u32 i = 0; i < mDX9InstanceIBuffers.size(); ++i)
    SafeRelease(mIBuffers[mDX9InstanceIBuffers[i]->mID]);
}

void DX9ResourceManager::OnResetDevice( void )
{
  for(u32 i = 0; i < mDX9InstanceVBuffers.size(); ++i)
  {
    DX9VertexBuffer* vb = mDX9InstanceVBuffers[i];
    HR( mDXDevice->CreateVertexBuffer(vb->mSize, vb->mUsage, 0, vb->mPool, &mVBuffers[vb->mID], 0) );
  }

  for(u32 i = 0; i < mDX9InstanceIBuffers.size(); ++i)
  {
    DX9IndexBuffer* ib = mDX9InstanceIBuffers[i];
    HR( mDXDevice->CreateIndexBuffer(ib->mSize, ib->mUsage, ib->mFormat, ib->mPool, &mIBuffers[ib->mID], 0) );
  }
}

} // namespace Virtus
