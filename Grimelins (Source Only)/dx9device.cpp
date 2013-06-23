: /*! ====================================================================================================================
  @file dx9device.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.03.2011

  @brief 
    Implementing a DirectX 9 graphics renderer

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

#include "precompiled.h"
#include "dx9device.h"
#include "dx9rendertargets.h"
#include "dx9resourcemanager.h"
#include "dx9shaders.h"
#include "dx9buffers.h"
#include "dx9texturetype.h"
#include "dx9vertextype.h"
#include "dx9shapes.h"
#include "color.h"

namespace Grimelins {

DX9Device::DX9Device(void)
  : mDeviceName("DirectX9"),
    mWinHandle(0),
    mDX9RM(NULL),
    mDXDevice(NULL),
    mDXObject(NULL),
    mBackBuffer(NULL),
    mFullScreen(false),
    mBBFlags(D3DCLEAR_ZBUFFER),
    mBBColor( GetColor(Black) ),
    mBBStencil(0),
    mBBDepth(1.0f),
    mRenderTarget(0)
{
  mDXDevType = D3DDEVTYPE_HAL;
  ZeroMemory(&mDXPP, sizeof(mDXPP));
}

DX9Device::~DX9Device(void)
{

}

void DX9Device::CreateDevice( Application::OSWINDOW winHandle, s32 winWidth, s32 winHeight )
{
  mWinHandle = winHandle;

  mDXObject = Direct3DCreate9( D3D_SDK_VERSION );

  if ( !mDXObject )
    throw Exception( E_GFXDEVICE_NOT_AVAILABLE, "DX9Device could not be created. Do you have DirectX 9 installed and compatible hardware?" );

  DXDisplayMode mode;
  mDXObject->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &mode );

  HR( mDXObject->CheckDeviceType( D3DADAPTER_DEFAULT, mDXDevType, mode.Format, mode.Format, true ) );
  HR( mDXObject->CheckDeviceType( D3DADAPTER_DEFAULT, mDXDevType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false ) );

  DXCaps caps;
  HR( mDXObject->GetDeviceCaps( D3DADAPTER_DEFAULT, mDXDevType, &caps ) );

  mDXPP.AutoDepthStencilFormat     = D3DFMT_D24S8;
  mDXPP.BackBufferCount            = 1;
  mDXPP.BackBufferFormat           = D3DFMT_UNKNOWN;
  mDXPP.BackBufferHeight           = 0;
  mDXPP.BackBufferWidth            = 0;
  mDXPP.EnableAutoDepthStencil     = true; 
  mDXPP.Flags                      = 0;
  mDXPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
  mDXPP.hDeviceWindow              = mWinHandle;
  mDXPP.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;
  mDXPP.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
  mDXPP.Windowed                   = true;
  mDXPP.MultiSampleQuality         = 0;
  mDXPP.MultiSampleType            = D3DMULTISAMPLE_NONE;

  // Set default settings
  UINT AdapterToUse = D3DADAPTER_DEFAULT;

  HR(mDXObject->CreateDevice(
    AdapterToUse,
    mDXDevType,
    mWinHandle,
    D3DCREATE_HARDWARE_VERTEXPROCESSING, 
    &mDXPP,
    &mDXDevice)
  );

  InitializeVertexDeclarations(mDXDevice);

  mDX9RM     = new DX9ResourceManager(mDXDevice);
  mDX9Shapes = new DX9Shapes(mDX9RM);
}

void DX9Device::DestroyDevice(void)
{
  SafeDelete(mBackBuffer);

  SafeDelete(mDX9RM);
  SafeDelete(mDX9Shapes);

  ReleaseVertexDeclarations();

  SafeRelease(mDXDevice);
  SafeRelease(mDXObject);
}

DX9RenderTarget* DX9Device::GetBackBuffer(void)
{
  if( !mBackBuffer )
  {
    DXSurface* bBuffer;
    mDXDevice->GetRenderTarget(0, &bBuffer);
    mBackBuffer = new DX9RenderTarget(bBuffer, mBBFlags, mBBColor, mBBStencil, mBBDepth);
  }

  return mBackBuffer;
}

void DX9Device::DrawIndexedTris( u32 count, u32 numVertexes, u32 startindex )
{
  HR( mDXDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, startindex, 0, numVertexes, 0, count ) );
}

void DX9Device::DrawTris( u32 count, u32 startindex )
{
  HR( mDXDevice->DrawPrimitive( D3DPT_TRIANGLELIST, startindex, count ) );
}

void DX9Device::DrawLines( u32 count, u32 startindex )
{
  HR( mDXDevice->DrawPrimitive( D3DPT_LINELIST, startindex, count ) );
}

void DX9Device::SetBuffer( DX9IndexBuffer* ibuffer )
{
  HR( mDXDevice->SetIndices( *(ibuffer->mBuffer) ) );
}

void DX9Device::SetDecl( DXVertexDecl* decl )
{
  HR( mDXDevice->SetVertexDeclaration(decl) );
}

void DX9Device::SetBuffer( DX9VertexBuffer* vbuffer, u32 stride, u32 stream, u32 offset )
{
  HR( mDXDevice->SetStreamSource( stream, *(vbuffer->mBuffer), offset, stride ) );
}

void DX9Device::SetShader( DX9PixelShader* pshader )
{
  HR( mDXDevice->SetPixelShader( pshader->mShader ) );
}

void DX9Device::SetShader( DX9VertexShader* vshader )
{
  HR( mDXDevice->SetVertexShader( vshader->mShader ) );
}

void DX9Device::SetStreamFreq( u32 stream, u32 flag )
{
  HR( mDXDevice->SetStreamSourceFreq(stream, flag) );
}

void DX9Device::SetVSData( void const* data, u32 numFloat4, u32 registerIndex )
{
  HR( mDXDevice->SetVertexShaderConstantF( registerIndex, RCAST(f32 const*, data), numFloat4 ) );
}

void DX9Device::SetPSData( void const* data, u32 numFloat4, u32 registerIndex, u1 ints )
{
  if(ints)
  {
    HR( mDXDevice->SetPixelShaderConstantI( registerIndex, RCAST(s32 const*, data), numFloat4 ) );
  }
  else
  {
    HR( mDXDevice->SetPixelShaderConstantF( registerIndex, RCAST(f32 const*, data), numFloat4 ) );
  }
}

void DX9Device::SetTexture( DX9Texture* tex, u32 index /*= 0 */ )
{
  HR( mDXDevice->SetTexture( index, *(tex->mTexture) ) );
}

