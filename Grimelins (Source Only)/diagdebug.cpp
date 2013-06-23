/*! ====================================================================================================================
  @file debug.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.15.2011

  @brief 
    Debug Diangostic tools (windows / visual studio)

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "diagdebug.h"


namespace
{
  std::ofstream sLogOut;
}

namespace Grimelins
{
  namespace Application
  {

    cvar write_log("write_log", "Writes a debug log", "0");
    cvar log_file("log_file", "The log file to write to when 'write_log' is enabled", VIRTUS_CONFIG_DEFAULT_LOG_FILE);
    cvar debug_to_console("debug_to_console", "Relay debug information to developer console", "0");

    bool RelayErrorHandler(char const* expression, char const* file, unsigned line, char const* message, ...)
    {
      const int BufSz = 1024;
      char FinalMessage[BufSz];

      int offset = sprintf_s(FinalMessage, "%s(%d) : Assertion Stop\n\n Expression: %s\n\n Error: ", 
        file , line, expression );	

      if (message != NULL)
      {	
        va_list args;
        va_start(args, message);
        offset += vsnprintf_s(FinalMessage + offset, BufSz - offset, _TRUNCATE , message, args);
        va_end(args);
      }
      else
      {
        strcpy_s(FinalMessage + offset, BufSz - offset, "No Error Message");
        offset = strlen(FinalMessage);

        if (sLogOut.is_open())
          sLogOut << FinalMessage << std::endl;
      }

      //Print to visual studio output window
      OutputDebugStringA(FinalMessage);
      OutputDebugStringA("\n\n");

      //Add some stuff for message box
      sprintf_s(FinalMessage + offset, 1024 - offset, "\n\nDo you want to debug break?\n\n");

      //Display a message box to the user
      int ret = MessageBoxA(NULL, FinalMessage, "Stop: Assert", MB_YESNOCANCEL | MB_ICONSTOP);

      if (ret == IDYES)
        return true; // Break
      else if (ret == IDNO)
        return false; // No Break
      else
        exit(-1);

      return true;
    }

    void DebugPrintHandler(const char* msg, ... )
    {
      const int BufSz = 1024;
      char FinalMessage[BufSz];

      va_list args;
      va_start(args, msg);
      vsnprintf_s(FinalMessage, BufSz , _TRUNCATE , msg, args);
      va_end(args);


      if (VIRTUS_CONFIG_PRINT_VS)
        OutputDebugStringA(FinalMessage);

      if (gCore->IsRunning() && debug_to_console)
      {
        RawStore arg;
        arg.AddString("message", FinalMessage);
        gCore->Events.Post("con_print", arg, MSG_PRIORITY_MEDIUM);
      }

      if (sLogOut.is_open())
        sLogOut << FinalMessage << std::endl;
    }

    void StartDebugDiag(void)
    {
      if (write_log)
        sLogOut.open((ch const*)log_file);
    }

  }
}

