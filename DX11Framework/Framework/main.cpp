#include "application.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
  Application* application;
  bool result;

  // Create application
  application = new Application();
  if(!application)
    return 0;

  // Run application
  result = application->Initialize();
  if(result)
    application->Run();

  // Cleanup after application runs
  application->Shutdown();
  delete application;
  application = NULL;

  return 0;
}