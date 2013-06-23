/*! ====================================================================================================================
  @file osutils.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.04.2011

  @brief 
    Operating system utilities.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include "Strsafe.h"

#include "apputilities.h"

#pragma comment(lib, "Version.lib")

namespace Grimelins
{

  namespace Application
  {

    void FullStopError( ch const* message )
    {
      MessageBoxA(NULL, message, "Full Stop Error", MB_OK | MB_ICONSTOP);
      exit(-1);
    }

    u1 YesNoQuestion( ch const* question )
    {
      s32 res = MessageBoxA(NULL, question, "Do you want to continue?", MB_YESNO | MB_ICONQUESTION);

      if (res == IDYES)
        return true;
      else
        return false;
    }

    void Sleep(f32 seconds)
    {
      if (seconds <= 0.f)
        return;

      Timer t; 
      t.StartTimer(); 

      ::Sleep(SCAST(DWORD, .9f * seconds * 1000.f)); 

      while(t.Get() < seconds);
    }

    void* aligned_malloc(size_t size, size_t alignment)
    {
      return _aligned_malloc(size, alignment);
    }

    void aligned_free(void* data)
    {
      _aligned_free(data);
    }

    u1 GetProductAndVersion(std::string& strProductName, std::string& strProductVersion)
    {
      // get the filename of the executable containing the version resource
      TCHAR szFilename[MAX_PATH + 1] = {0};
      if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
        return false;

      // allocate a block of memory for the version info
      DWORD dummy;
      DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
      if (dwSize == 0)
        return false;
      
      std::vector<BYTE> data(dwSize);

      // load the version info
      if (!GetFileVersionInfo(szFilename, NULL, dwSize, &data[0]))
        return false;

      // get the name and version strings
      LPVOID pvProductName = NULL;
      unsigned int iProductNameLen = 0;
      LPVOID pvProductVersion = NULL;
      unsigned int iProductVersionLen = 0;

      // replace "040904e4" with the language ID of your resources
      if (!VerQueryValue(&data[0], "\\StringFileInfo\\040904e4\\ProductName", &pvProductName, &iProductNameLen) ||
        !VerQueryValue(&data[0], "\\StringFileInfo\\040904e4\\ProductVersion", &pvProductVersion, &iProductVersionLen))
        return false;

      strProductName.assign((LPCTSTR)pvProductName, iProductNameLen);
      strProductVersion.assign((LPCTSTR)pvProductVersion, iProductVersionLen);

      return true;
    }


    u1 QueryPerformanceFrequency(lInt* frequency)
    {
      return ::QueryPerformanceFrequency(RCAST(LARGE_INTEGER*, frequency)) != 0;
    }

    u1 QueryPerformanceCounter(lInt* counter)
    {
      return ::QueryPerformanceCounter(RCAST(LARGE_INTEGER*, counter)) != 0;
    }
    
    OSMODULE LoadLib(std::string filename)
    {
      return LoadLibraryA(filename.c_str());
    }

    Grimelins::Application::OSFARPROC GetFunctionAddress(OSMODULE inst, std::string const& fn)
    {
      return GetProcAddress(inst, fn.c_str());
    }

    std::string GetLastError(ch const* lpszFunction) 
    { 
      // Retrieve the system error message for the last-error code

      LPVOID lpMsgBuf;
      LPVOID lpDisplayBuf;
      DWORD dw = ::GetLastError(); 

      FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );


      lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 

      StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 

      std::string msg((LPCTSTR) lpDisplayBuf);
      LocalFree(lpMsgBuf);
      LocalFree(lpDisplayBuf);

      return msg;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Threading

    void CloseHandle(OSHANDLE handle)
    {
      ::CloseHandle(handle);
    }

    OSHANDLE MakeThread(ThreadProc fn, void* data, u1 suspendedStart, u32* idDescriptor)
    {
      return (OSHANDLE) _beginthreadex(0, 0, fn, data, (suspendedStart) ? CREATE_SUSPENDED : 0, idDescriptor);
    }
    
    void WaitForThread(OSHANDLE Thread, f32 Time)
    {
      if (Thread)
      {
        u32 time;

        if (Time < 0.f) time = SCAST(u32, Time * 1000.f); // Convert to milli seconds
        else            time = INFINITE;

        WaitForSingleObjectEx(Thread, time, false);
      }
    }
    
    void KillThread(OSHANDLE Thread, s32 code)
    {
      if (Thread)
        TerminateThread(Thread, code);
    }

    u1 PauseThread(OSHANDLE Thread)
    {
      return SuspendThread(Thread) != ((DWORD)(-1));
    }

    u1 UnPauseThread(OSHANDLE Thread)
    {
      return ResumeThread(Thread) != ((DWORD)(-1));
    }

    namespace
    {
      //
      // Usage: SetThreadName (-1, "MainThread");
      //
      typedef struct tagTHREADNAME_INFO
      {
        DWORD dwType; // must be 0x1000
        LPCSTR szName; // pointer to name (in user addr space)
        DWORD dwThreadID; // thread ID (-1=caller thread)
        DWORD dwFlags; // reserved for future use, must be zero
      } THREADNAME_INFO;

      void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
      {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = szThreadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;

        __try
        {
          // Throw structured exception with debugger as target
          RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info);
        }
        __except(EXCEPTION_CONTINUE_EXECUTION)
        {
          // Swallow exception when no debugger is attached
        }
      }
    }

    void NameThread(u32 id, ch const* name)
    {
      SetThreadName(id, name);
    }

  } // namespace OS
} // namespace Virtus




