/*! ====================================================================================================================
  @file dx9device.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.03.2011

  @brief 
    DirectX9 Render Device

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once

#include "dx9.h"
#include "shapes.h"

namespace Grimelins {

    class DX9RenderTarget;
    class DX9IndexBuffer;
    class DX9VertexBuffer;
    class DX9PixelShader;
    class DX9VertexShader;
    class DX9Texture;
    class DX9CubeTexture;
    class DX9ResourceManager;
    class DX9Shapes;

    class DX9Device
    {
    public:
      DX9Device(void);
      ~DX9Device(void);

      // IGraphicsDevice interface
    public:
      void CreateDevice(Application::OSWINDOW winHandle, s32 winWidth, s32 winHeight);
      void DestroyDevice(void);

      void Present(void);
      std::string GetName(void);

      void BeginScene(void);
      void EndScene(void);

      DX9Shapes*          GetShapes( void ) { return mDX9Shapes; }
      DXDevice*           GetDevice( void ) { return mDXDevice;  } 
      DX9ResourceManager* GetRM    ( void ) { return mDX9RM;     }

      void SetDecl   ( DXVertexDeclPtr decl );
      void SetTexture( DX9Texture* tex, u32 index = 0 );
      void SetTexture( DX9CubeTexture* tex, u32 index = 0 );
      void SetShader ( DX9PixelShader*  pshader );
      void SetShader ( DX9VertexShader* vshader );
      void SetBuffer ( DX9IndexBuffer*  ibuffer );
      void SetBuffer ( DX9VertexBuffer* vbuffer, u32 stride, u32 stream = 0, u32 offset = 0 );
      void SetStreamFreq( u32 stream, u32 flag );

      vec2 GetScreenDim( void ) { return vec2( SCAST(f32, mDXPP.BackBufferWidth), SCAST(f32, mDXPP.BackBufferHeight) ); }

      void DrawIndexedTris( u32 count, u32 numVertexes, u32 startindex = 0 );
      void DrawTris( u32 count, u32 startindex = 0 );
      void DrawLines( u32 count, u32 startindex = 0 );

      void SetVSData( void const* data, u32 numFloat4, u32 registerIndex = 0 );
      void SetPSData( void const* data, u32 numFloat4, u32 registerIndex = 0, u1 ints = false );

      u1 IsDeviceLost( void );
      void ToggleFullScreen( void );
      void ChangeResolution( u32 width, u32 height );

      DX9RenderTarget* GetBackBuffer( void );
      void ClearRenderTarget( void );
      void SetRenderTarget( DX9RenderTarget* renderTarget, u32 index = 0 );
      
      void GetDeviceResolutions(void);

    private:
      void ResetDevice( void );
      void OnLostDevice( void );
      void OnResetDevice( void );

    public:
      u1 support1024;
      u1 support1280;
      u1 support1600;
      u1 support1920;
      // Backbuffer related variables
      u32 mBBFlags;
      u32 mBBColor;
      u32 mBBStencil;
      f32 mBBDepth;
    private:

        // Application related variables
      std::string           mDeviceName;
      Application::OSWINDOW mWinHandle;

        // DirectX related Variables

      DXObject* mDXObject;
      DXDevice* mDXDevice;
      DXDevType mDXDevType;
      DXPP      mDXPP;

      DX9ResourceManager* mDX9RM;
      DX9RenderTarget*    mBackBuffer;
      DX9RenderTarget*    mRenderTarget;
      DX9Shapes*          mDX9Shapes;

      u1 mFullScreen;
      u1 mDeviceLost;


    };
} // namespace Virtus