void DX9Device::SetTexture( DX9CubeTexture* tex, u32 index /*= 0 */ )
{
  HR( mDXDevice->SetTexture( index, *(tex->mTexture) ) );
}


void DX9Device::ClearRenderTarget(void)
{
  HR( mDXDevice->Clear(
    0, 0, 
    mRenderTarget->mBufferFlags, 
    mRenderTarget->mColorValue,
    mRenderTarget->mDepthValue, 
    mRenderTarget->mStencilValue ) 
  );
}

void DX9Device::SetRenderTarget(DX9RenderTarget* renderTarget, u32 index)
{
  mRenderTarget = renderTarget;
  HR( mDXDevice->SetRenderTarget( index, mRenderTarget->mSurface ) );
}

void DX9Device::BeginScene(void)
{
  HR( mDXDevice->BeginScene() );
}

void DX9Device::EndScene(void)
{
  HR( mDXDevice->EndScene() );
}

void DX9Device::Present(void)
{
  switch( HRESULT ret = mDXDevice->TestCooperativeLevel() )
  {
  case D3DERR_DEVICELOST:
    mDeviceLost = true;
    return;

  case D3DERR_DEVICENOTRESET:
    Sleep(20);
    mDeviceLost = true;
    return;
  }

  HRESULT hr = mDXDevice->Present(0,0,0,0);

  if( FAILED(hr) )
    mDeviceLost = true;
}

std::string DX9Device::GetName(void)
{
  return mDeviceName;
}

bool DX9Device::IsDeviceLost(void)
{
  if(!mDeviceLost)
    return false;

  switch( HRESULT ret = mDXDevice->TestCooperativeLevel() )
  {
  case D3DERR_DEVICELOST:
    return true;

  case D3DERR_DEVICENOTRESET:
    ResetDevice();
    return true;

  case D3DERR_DRIVERINTERNALERROR:
    throw Exception(E_GFX_DRIVER_ERROR, "Internal Driver Error");
    return true;

  default:
    if( FAILED(ret) )
      throw Exception(E_GFX_DRIVER_ERROR, "Why you break grapics? ");
    return false;
  }
}

void DX9Device::ToggleFullScreen( void )
{
  mFullScreen = !mFullScreen;
  ChangeResolution(0,0);
}

