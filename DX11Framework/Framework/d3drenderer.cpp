#include "d3drenderer.h"

#define CHECK(x) if(FAILED(x)) return false;
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }
#define SafeDelete(x) { delete x; x = 0; }

D3DRenderer::D3DRenderer() : m_swapChain(0), m_device(0), m_deviceContext(0), m_renderTargetView(0), 
                             m_depthStencilBuffer(0), m_depthStencilState(0), m_depthStencilView(0), m_rasterState(0)
{
}

D3DRenderer::~D3DRenderer()
{
}


bool D3DRenderer::Initialize( int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenFar, float screenNear )
{
  // Store the vsync setting.
  m_vsync_enabled = vsync;

  // Create a DirectX graphics interface factory.
  IDXGIFactory* factory;
  CHECK( CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory) );

  // Use the factory to create an adapter for the primary graphics interface (video card).
  IDXGIAdapter* adapter;
  CHECK( factory->EnumAdapters( 0, &adapter ) );

  // Enumerate the primary adapter output (monitor).
  IDXGIOutput* adapterOutput;
  CHECK( adapter->EnumOutputs( 0, &adapterOutput ) );

  // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
  unsigned numModes;
  CHECK( adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, 0 ) );

  // Create a list to hold all the possible display modes for this monitor/video card combination.
  DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
  if(!displayModeList)
    return false;

  // Now fill the display mode list structures.
  CHECK( adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList ) );

  // Now go through all the display modes and find the one that matches the screen width and height.
  // When a match is found store the numerator and denominator of the refresh rate for that monitor.
  unsigned numerator, denominator;
  for( unsigned i = 0; i < numModes; ++i )
  {
    if(displayModeList[i].Width == (unsigned)screenWidth)
    {
      if(displayModeList[i].Height == (unsigned)screenHeight)
      {
        numerator = displayModeList[i].RefreshRate.Numerator;
        denominator = displayModeList[i].RefreshRate.Denominator;
      }
    }
  }

  // Get the adapter (video card) description.
  DXGI_ADAPTER_DESC adapterDesc;
  CHECK( adapter->GetDesc(&adapterDesc) );

  // Release the display mode list.
  delete [] displayModeList;
  displayModeList = 0;

  // Release the adapter output.
  ReleaseCOM(adapterOutput);
  ReleaseCOM(adapter);
  ReleaseCOM(factory);

  DXGI_SWAP_CHAIN_DESC swapChainDesc;

  // Initialize the swap chain description.
  ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

  // Set to a single back buffer.
  swapChainDesc.BufferCount = 1;

  // Set the width and height of the back buffer.
  swapChainDesc.BufferDesc.Width = screenWidth;
  swapChainDesc.BufferDesc.Height = screenHeight;

  // Set regular 32-bit surface for the back buffer.
  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

  // Set the refresh rate of the back buffer.
  if(m_vsync_enabled)
  {
    swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
  }
  else
  {
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
  }

  // Set the usage of the back buffer.
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

  // Set the handle for the window to render to.
  swapChainDesc.OutputWindow = hwnd;

  // Turn multi-sampling off.
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;

  // Set to full screen or windowed mode.
  if(fullscreen)
    swapChainDesc.Windowed = false;
  else
    swapChainDesc.Windowed = true;

  // Set the scan line ordering and scaling to unspecified.
  swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

  // Discard the back buffer contents after presenting.
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  // Don't set the advanced flags.
  swapChainDesc.Flags = 0;

  // Set the feature level to DirectX 11.
  D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

  // Create the swap chain, Direct3D device, and Direct3D device context.
  CHECK( D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, 
    D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, 0, &m_deviceContext));

  // Get the pointer to the back buffer.
  ID3D11Texture2D* backBufferPtr;
  CHECK( m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr) );

  // Create the render target view with the back buffer pointer.
  CHECK( m_device->CreateRenderTargetView(backBufferPtr, 0, &m_renderTargetView) );

  // Release pointer to the back buffer as we no longer need it.
  ReleaseCOM( backBufferPtr );

  // Initialize the description of the depth buffer.
  D3D11_TEXTURE2D_DESC depthBufferDesc;
  ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

  // Set up the description of the depth buffer.
  depthBufferDesc.Width = screenWidth;
  depthBufferDesc.Height = screenHeight;
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize = 1;
  depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  // Create the depth/stencil buffer
  CHECK( m_device->CreateTexture2D(&depthBufferDesc, 0, &m_depthStencilBuffer) );

  // Initialize the description of the stencil state.
  D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
  ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

  // Set up the description of the stencil state.
  depthStencilDesc.DepthEnable = true;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

  depthStencilDesc.StencilEnable = true;
  depthStencilDesc.StencilReadMask = 0xFF;
  depthStencilDesc.StencilWriteMask = 0xFF;

  // Stencil operations if pixel is front-facing.
  depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Stencil operations if pixel is back-facing.
  depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Create the depth stencil state.
  CHECK( m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState) );

  // Set the depth stencil state.
  m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

  // Initialize the depth stencil view.
  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

  // Set up the depth stencil view description.
  depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;

  // Create the depth stencil view.
  CHECK( m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView) );

  // Bind the render target view and depth stencil buffer to the output render pipeline.
  m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


  // Setup the raster description which will determine how and what polygons will be drawn.
  D3D11_RASTERIZER_DESC rasterDesc;
  ZeroMemory(&rasterDesc, sizeof(rasterDesc));

  rasterDesc.AntialiasedLineEnable = false;
  rasterDesc.CullMode = D3D11_CULL_BACK;
  rasterDesc.DepthBias = 0;
  rasterDesc.DepthBiasClamp = 0.0f;
  rasterDesc.DepthClipEnable = true;
  rasterDesc.FillMode = D3D11_FILL_SOLID;
  rasterDesc.FrontCounterClockwise = false;
  rasterDesc.MultisampleEnable = false;
  rasterDesc.ScissorEnable = false;
  rasterDesc.SlopeScaledDepthBias = 0.0f;

  // Create the rasterizer state from the description we just filled out.
  CHECK( m_device->CreateRasterizerState(&rasterDesc, &m_rasterState) );

  // Now set the rasterizer state.
  m_deviceContext->RSSetState(m_rasterState);

  // Setup the viewport for rendering.
  D3D11_VIEWPORT viewport;
  viewport.Width = (float)screenWidth;
  viewport.Height = (float)screenHeight;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;

  // Create the viewport.
  m_deviceContext->RSSetViewports(1, &viewport);

  // Setup the projection matrix.
  float fieldOfView, screenAspect;
  fieldOfView = (float)D3DX_PI / 4.0f;
  screenAspect = (float)screenWidth / (float)screenHeight;

  // Create the projection matrix for 3D rendering.
  D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenFar);

  // Initialize the world matrix to the identity matrix.
  D3DXMatrixIdentity(&m_worldMatrix);

  // Create an orthographic projection matrix for 2D rendering.
  D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenFar);

  return true;
}


