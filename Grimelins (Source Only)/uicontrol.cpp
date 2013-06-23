/*! ====================================================================================================================
  @file uicontrol.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 27.Sep.2011

  @brief 
    UI control base class implementation.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "uicontrol.h"

namespace Grimelins
{

    //////////////////////////////////////////////////////////////////////////
    // Autos

    UIControl::UIControl(u1 enabled, u1 active) : mEnabled(enabled), 
                                                  mActive(active)
    {

    }

    UIControl::~UIControl(void)
    {

    }

    //////////////////////////////////////////////////////////////////////////
    // Implemented

    void UIControl::Activate(void)
    {
      if (mActive)
        return;

      UIControlChain::iterator it = mChildren.begin();
      UIControlChain::iterator endIt = mChildren.end();
      for (; it != endIt; ++it)
        (*it)->Activate();

      vActivate();
      mActive = true;
    }

    void UIControl::Deactivate(void)
    {
      if (!mActive)
        return;

      UIControlChain::iterator it = mChildren.begin();
      UIControlChain::iterator endIt = mChildren.end();
      for (; it != endIt; ++it)
        (*it)->Deactivate();

      vDeactivate();
      mActive = false;
    }

    void UIControl::Enable(void)
    {
      if (mEnabled)
        return;

      UIControlChain::iterator it = mChildren.begin();
      UIControlChain::iterator endIt = mChildren.end();
      for (; it != endIt; ++it)
        (*it)->Enable();

      vEnable();
      mEnabled = true;
    }

    void UIControl::Disable(void)
    {
      if (!mEnabled)
        return;

      UIControlChain::iterator it = mChildren.begin();
      UIControlChain::iterator endIt = mChildren.end();
      for (; it != endIt; ++it)
        (*it)->Disable();

      vDisable();
      mEnabled = false;
    }

    u1 UIControl::IsActive(void) const
    {
      return mActive;
    }

    u1 UIControl::IsEnabled(void) const
    {
      return mEnabled;
    }

} // namespace Virtus