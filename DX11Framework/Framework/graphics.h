#ifndef GRAPHICS_H
#define GRAPHICS_H

// Once I generalize the graphics system, this goes away
#include <windows.h>

// Globals will be dying soon.
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
    Graphics(const Graphics& rhs);
    ~Graphics();

    bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
    void Shutdown();
    bool Update();

  private:
    bool Render();
};

#endif  // GRAPHICS_H