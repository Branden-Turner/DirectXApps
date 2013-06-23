/*! ====================================================================================================================
  @file core.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.24.2011

  @brief 
    Core engine

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once

namespace Grimelins
{
    class ILayer;


    class LayerManager
    {
    private: // Denied
      LayerManager(LayerManager const& rhs);
      LayerManager& operator=(LayerManager const& rhs);

    public: // Autos
      LayerManager(void);
      virtual ~LayerManager(void);

    public: // Methods
      template <typename T> void AddLayer(T const& Layer);

    friend class Core;
    private:
      void loadLayers(void);
      void updateLayers(void);
      void unloadLayers(void);

    protected: // Decls
      typedef std::vector<ILayer*> Layers;

    protected: // Data
      Layers  m_Layers;
      u1      m_LayersLoaded;
    };
} // namespace Virtus

#include "layermanager.inl" // Templated / inline functions
