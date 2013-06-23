/*! ====================================================================================================================
  @file core.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.24.2011

  @brief 
    Defines implementation of core engine.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "ilayer.h"
#include "layermanager.h"


namespace Grimelins
{
    LayerManager::LayerManager(void) : m_LayersLoaded(false)
    {

    }

    LayerManager::~LayerManager(void)
    {
      if (m_LayersLoaded)
        unloadLayers();

      Layers::iterator it = m_Layers.begin();
      Layers::iterator endIt = m_Layers.end();

      for (; it != endIt; ++it)
        delete *it;
    }

    void LayerManager::loadLayers(void)
    {
      m_LayersLoaded = true;

      Layers::iterator it = m_Layers.begin();
      Layers::iterator endIt = m_Layers.end();

      for (; it != endIt; ++it)
        (*it)->vLoadLayer();
    }

    void LayerManager::updateLayers(void)
    {
      Layers::iterator it = m_Layers.begin();
      Layers::iterator endIt = m_Layers.end();

      for (; it != endIt; ++it)
      {
        (*it)->vUpdateLayer();
      }
    }

    void LayerManager::unloadLayers(void)
    {
      Layers::reverse_iterator it = m_Layers.rbegin();
      Layers::reverse_iterator endIt = m_Layers.rend();

      for (; it != endIt; ++it)
        (*it)->vUnloadLayer();

      m_LayersLoaded = false;
    }

} // namespace Virtus
