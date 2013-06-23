/*! ====================================================================================================================
  @file window.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.03.2011

  @brief 
    Window Class

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_WINDOW_H
#define VIRTUS_WINDOW_H

namespace Grimelins  
{
    class Window : public ISystem
    {   
    private:
      hstring     mIDName;
      HINSTANCE   mInstance;
      HINSTANCE   mPrevInstance;
      int         mCmdShow;
      static HWND mHWND;

    public:
      Window(Application::OSINSTANCE instance, Application::OSINSTANCE prevInstance, int cmdShow);
      virtual ~Window(void);

    public: // ISystem interface
      virtual void vLoadSystem(void);
      virtual void vUpdateSystem(void);
      virtual void vUnloadSystem(void);
      virtual hstring vGetSystemID(void) const { return mIDName; }

    public: // methods
      static void TrackMouse(void);

    private: // ISubscriber interface

    private: 
      static LRESULT CALLBACK WindowProcess(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
      static LRESULT MessagePump(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
      void createWindow(void);


    public: // encaps
      static Application::OSWINDOW mWinHandle(void) { return mHWND; }
      static void mWinHandle(Application::OSWINDOW val) { mHWND = val; }

    };

}// namespace Virtus

#endif // Include Guard
