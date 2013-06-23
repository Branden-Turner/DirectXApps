/*! ====================================================================================================================
  @file dx9.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.17.2011

  @brief
    DirectX9 Related stuff

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DX9_H
#define VIRTUS_DX9_H

// DirectX
#if (VIRTUS_CONFIG_DEBUG_MODE)
  #ifndef D3D_DEBUG_INFO
    #define D3D_DEBUG_INFO
  #endif
#endif

#if (VIRTUS_CONFIG_DEBUG_MODE)
  #include <DxErr.h>
#endif

#undef new
  #include <d3dx9.h>
  #include <d3d9.h>
#define new VIRTUS_NEW

#if (VIRTUS_CONFIG_DEBUG_MODE)
  #pragma comment(lib, "d3dx9d.lib")
  #pragma comment(lib, "dxerr.lib")
#else
  #pragma comment(lib, "d3dx9.lib")
#endif

#pragma comment(lib, "d3d9.lib")

typedef IDirect3DVertexDeclaration9 DXVertexDecl;
typedef IDirect3DIndexBuffer9       DXIndexBuffer;
typedef IDirect3DVertexBuffer9      DXVertexBuffer;
typedef IDirect3DPixelShader9       DXPixelShader;
typedef IDirect3DVertexShader9      DXVertexShader;
typedef IDirect3DSwapChain9         DXSwapChain;
typedef IDirect3D9                  DXObject;
typedef IDirect3DDevice9            DXDevice;
typedef IDirect3DTexture9           DXTexture;
typedef IDirect3DCubeTexture9       DXCubeTexture;
typedef IDirect3DSurface9           DXSurface;

typedef ID3DXEffect        DXEffect;
typedef ID3DXConstantTable DXTable;
typedef ID3DXBuffer        DXBuffer;

typedef D3DPRESENT_PARAMETERS DXPP;
typedef D3DDISPLAYMODE        DXDisplayMode;
typedef D3DCAPS9              DXCaps;
typedef D3DPOOL               DXPool;
typedef D3DFORMAT             DXFormat;
typedef D3DDEVTYPE            DXDevType;
typedef D3DSURFACE_DESC       DXSurfaceDesc;
typedef D3DVERTEXELEMENT9     DXVertexElem;
typedef D3DPRIMITIVETYPE      DXPrimitiveType;

typedef D3DXCONSTANTTABLE_DESC DXTableDesc;
typedef D3DXCONSTANT_DESC      DXConstDesc;
typedef D3DXHANDLE             DXHandle;
typedef D3DXIMAGE_INFO         DXImageInfo;

typedef DXVertexDecl*   DXVertexDeclPtr;
typedef DXTexture*      DXTexturePtr;
typedef DXCubeTexture*  DXCubeTexturePtr;
typedef DXIndexBuffer*  DXIndexBufferPtr;
typedef DXVertexBuffer* DXVertexBufferPtr;
typedef DXVertexShader* DXVertexShaderPtr;
typedef DXPixelShader*  DXPixelShaderPtr;

typedef DXTexture**      DXTextureHndl;
typedef DXCubeTexture**  DXCubeTextureHndl;
typedef DXIndexBuffer**  DXIndexBufferHndl;
typedef DXVertexBuffer** DXVertexBufferHndl;

// Oh fuck these are ugly.     

#if (VIRTUS_CONFIG_DEBUG_MODE)
  #define HR(x) do {                         \
    HRESULT hr = x;                          \
    if( FAILED(hr) ) do {                    \
    std::string str( "\n" );                 \
    str.append( #x );                        \
    str.append( "\n\nDXError: " );           \
    str.append( DXGetErrorString(hr) );      \
    str.append( "\nDXDescription: " );       \
    str.append( DXGetErrorDescription(hr) ); \
    Error( str.c_str() );                    \
    } while(0);                              \
  } while(0);
#else
  #define HR(x) (x)
#endif

#if (VIRTUS_CONFIG_DEBUG_MODE)
  #define HRX(x,y) do {                                    \
    HRESULT hr = x;                                        \
    if( FAILED(hr) ) do {                                  \
    std::string str( "\n" );                               \
    str.append( #x );                                      \
    str.append( "\n\nDXError: " );                         \
    str.append( DXGetErrorString(hr) );                    \
    str.append( "\nDXDescription: " );                     \
    str.append( DXGetErrorDescription(hr) );               \
    if(y) {                                                \
      str.append ("\n");                                   \
      str.append( RCAST(char*, y->GetBufferPointer() ) );  \
    }                                                      \
    Error( str.c_str() );                                  \
      } while(0);                                          \
  } while(0)
#else
  #define HRX(x,y) (x)
#endif

#endif // Include Guard
