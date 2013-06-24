#ifndef D3D_RENDERER_H
#define D3D_RENDERER_H

#pragma warning( disable : 4005 )
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3dx10math.h>

//TODO:  This will be a renderer ancestor, as will my openGL renderer
class D3DRenderer
{
  public:
    D3DRenderer();
    ~D3DRenderer();

    bool Initialize( int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenFar, float screenNear );
    void Shutdown();

    void BeginScene( float r, float g, float b, float a );
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix( D3DXMATRIX& projOut );
    void GetWorldMatrix( D3DXMATRIX& worldOut );
    void GetOrthoMatrix( D3DXMATRIX& orthoOut );

  private:
    bool m_vsync_enabled;
    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11RasterizerState* m_rasterState;
    D3DXMATRIX m_projectionMatrix;
    D3DXMATRIX m_worldMatrix;
    D3DXMATRIX m_orthoMatrix;
};

#endif  // D3D_RENDERER_H