void DX9Device::ChangeResolution( u32 width, u32 height )
{
  static RECT mClientRect;
  static RECT mWindowRect;
  static u32 mWindowHeight;
  static u32 mWindowWidth;
  static u32 mBBWidth;
  static u32 mBBHeight;
  static u32 mFullScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
  static u32 mFullScreenHeight = GetSystemMetrics(SM_CYSCREEN);
  static long mWinStyleHndl = NULL;

  if(width != 0)
    if(mFullScreen)
      mFullScreenWidth = width;
    else
      mWindowWidth = width;

  if(height != 0)
    if(mFullScreen)
        mFullScreenHeight = height;
    else
      mWindowHeight = height;

  if( mFullScreen && mDXPP.Windowed )
  {

    GetClientRect( mWinHandle, &mClientRect );
    GetWindowRect( mWinHandle, &mWindowRect );

    mBBWidth = mClientRect.right - mClientRect.left;
    mBBHeight = mClientRect.bottom - mClientRect.top;

    mWindowWidth = mWindowRect.right - mWindowRect.left;
    mWindowHeight = mWindowRect.bottom - mWindowRect.top;

    mDXPP.BackBufferFormat = D3DFMT_X8R8G8B8;
    mDXPP.BackBufferWidth  = mFullScreenWidth;
    mDXPP.BackBufferHeight = mFullScreenHeight;
    mDXPP.Windowed         = false;

    mWinStyleHndl = SetWindowLong(mWinHandle, GWL_STYLE, WS_POPUP);
    SetWindowPos( mWinHandle, HWND_TOP, 0, 0, mFullScreenWidth, mFullScreenHeight, SWP_NOZORDER | SWP_SHOWWINDOW );	
  }
  else if(!mFullScreen) 
  {
//     if(mDXPP.Windowed )
//       return;
    mDXPP.BackBufferFormat = D3DFMT_X8R8G8B8;
    mDXPP.BackBufferWidth  = mBBWidth;
    mDXPP.BackBufferHeight = mBBHeight;
    mDXPP.Windowed         = true;

    //Uses the handle to our previous style to reset it back the way it was
    SetWindowLong( mWinHandle, GWL_STYLE, mWinStyleHndl );
    SetWindowPos( mWinHandle, HWND_NOTOPMOST, mWindowRect.left, mWindowRect.top, mWindowWidth, mWindowHeight, SWP_SHOWWINDOW );
  }
#ifdef _DEBUG
  ShowCursor(true);
#endif

  //refresh the device when in fullscreen, to do this, toggle to windowed mode, then back
if(height!=0 && width!=0 && mFullScreen)
  {

    ToggleFullScreen();
    ToggleFullScreen();
#ifndef _DEBUG
    ShowCursor(false);
#endif
  }
  ResetDevice();
}

void DX9Device::OnLostDevice( void )
{
  mDX9RM->OnLostDevice();

  ANT__WindowSize(mDXPP.BackBufferWidth, mDXPP.BackBufferHeight);

  SafeDelete(mBackBuffer);
}

void DX9Device::OnResetDevice( void )
{
  GetDeviceResolutions();
  mDX9RM->OnResetDevice();
}

void DX9Device::ResetDevice( void )
{
  OnLostDevice();

  HRESULT hr = mDXDevice->Reset(&mDXPP);

  if (hr == D3DERR_DEVICELOST)
  {
    mDeviceLost = true;
    return;
  }
  else if ( FAILED(hr) )
    throw Exception(E_GFX_DRIVER_ERROR, "Why would you break Graphics like that?");

  mDeviceLost = false;

  OnResetDevice();
}

void DX9Device::GetDeviceResolutions( void )
{
  support1024 = support1280 = support1600 = support1920 = false;

  unsigned modeCount = mDXObject->GetAdapterModeCount( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8 );
  for ( unsigned i = 0; i < modeCount; ++i )
  {
    D3DDISPLAYMODE dMode;
    SecureZeroMemory( &dMode, sizeof( D3DDISPLAYMODE ) );
    HR( mDXObject->EnumAdapterModes( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &dMode ) );
    if(dMode.Width == 1024 && dMode.Height == 768)
      support1024 = true;
    else if(dMode.Width == 1280 && dMode.Height == 720)
      support1280 = true;
    else if(dMode.Width == 1600 && dMode.Height == 900)
      support1600 = true;
    else if(dMode.Width == 1920 && dMode.Height == 1080)
      support1920 = true;
  }
  
}

} // namespace Virtus