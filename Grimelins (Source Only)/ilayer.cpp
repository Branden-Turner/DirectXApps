/*! ====================================================================================================================
  @file ilayer.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.15.2011

  @brief 
    Implementation of helper methods for layers.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "isystem.h"
#include "ilayer.h"


namespace Grimelins
{
    void ILayer::LoadMySystems(void)
    {
      Systems::iterator it = m_Systems.begin();
      Systems::iterator endIt = m_Systems.end();

      for (; it != endIt; ++it)
      {
        (*it)->vLoadSystem();
        //(*it)->m_LastUpdate = gCore->Time.Get();
      }
    }

    void ILayer::UpdateMySystems(void)
    {
      Systems::iterator it = m_Systems.begin();
      Systems::iterator endIt = m_Systems.end();

      for (; it != endIt; ++it)
      {
        //(*it)->m_LastUpdate = gCore->Time.Get() - (*it)->m_LastUpdate;
        (*it)->vUpdateSystem();
      }
    }

    void ILayer::UnloadMySystems(void)
    {
      Systems::reverse_iterator it = m_Systems.rbegin();
      Systems::reverse_iterator endIt = m_Systems.rend();

      for (; it != endIt; ++it)
        (*it)->vUnloadSystem();
    }

    void ILayer::RemoveAllMySystems(void)
    {
      Systems::reverse_iterator it = m_Systems.rbegin();
      Systems::reverse_iterator endIt = m_Systems.rend();

      for (; it != endIt; ++it)
        delete *it;

      m_Systems.clear();
    }

} // namespace Virtus