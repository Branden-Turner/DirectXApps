#ifndef APPLICATION_H
#define APPLICATION_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>

#include "input.h"
#include "graphics.h"

class Application
{
  public:
    Application();
    Application(const Application& rhs);
    ~Application();

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

  private:
    bool Update();
    void InitializeWindows(int& screenWidth, int& screenHeight);
    void ShutdownWindows();

  private:
    LPCWSTR m_appName;
    HINSTANCE m_hInstance;
    HWND m_hwnd;

    std::vector<System*> m_systems;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

static Application* AppInstance = 0;

#endif  // APPLICATION_H