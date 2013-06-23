/*! ====================================================================================================================
  @file window.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.03.2011

  @brief 
    Window class - Microsoft/Windows32 Specific

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "syswindow.h"
#include "sysinput.h"

// #include "Squirrel/importSquirrel.h"
#include "Shellapi.h"
#include <stdio.h>
#include "factory.h"//factory creates the model viewer for models on drag_drop

namespace Grimelins
{
    /////////// Static Members ////////////
    HWND Window::mHWND = 0;


    /////////// Autos ////////////
    Window::Window(Application::OSINSTANCE instance, Application::OSINSTANCE prevInstance, int cmdShow)
      : mIDName(H("Window")),
        mInstance(instance),
        mPrevInstance(prevInstance),
        mCmdShow(cmdShow)
    {

    }

    Window::~Window(void)
    {

    }

    void Window::vLoadSystem(void)
    {
#pragma message(Reminder "Clean this configuration mess up")
      WNDCLASS wc;
      ZeroMemory(&wc, sizeof(WNDCLASS));

      wc.style         = CS_HREDRAW | CS_VREDRAW;
      wc.lpfnWndProc   = WindowProcess;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.hInstance     = mInstance;
      wc.hIcon         = 0; // LoadIcon (mInstance, MAKEINTRESOURCE(IDI_MAINICON));
      wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
      wc.lpszMenuName  = NULL;
      wc.lpszClassName = "VirtusWindowClass";

      ATOM regClass = RegisterClass(&wc);
      ErrorIf(!regClass, "Could not create Window");

      //create the window at 3/4 the current screen resolution
      f32 ar = SCAST(f32,GetSystemMetrics(SM_CYSCREEN))/GetSystemMetrics(SM_CXSCREEN);
      s32 winWidth = SCAST(s32, SCAST(f32, GetSystemMetrics(SM_CXSCREEN))/1.5f);
      s32 winHeight = SCAST(s32,winWidth*ar);

      //       if(winWidth > 2560 || winWidth < 320)
      //         winWidth = CONFIG_GFX_DEFAULT_WINDOW_WIDTH;
      //       if(winHeight > 1600 || winHeight < 240)
      //         winHeight = CONFIG_GFX_DEFAULT_WINDOW_HEIGHT;

      RECT R = {0, 0, winWidth, winHeight};
      AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

      mHWND = CreateWindow( 
        "VirtusWindowClass", 
        "Grimelin Gladiator"/*CONFIG_GENERAL_TITLE*/,
        WS_OVERLAPPEDWINDOW,
        100, 
        100, 
        R.right, 
        R.bottom, 
        0, 
        0, 
        mInstance,
        0); 

      ShowWindow(mHWND, mCmdShow);
      UpdateWindow(mHWND);
      DragAcceptFiles(mHWND, TRUE);
    }

    void Window::TrackMouse(void)
    {
      static TRACKMOUSEEVENT mt;
      mt.cbSize = sizeof(mt);
      mt.dwFlags = TME_LEAVE | TME_HOVER;
      mt.hwndTrack = mHWND;
      mt.dwHoverTime = HOVER_DEFAULT;

      if (!TrackMouseEvent(&mt))
        printDebug(Application::GetLastError("TrackMouseEvent").c_str());
    }

    void Window::vUpdateSystem(void)
    {

      MSG msg;
      SecureZeroMemory(&msg, sizeof(MSG));

      while( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) )
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    void Window::vUnloadSystem(void)
    {

    }

    LRESULT Window::MessagePump(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
      if (gCore->IsRunning())
      {

        ANT__Event_Handle(hwnd, msg, wParam, lParam);

        switch( msg )
        {

        case WM_ACTIVATE:
          if (wParam == 0)
            gCore->Events.Post("focus_lost", MSG_PRIORITY_HIGH | MSG_NOT_CONSUMABLE);
          else
            gCore->Events.Post("focus_gained", MSG_PRIORITY_HIGH | MSG_NOT_CONSUMABLE);

          return 0;

        case WM_SETFOCUS:
          return 0;

        case WM_KILLFOCUS:
          return 0;

        case WM_CANCELMODE:
          return 0;

        case WM_MOVING:
          return 0;


        case WM_MOVE:
          return 0;

        case WM_SIZE:
          return 0;

        case WM_EXITSIZEMOVE:
          return 0;

        case WM_DESTROY:
        case WM_QUIT:
        case WM_CLOSE:
          gCore->Stop();
          return 0;

        case WM_DROPFILES:
          {
            HDROP hd = (HDROP)wParam;
            DWORD nf = DragQueryFile(hd, 0xFFFFFFFF, NULL, 0);
            static const int buflen = _MAX_PATH + 1;
            char buff[buflen];
            for(DWORD names = 0; names < nf; ++names)
            {
              SecureZeroMemory(buff, buflen);
              if(DragQueryFile(hd, names, buff, buflen))
              {

                Application::FilePath fp(buff);
                if(fp.Extension() == "nut")
                {
                  //if an event file was drag_dropped tell factory
                  Text fn = fp.Name();
                  fn = fn.substr(0, fn.find_first_of("_"));
                  if(fn == "event")
                  {
                    RawStore ca;
                    ca.AddString("file", fp.Name());
                    gCore->Events.PostNow(H("DragBehavior"), ca, MSG_PRIORITY_HIGH);
                  }
                  else
                    gCore->Scripter.runScript(fp.FileName());
                }
                else if(fp.Extension() == "bin")
                {
                  FACTORY->buildModelViewLevel(fp.FileName());
                }
              }
            }
            DragFinish((HDROP)wParam);
            break;
          }

          // Input
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_CHAR:
        case WM_DEADCHAR:
        case WM_HOTKEY:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_MOUSEWHEEL:
        case WM_MOUSEMOVE:
        case WM_MOUSELEAVE:
        case WM_MOUSEHOVER:
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDOWN:
          if (gCore->IsRunning())
            Input::HandleInputMessage(msg, wParam, lParam);
          return 0;
        }
      }

      return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK Window::WindowProcess(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    { 
      return Window::MessagePump(hwnd, message, wParam, lParam);
    }
} // namespace Virtus