void D3DRenderer::Shutdown()
{
  // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
  if(m_swapChain)
    m_swapChain->SetFullscreenState(false, 0);

  // Release all COM Objects
  ReleaseCOM(m_rasterState);
  ReleaseCOM(m_depthStencilView);
  ReleaseCOM(m_depthStencilState);
  ReleaseCOM(m_depthStencilBuffer);
  ReleaseCOM(m_renderTargetView);
  ReleaseCOM(m_deviceContext);
  ReleaseCOM(m_device);
  ReleaseCOM(m_swapChain);
}

void D3DRenderer::BeginScene( float r, float g, float b, float a )
{
  float color[4] = { r, g, b, a };

  // Clear the back buffer.
  m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

  // Clear the depth buffer.
  m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DRenderer::EndScene()
{
  // Present the back buffer to the screen since rendering is complete.
  if(m_vsync_enabled)
    m_swapChain->Present(1, 0);
  else
    // Present as fast as possible.
    m_swapChain->Present(0, 0);
}

ID3D11Device* D3DRenderer::GetDevice()
{
  return m_device;
}

ID3D11DeviceContext* D3DRenderer::GetDeviceContext()
{
  return m_deviceContext;
}

void D3DRenderer::GetProjectionMatrix( D3DXMATRIX& projOut )
{
  projOut = m_projectionMatrix;
}

void D3DRenderer::GetWorldMatrix( D3DXMATRIX& worldOut )
{
  worldOut = m_worldMatrix;
}

void D3DRenderer::GetOrthoMatrix( D3DXMATRIX& orthoOut )
{
  orthoOut = m_orthoMatrix;
}
