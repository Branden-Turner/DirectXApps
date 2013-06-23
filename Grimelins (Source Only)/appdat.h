/*! ====================================================================================================================
  @file appdat.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.17.2011

  @brief 
    System dependent data declarations.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_APPDAT_H
#define VIRTUS_APPDAT_H

namespace Grimelins
{
  namespace Application
  {
    typedef HWND OSWINDOW;
    typedef HINSTANCE OSINSTANCE;
    typedef HMODULE OSMODULE;
    typedef HANDLE OSHANDLE;

    typedef u32 (_stdcall *ThreadProc)(void* data);
    typedef FARPROC OSFARPROC;

  } // namespace Application
} // namespace Virtus

#endif // Include Guard