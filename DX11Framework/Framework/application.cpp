#include "application.h"

Application::Application() : m_input(NULL), m_graphics(NULL)
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
  bool result;

  // Initialize return variables
  screenWidth = 0;
  screenHeight = 0;

  // Initialize the windows api.
  InitializeWindows(screenWidth, screenHeight);

  /// TODO: serialize in systems so that they're created an initialized all at once.

  // Create the input object.  This object will be used to handle reading the keyboard input from the user.
  m_input = new Input();
  if(!m_input)
    return false;

  // Setup Input
  m_input->Initialize();

  // Create and setup Graphics
  m_graphics = new Graphics();
  if(!m_graphics)
    return false;

  result = m_graphics->Initialize(screenWidth, screenHeight, m_hwnd);
  if(!result)
    return false;

  return true;
}

void Application::Shutdown()
{
  // TODO make this a loop through all the systems in the application.

  // Release the graphics object.
  if(m_graphics)
  {
    m_graphics->Shutdown();
    delete m_graphics;
    m_graphics = NULL;
  }

  // Release the input object.
  if(m_input)
  {
    delete m_input;
    m_input = NULL;
  }

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
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
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
      m_input->KeyDown((unsigned)wparam);
      return 0;
    }
    case WM_KEYUP:
    {
      m_input->KeyUp((unsigned)wparam);
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
  // TODO: This should just loop through all systems calling update, and checking for results that may stop the engine.

  bool result;

  // Check if the user pressed escape and wants to exit the application.
  if(m_input->IsKeyDown(VK_ESCAPE))
    return false;

  // Do the frame processing for the graphics object.
  result = m_graphics->Update();
  if(!result)
    return false;

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
  m_hInstance = GetModuleHandle(NULL);

  // Give the application a name.
  m_appName = L"DX11 Framework";

  // Setup the windows class with default settings.
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = m_hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  wc.hIconSm = wc.hIcon;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = NULL;
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
  m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_appName, m_appName, 
    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
    posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

  // Bring the window up on the screen and set it as main focus.
  ShowWindow(m_hwnd, SW_SHOW);
  SetForegroundWindow(m_hwnd);
  SetFocus(m_hwnd);

  // Hide the mouse cursor.
  ShowCursor(false);

  return;
}

void Application::ShutdownWindows()
{
  // Show the mouse cursor.
  ShowCursor(true);

  // Fix the display settings if leaving full screen mode.
  if(FULL_SCREEN)
  {
    ChangeDisplaySettings(NULL, 0);
  }

  // Remove the window.
  DestroyWindow(m_hwnd);
  m_hwnd = NULL;

  // Remove the application instance.
  UnregisterClass(m_appName, m_hInstance);
  m_hInstance = NULL;

  // Release the pointer to this class.
  AppInstance = NULL;

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
