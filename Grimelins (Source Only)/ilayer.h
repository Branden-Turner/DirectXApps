/*! ====================================================================================================================
  @file isystem.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.24.2011

  @brief 
    Defines the interface for a system

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_ILAYER_H
#define VIRTUS_ILAYER_H

namespace Grimelins
{
    class LayerManager;
    class ILayer;
    class ISystem;

    /////////// System Interface ////////////
    class ILayer
    {
    protected: // Decls
      typedef std::vector<ISystem*> Systems;

    public: // Autos
      virtual ~ILayer(void) {}

    public: // Interface
      virtual void vLoadLayer(void) = 0;
      virtual void vUnloadLayer(void) = 0;
      virtual void vUpdateLayer(void) = 0;
      virtual hstring vGetLayerID(void) const = 0;

    protected: // Implemented helpers
      void LoadMySystems(void);
      void UpdateMySystems(void);
      void UnloadMySystems(void);
      void RemoveAllMySystems(void);

    public: // Methods
      template <typename T> void AddSystem(T const& System);

    private: // Data
      Systems    m_Systems;
      f32        m_LastUpdate;

    protected: // Accessors for children
      inline Systems& getSubSystems(void) { return m_Systems; }

    };
} // namespace Virtus

#include "ilayer.inl" // templated / inline functions


#endif