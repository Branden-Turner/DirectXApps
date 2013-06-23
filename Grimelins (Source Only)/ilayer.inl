/*! ====================================================================================================================
  @file isystem.inl

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.01.2011

  @brief 
    Inline implementations (templates and such) of the system interface.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

namespace Grimelins
{

    typedef std::vector<ISystem*> Subsystems;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // class ILayer

    template <typename T> 
    void ILayer::AddSystem(T const& System)
    {
      hstring name = System.vGetSystemID();

      Systems::iterator it = m_Systems.begin();
      Systems::iterator endIt = m_Systems.end();

      // Check if system already exists
      for (; it != endIt; ++it)
      {
        if ((*it)->vGetSystemID() == name)
        {
          Error("Attempt to add multiple %s systems to %s layer.", name.str(), vGetLayerID().str());
          return; // In release this would result in undefined behavior
        }
      }

      T* sys = new T(System);
      ISystem* isys = DCAST(ISystem*, sys);

      if (!isys)
      {
        ErrorIf("SystemHandler", "Attempt to add non sub system to system.");
        delete sys;
      }
      else
      {
        isys->mLayer = this;
        m_Systems.push_back(isys);
      }
    }

} // namespace Virtus