#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "d3drenderer.h"

// These should be serialized in.  Serialization should be one of the next things I do...
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_FAR = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class Graphics
{
  public:
    Graphics();
    ~Graphics();

    bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
    void Shutdown();
    bool Update();

  private:
    bool Render();

  private:
    D3DRenderer* m_d3dRenderer;
};

#endif  // GRAPHICS_H