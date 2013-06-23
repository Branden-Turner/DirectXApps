#include "precompiled.h"



HINSTANCE   g_hInst = NULL;
HWND        g_hWnd = NULL;
D3D10_DRIVER_TYPE       g_driverType = D3D10_DRIVER_TYPE_NULL;
ID3D10Device*           g_pd3dDevice = NULL;
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
  //I'm not going to use these.
  UNREFERENCED_PARAMETER( hPrevInstance );
  UNREFERENCED_PARAMETER( lpCmdLine );

  cs350::DXAPP app(hInstance);
  app.initApp();

  return app.run();
}
