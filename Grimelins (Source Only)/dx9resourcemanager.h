/*! ====================================================================================================================
  @file dx9resourcemanager.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.24.2011

  @brief
    DirectX9 Resource Manager

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9RESOURCEMANAGER_H
#define VIRTUS_DX9RESOURCEMANAGER_H

#include "dx9.h"

namespace Grimelins
{

    class DX9Shader;
    class DX9Texture;
    class DX9CubeTexture;
    class DX9PixelShader;
    class DX9VertexShader;
    class DX9VertexBuffer;
    class DX9IndexBuffer;
    class DX9ResourceManager;

    class DX9ResourceManager
    {
    public:
      DX9ResourceManager( DXDevice* dxdevice );
      ~DX9ResourceManager( void );

    public:

      void OnLostDevice( void );
      void OnResetDevice( void );

      DX9Texture* GetTextureFromFile( std::string const& file );
      DX9CubeTexture* GetCubeTextureFromFile( std::string const& file );

      DX9Texture* GetSpriteSheetAnimFromFile( std::string const& file );

      DX9VertexBuffer* GetVertexBuffer( u32 size, u32 usageFlags, DXPool poolFlags = D3DPOOL_MANAGED );
      DX9IndexBuffer*  GetIndexBuffer( u32 size, u32 usageFlags, DXFormat format = D3DFMT_INDEX16, DXPool poolFlags = D3DPOOL_MANAGED);

      DX9VertexShader* GetVertexShader( const std::string& filename );
      DX9PixelShader*  GetPixelShader( const std::string& filename );
      DX9Shader*       GetShader( const std::string& filename );

      typedef google::dense_hash_map<std::string, DX9Shader*>      ShaderStorage;
      typedef ShaderStorage::const_iterator ShaderConstIter;
      typedef ShaderStorage::iterator       ShaderIter;

      typedef std::vector<DX9VertexBuffer*>     DX9VBufferStorage; 
      typedef DX9VBufferStorage::const_iterator DX9VBuffConstIter;
      typedef DX9VBufferStorage::iterator       DX9VBuffIter;

      typedef std::vector<DX9IndexBuffer*>      DX9IBufferStorage; 
      typedef DX9IBufferStorage::const_iterator DX9IBuffConstIter;
      typedef DX9IBufferStorage::iterator       DX9IBuffIter;

      typedef std::vector<DX9Texture*>          DX9TextureStorage; 
      typedef DX9TextureStorage::const_iterator DX9TextureConstIter;
      typedef DX9TextureStorage::iterator       DX9TextureIter;

      typedef std::vector<DX9CubeTexture*>          DX9CubeTextureStorage; 
      typedef DX9CubeTextureStorage::const_iterator DX9CubeTextureConstIter;
      typedef DX9CubeTextureStorage::iterator       DX9CubeTextureIter;

      typedef std::vector<DXVertexBuffer*>   VBufferStorage; 
      typedef VBufferStorage::const_iterator VBuffConstIter;
      typedef VBufferStorage::iterator       VBuffIter;

      typedef std::vector<DXIndexBuffer*>    IBufferStorage; 
      typedef IBufferStorage::const_iterator IBuffConstIter;
      typedef IBufferStorage::iterator       IBuffIter;

      typedef std::vector<DXTexture*>        TextureStorage;
      typedef TextureStorage::const_iterator TextureConstIter;
      typedef TextureStorage::iterator       TextureIter;

      typedef std::vector<DXCubeTexture*>        CubeTextureStorage;
      typedef CubeTextureStorage::const_iterator CubeTextureConstIter;
      typedef CubeTextureStorage::iterator       CubeTextureIter;

    private: 
      void LoadShaderFile( std::string const& file );
     
    private: 
      DX9VBufferStorage mDX9VBuffers;
      DX9VBufferStorage mDX9InstanceVBuffers;

      DX9IBufferStorage mDX9IBuffers;
      DX9IBufferStorage mDX9InstanceIBuffers;

      DX9TextureStorage     mDX9Textures;
      DX9CubeTextureStorage mDX9CubeTextures; 

        // These contain the actual pointers to DirectX Resources
      VBufferStorage mVBuffers;
      IBufferStorage mIBuffers;
      TextureStorage mTextures;
      TextureStorage m2DAnims;
      CubeTextureStorage mCubeTextures;

      ShaderStorage   mShaderData;
      DXDevice*       mDXDevice;

      DX9TextureStorage mDX92DAnims;

      static u32 mShaderID;
      static u32 mTextureID;
      static u32 mCubeTextureID;
      static u32 mVertexBufferID;
      static u32 mIndexBufferID;
      static u32 m2DAnimID;
    };
} // namespace Virtus

#endif
