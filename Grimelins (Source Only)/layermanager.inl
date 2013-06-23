/*! ====================================================================================================================
  @file systemmanager.inl

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.01.2011

  @brief 
    Templated portions of the system manager interface.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

namespace Grimelins
{

    template <typename T> 
    void LayerManager::AddLayer(T const& Layer)
    {
      T* layer = new T(Layer);
      ILayer* ilayer = DCAST(ILayer*, layer);

      if (!ilayer)
      {
        ErrorIf("LayerHandler", "Attempt to add non layer to layer handler.");
        delete layer;
      }
      else
      {
        m_Layers.push_back(ilayer);
      }
    }

} // namespace Virtus
