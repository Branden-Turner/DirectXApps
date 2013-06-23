/*! ====================================================================================================================
  @file main.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.04.2011

  @brief 
    Program entry point.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "game.h"
#include "view.h"
#include "graphics.h"

namespace Grimelins
{

  Core* CoreInstance(void)
  {
    static Core Core;
    return &Core;
  }

  Core* gCore = CoreInstance();

  WatchDog CrashHandler;
}


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  try
  {
    using namespace Grimelins;


#pragma message(Reminder "Reintroduce hardware checks")
    // Run System Diagnostic for requirement checks
    //Application::RunSystemDiagnostic();
    Application::StartDebugDiag();

    // Add engine layers

    RawStore startupinf;
    startupinf.AddS32("instance", (int)hInstance);
    startupinf.AddS32("prev_instance", (int)hPrevInstance);
    startupinf.AddString("cmd_line", lpCmdLine);
    startupinf.AddS32("cmd_show", nCmdShow);

    
    gCore->Layers.AddLayer(ViewLayer(startupinf));
    gCore->Layers.AddLayer(GameLayer());

    gCore->Start(startupinf);

#pragma message(Reminder "Import this loop into core itself")
    while ( gCore->IsRunning() )
      gCore->NextFrame();

    return 0;
  }
  catch (Grimelins::Exception& e)
  {
    using namespace Grimelins;

    // Catch Fatal Engine Errors (last stand)

    std::stringstream error;
    error << "Error " << e.code() << ": ";

    switch( e.code() )
    {
    case E_HD_NOT_ENOUGH_SPACE:
      error << "There is not enough free hard drive space available to run the application. "
        << "Please clear up some space and try again.";
      break;

    case E_MEM_PHYS_NOT_ENOUGH:
      error << "Your machine does not meet the minimum RAM requirements. ";
      break;

    case E_MEM_VIRT_NOT_ENOUGH:
    case E_MEM_CONT_NOT_NEOUGH:
      error << "There isn't enough RAM available for the application to run. "
        << "Close all background applications and try to start the application again. ";
      break;

    case E_CPU_TOO_SLOW:
      error << "Your processor does not seem to meet the minimum requirements. "
        << "Close all background applications and try to start the application again. ";
      break;

    case E_CONFIG_MISSING:
      error << "The configuration file could not be found or read.";
      break;

    case E_MEM_NOT_INITIALZED:
    case E_MEM_ERROR:
      error << "An internal memory error occurred.";
      break;

    case E_GFXDEVICE_NOT_AVAILABLE:
      error << "Graphics device could not be created.";
      break;

    case E_SOUND_NOT_INITIALIZED:
      error << "Sound couldn't be initialized and the user chose to abort.";
      break;

    case E_MISSING_DLL:
      error << "Couldn't load a runtime module. Critical DLLs might be missing.";
      break;

    case E_UNKNOWN:
    default:
      error << "An unknown error occurred.";
      break;
    }

    error << std::endl << std::endl << "Additional Information:" << std::endl << e.what();

    Application::FullStopError(error.str().c_str());
  }
  catch (Grimelins::ch const* ex)
  {
    Grimelins::Application::FullStopError(ex);
  }
  catch (...)
  {
    Grimelins::Application::FullStopError("Unknown Error: An unknown exception was caught.");
  }
  return -1;
}
