/*! ====================================================================================================================
  @file isystem.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Jun.15.2011

  @brief 
    System interface.
    Sep.23.2011 Updated GetSystemDT to provide an average time for a smoother frameTime-> 

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_ISYSTEM_H
#define VIRTUS_ISYSTEM_H

namespace Grimelins
{
    class ILayer;
    class Component;

    /////////// Subsystem Interface ////////////
    class ISystem
    {
      friend ILayer;

    public: // Autos
      ISystem(void);
      virtual ~ISystem(void) {}

    public: // Abstract Interface
      virtual void vLoadSystem(void) = 0;
      virtual void vUnloadSystem(void) = 0;
      virtual void vUpdateSystem(void) = 0;
      virtual hstring vGetSystemID(void) const = 0;
      virtual void vRegisterComponent(TypeID id, Component* component){};
      virtual void vUnregisterComponent(TypeID id, Component* component){};

    private: // Data
      ILayer* mLayer;

    protected: // Accessors for children
      inline ILayer* myLayer(void) const { return mLayer;      }
      f32 GetSystemDT(void);
  
    };
}

#endif // Include Guard