#include "graphics.h"

Graphics* Graphics::m_instance = 0;

Graphics::Graphics(int screenWidth, int screenHeight, HWND hwnd) : m_d3dRenderer(0)
{
  Initialize(screenWidth, screenHeight, hwnd);
  m_instance = this;
}

Graphics::~Graphics()
{
  m_instance = 0;
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
  bool result;

  // Create DX renderer
  m_d3dRenderer = new D3DRenderer();
  if(!m_d3dRenderer)
    return false;

  // Init renderer
  result = m_d3dRenderer->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
  if(!result)
  {
    MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
    return false;
  }

  return true;
}


void Graphics::Shutdown()
{
  if(m_d3dRenderer)
  {
    m_d3dRenderer->Shutdown();
    delete m_d3dRenderer;
    m_d3dRenderer = 0;
  }

  return;
}


bool Graphics::Update()
{
  // Updates to the scene and shader manager will go here.
  return Render();
}


bool Graphics::Render()
{
  // Clear the buffers to begin the scene.
  m_d3dRenderer->BeginScene(0.7f, 0.5f, 0.5f, 1.0f);

  // Do all of the model processing here, as well as shading logic/passes/etc

  // Present the rendered scene to the screen.
  m_d3dRenderer->EndScene();

  return true;
}
