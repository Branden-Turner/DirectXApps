/*! ====================================================================================================================
  @file view.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.14.2011

  @brief 
    View system. Manages plugged in views.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "view.h"
#include "syswindow.h"
#include "sysinput.h"
#include "graphics.h"

#ifdef SOUNDSYS_V2
#include "syssound.h"
#else
#include "sound.h"
#endif

namespace Grimelins
{

// autos
    
    ViewLayer::ViewLayer(void)
    {
    }

    ViewLayer::ViewLayer( RawStore const& startupinfo )
    {
      instance = (HINSTANCE)startupinfo.GetS32("instance");
      prevInstance = (HINSTANCE)startupinfo.GetS32("prev_instance");
      cmdShow = startupinfo.GetS32("cmd_show");

      ErrorIf(!instance, "Invalid application instance received");
    }

    ViewLayer::~ViewLayer(void)
    {

    }

// virtual interface

    void ViewLayer::vLoadLayer(void)
    {
      // Add systems here
      this->AddSystem(Window(instance, prevInstance, cmdShow));
      this->AddSystem(Input());
      this->AddSystem(Graphics());
      this->AddSystem(Sound());

      this->LoadMySystems();
    }

    void ViewLayer::vUnloadLayer(void)
    {
      this->UnloadMySystems();

      this->RemoveAllMySystems();
    }

    void ViewLayer::vUpdateLayer(void)
    {
      this->UpdateMySystems();
    }

    hstring ViewLayer::vGetLayerID(void) const
    {
      return H("Views");
    }
} // namespace Virtus