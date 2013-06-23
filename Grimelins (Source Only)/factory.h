/*! ====================================================================================================================
  @file factory.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  May.24.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_FACTORY_H
#define VIRTUS_FACTORY_H

#include "component.h"
#include "object.h"
#include "behavior.h"

namespace Grimelins
{
  class Transform;
  class Body;
  class gfxShape;
  class gfxModel;
  class Data;
  class Speaker;

    //FWD DEC

    //THIS IS A SCRIPT BOUND CLASS
    SCRIPT_HeaderDefine(Factory);

    class Factory : public ISystem, public IListener
    {
    public: // ANT section
      ANT__Bar(bar);

    public: //SCRIPT methods
      #define FACTORY_COMPONENT_GET_DECL(cmp) cmp* get##cmp(Object& obj)
        FACTORY_COMPONENT_GET_DECL(Transform);
        FACTORY_COMPONENT_GET_DECL(Body);
        FACTORY_COMPONENT_GET_DECL(gfxShape);
        FACTORY_COMPONENT_GET_DECL(gfxModel);
        FACTORY_COMPONENT_GET_DECL(Data);
        FACTORY_COMPONENT_GET_DECL(Speaker);
      #undef FACTORY_COMPONENT_GET_DECL

    public: // CTOR&DTOR
      Factory(void);
      Factory(Factory const& rhs);
      virtual ~Factory(void);

    public: // ISystem Interface
      virtual void vLoadSystem(void);
      virtual void vUpdateSystem(void);
      virtual void vUnloadSystem(void);
      virtual hstring vGetSystemID(void) const;
      virtual void vRegisterComponent(TypeID id, Component* component);
      virtual void vUnregisterComponent(TypeID id, Component* component);

    private: // IListener interface
      virtual bool handleEvent( hstring const& message, RawStore const& args, MessageType type, f32 timestamp);
      virtual u1 handleRequest( hstring const& request, RawStore& data );

    public:// Public Interface (use this stuff)
      void dumpObjects( void );
      Object* createObject(Archetype const& archName);
      Object* partialCreateObject(Archetype const& archName);
      void refreshBehaviors(void);
      void loadLevel(Text const& name);
      void unloadLevel(void);
      void switchLevel(Text const& name);
      void saveStateAsLevel(Text const& LevelName = "");
      void saveLevelAs(Text const& name);//same as above, except bound to script
      void loadObjectLibrary(void);
      bool archetypeOnDemand(Text const& name);
//       Object* scriptCreateObject(Text const& name, Text const& instance);
      void attachObjectToBone( Object& obj, Text const& boneName, bool bound = false );
      ScriptVec3 getBonePos(Text const& boneName) const;

      void buildModelViewLevel(Text const& file, bool writeGMDL=false);

      Object* getObjectByID(ObjectID id);

#ifdef ANT_TWEAK_ENABLED
    public://ANT special interface
      Object* activeAntObject;
      void setActiveAntObject(Object* obj);
#endif

    private://Object Direct Methods (that means don't use them!)
      friend class Object;
      friend class Level;
      void o_Delete(Object const& object);
      Object* o_New(Object const& source);
      Component* o_Cmp(Component const& cmp);
      Component* o_Cmp(Text const& name, Scope& scope);
      Behavior* o_Behavior(BehaviorID const& name);

      Object* allocateAndSerializeObject(Object const& source);
      void invokeCreateForObject(Object & obj);

    private://internal use methods
      void defineComponent(Text const& name, TypeID const& id, componentCreator cmpCreationFn);
      void ConstructTemplates(void);
      void CleanupDeletedObjects(void);
      void recompileBehavior(Text const& file);
      void preloadLevels(void);
      void updateAttachedObjects(void);
      void updateHUD();
      void updateBossHUD();
      void updatePlayerHUD();
      
    private://internal data types
      typedef std::map<Text, componentCreator> CmpCreateByName;
      typedef std::map<TypeID, componentCreator> CmpCreateByTypeID;
      typedef std::map<BehaviorID, Behavior*> BehaviorMap;
      typedef std::map<ObjectID, Object*> ObjectMap;
      typedef std::map<Archetype, Object*> ArchetypeMap;
      typedef std::set<ObjectID> ObjectDeleteList;
      typedef std::vector<Level*> LevelArray;
      typedef std::map<Text, u32> LevelIndexMap;
      typedef std::vector<Object*> ObjectList;

    private: //data members
      hstring m_IDName;
      ArchetypeMap m_archetypes;
      ObjectMap m_objects;
      BehaviorMap m_behaviors;
      CmpCreateByName m_createByName;
      CmpCreateByTypeID m_createByID;
      ObjectID m_objectIDs;
      ComponentID m_componentIDs;
      ObjectDeleteList m_deletedObjects;
      LevelArray m_levels;
      LevelIndexMap m_levelIndicies;
      u32 m_curLevel;
      u1 m_loadingArchetypes;
      Text m_nextLevel;
    };
    extern Factory* gFactory;
}//namespace

#endif