#include "application.h"

Application::Application()
{
}

Application::Application(const Application& rhs)
{
}

Application::~Application()
{
}

bool Application::Initialize()
{
  int screenWidth, screenHeight;

  // Initialize return variables
  screenWidth = 0;
  screenHeight = 0;

  // Initialize the windows api.
  InitializeWindows(screenWidth, screenHeight);

  /// TODO: serialize in systems so that they're created an initialized all at once.

  Input* input = new Input();
  Graphics* gfx = new Graphics(screenWidth, screenHeight, m_hwnd);

  m_systems.push_back( input );
  m_systems.push_back( gfx );

  // Call the main init function of all systems.
  for( System* sys : m_systems ) sys->Initialize();

  return true;
}

void Application::Shutdown()
{
  // Shutdown all systems
  for( System* sys : m_systems ) 
  {
    sys->Shutdown();
    delete sys;
    sys = 0;
  }
  m_systems.clear();

  // Shutdown windows API stuff.
  ShutdownWindows();

  return;
}

void Application::Run()
{
  MSG msg;
  bool done, result;

  // Initialize the message structure.
  ZeroMemory(&msg, sizeof(MSG));

  // Loop until there is a quit message from the window or the user.
  done = false;
  while(!done)
  {
    // Handle the windows messages.
    if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // If windows signals to end the application then exit out.
    if(msg.message == WM_QUIT)
      done = true;
    else
    {
      // Otherwise do the frame processing.
      result = Update();
      if(!result)
        done = true;
    }
  }

  return;
}

LRESULT CALLBACK Application::MessageHandler( HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam )
{
  // Route the windows messages to the input handler
  switch(umsg)
  {
    case WM_KEYDOWN:
    {
      Input::Instance()->KeyDown((unsigned)wparam);
      return 0;
    }
    case WM_KEYUP:
    {
      Input::Instance()->KeyUp((unsigned)wparam);
      return 0;
    }
    // Any other messages send to the default message handler as our application won't make use of them.
    default:
    {
      return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
  }
}

bool Application::Update()
{
  // Check if the user pressed escape and wants to exit the application.
  if(Input::Instance()->IsKeyDown(VK_ESCAPE))
    return false;

  // Call the main init function of all systems.
  for( System* sys : m_systems )
  {
    // return false if an update fails.
    if( !sys->Update() )
      return false;
  }

  return true;
}

void Application::InitializeWindows( int& screenWidth, int& screenHeight )
{
  WNDCLASSEX wc;
  DEVMODE dmScreenSettings;
  int posX, posY;

  // Get an external pointer to this object.
  AppInstance = this;

  // Get the instance of this application.
  m_hInstance = GetModuleHandle(0);

  // Give the application a name.
  m_appName = L"DX11 Framework";

  // Setup the windows class with default settings.
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = m_hInstance;
  wc.hIcon = LoadIcon(0, IDI_WINLOGO);
  wc.hIconSm = wc.hIcon;
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = 0;
  wc.lpszClassName = m_appName;
  wc.cbSize = sizeof(WNDCLASSEX);

  // Register the window class.
  RegisterClassEx(&wc);

  // Determine the resolution of the clients desktop screen.
  screenWidth  = GetSystemMetrics(SM_CXSCREEN);
  screenHeight = GetSystemMetrics(SM_CYSCREEN);

  // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
  if(FULL_SCREEN)
  {
    // If full screen set the screen to maximum size of the users desktop and 32bit.
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
    dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
    dmScreenSettings.dmBitsPerPel = 32;			
    dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Change the display settings to full screen.
    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    // Set the position of the window to the top left corner.
    posX = posY = 0;
  }
  else
  {
    // If windowed then set it to WidthxHeight resolution.
    screenWidth  = SCREEN_WIDTH;
    screenHeight = SCREEN_HEIGHT;

    // Place the window in the middle of the screen.
    posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
    posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
  }

  // Create the window with the screen settings and get the handle to it.
  m_hwnd = CreateWindow(m_appName, m_appName, WS_OVERLAPPEDWINDOW,
    posX, posY, screenWidth, screenHeight, 0, 0, m_hInstance, 0);

  // Bring the window up on the screen and set it as main focus.
  ShowWindow(m_hwnd, SW_SHOW);
  SetForegroundWindow(m_hwnd);
  SetFocus(m_hwnd);

  return;
}

void Application::ShutdownWindows()
{
  // Show the mouse cursor.
  ShowCursor(true);

  // Fix the display settings if leaving full screen mode.
  if(FULL_SCREEN)
    ChangeDisplaySettings(0, 0);

  // Remove the window.
  DestroyWindow(m_hwnd);
  m_hwnd = 0;

  // Remove the application instance.
  UnregisterClass(m_appName, m_hInstance);
  m_hInstance = 0;

  // Release the pointer to this class.
  AppInstance = 0;

  return;
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
  switch(umsg)
  {
    // Check if the window is being destroyed.
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      return 0;
    }

    // Check if the window is being closed.
    case WM_CLOSE:
    {
      PostQuitMessage(0);		
      return 0;
    }

    // All other messages pass to the message handler in the system class.
    default:
    {
      return AppInstance->MessageHandler(hwnd, umsg, wparam, lparam);
    }
  }
}
