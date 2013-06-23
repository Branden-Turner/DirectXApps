/*! ====================================================================================================================
  @file factory.cpp

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  May.24.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "factory.h"
#include "object.h"
#include "behavior.h"
#include "level.h"
#include "graphics.h"
#include "scriptGlobal.h"
#include "uisystem.h"
#include "dx9resourcemanager.h"
#include "dx9device.h"
//INCLUDE YOUR COMPONENTS HERE
#include "compbody.h"
#include "comptransform.h"
#include "cmpgfx.h"
#include "cmpData.h"
#include "compspeaker.h"
#include "userinterface.h"

#include <ctime>

#define OBJECT_ARCHETYPE_FILE_DIR "assets\\objects\\"
#define OBJECT_ARCHTYPE_FILE "assets\\object_templates.gof"
///////////////////////////////////////////////////////////////////////////////////////////////

///////////       //////        /////    ////////////       /////         //////     //     //
//               ///  ///      ///          ///           ///   //       //   //     //    //
/////           /////////      //           ///           //    //       // ///       //  //
//             ///     ///     ///          ///           //   //        ///  //        ///
//            ///      ///      ///  //     ///           //  //         //    //      //
//            ///      ///       /////      ///            ////          //     //    //

//////////////////////////////////////////////////////////////////////////////////////////////


namespace Grimelins
{
  // GLOBAL FACTORY INSTANCE
  Factory* gFactory=NULL;
  Factory* FACTORY;
  static const u32 NO_LEVEL_LOADED = -1;
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //SCRIPT BINDING
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //Factory implementation of script functions to get components from an object
  // Script calling syntax: gFactory.getXXXXX(obj); where XXXXX is a component, and obj is the object in script
  //EX:  local myTransform = gFactory.getTransform(obj);
#define FACTORY_COMPONENT_GET_IMPL(cmp) cmp* Factory::get##cmp(Object& obj){ return obj.getComponentT<cmp>();}

  FACTORY_COMPONENT_GET_IMPL(Transform);
  FACTORY_COMPONENT_GET_IMPL(Body);
  FACTORY_COMPONENT_GET_IMPL(gfxShape);
  FACTORY_COMPONENT_GET_IMPL(gfxModel);
  FACTORY_COMPONENT_GET_IMPL(Data);
  FACTORY_COMPONENT_GET_IMPL(Speaker);
#undef FACTORY_COMPONENT_GET_IMPL

  //Actual Factory script bindings
  SCRIPT_ClassStart(Factory);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(dumpObjects);
  SCRIPT_Method(loadLevel);
  SCRIPT_Method(unloadLevel);
  SCRIPT_Method(switchLevel);
  SCRIPT_Method(loadObjectLibrary);
  SCRIPT_Method(saveLevelAs);

  SCRIPT_Method(getTransform);
  SCRIPT_Method(getBody);
  SCRIPT_Method(getgfxShape);
  SCRIPT_Method(getgfxModel);
  SCRIPT_Method(getData);
  SCRIPT_Method(getSpeaker);

  SCRIPT_Method(refreshBehaviors);
  SCRIPT_Method(createObject);
  SCRIPT_Method(attachObjectToBone);
  SCRIPT_Method(getBonePos);
  SCRIPT_Method(getObjectByID);

  SCRIPT_ClassEnd();

  ANT__RETVAL ANT__FacRefreshBehaviors( void* ud)
  {
    gFactory->refreshBehaviors();
  }

  ANT__RETVAL ANT__AddRemObjBehavior( void* beh)
  {
    #ifdef ANT_TWEAK_ENABLED
    if(!gFactory->activeAntObject)
      return;
    Behavior* b = SCAST(Behavior*, beh);
    if(gFactory->activeAntObject->hasBehavior(b->id))
      gFactory->activeAntObject->removeBehavior(b);
    else
      gFactory->activeAntObject->addBehavior(b);
    #endif
  }

  ANT__RETVAL ANT__FacSaveLevel( void* vp)
  {
    FACTORY->saveStateAsLevel();
  }


  #ifdef ANT_TWEAK_ENABLED
  std::list<Text> GMDL_modelList;
  bool GMDL_record = false;
  #endif


  ANT__RETVAL ANT__FacRecordModel( void* beh)
  {
    #ifdef ANT_TWEAK_ENABLED
    GMDL_record = !GMDL_record;
    if(!GMDL_record)
    {
      if(GMDL_modelList.size() > 0)
        gFactory->buildModelViewLevel(GMDL_modelList.front(), true);
      ANT__Define(gFactory->bar, "/BMG label='START MODEL GROUP'");
    }
    else
    {
      ANT__Define(gFactory->bar, "/BMG label='SAVE MODEL GROUP'");
    }

    #endif
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //CTOR&DTOR
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Factory::Factory( void )
    : m_IDName(H("BrokenFactory"))
  {
  }

  Factory::Factory( Factory const& rhs )
    : m_IDName(H("Factory"))
  {
    SCRIPT_RegisterGlobal(Factory);
  }

  Factory::~Factory( void )
  {
    ErrorIf(m_archetypes.size(), "Warning: Factory shutdown with unreleased object archetypes.\n\n This will only happen if the Factory was not unloaded before the destructor call.");
    ErrorIf(m_objects.size(), "Warning: Factory shutdown with unreleased objects.\n\n This will only happen if the Factory was not unloaded before the destructor call.");
    ErrorIf(m_behaviors.size(), "Warning: Factory shutdown with unreleased behaviors.\n\n This will only happen if the Factory was not unloaded before the destructor call.");
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //ISystem Interface
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Factory::vLoadSystem( void )
  {
    ErrorIf(gFactory, "Multiple factories.");
    gFactory = this;
    FACTORY = this;
    m_objectIDs = 0;
    m_componentIDs = 0;
#ifdef ANT_TWEAK_ENABLED
    activeAntObject = NULL;
#endif

    gCore->Events.Subscribe(H("switchLevel"), this);

    //sent once all events have been serialized
    gCore->Events.Subscribe(H("EventsReady"), this);

    //sent when a user drag-drops a behavior (that is "event_xxx.nut")
    gCore->Events.Subscribe(H("DragBehavior"), this);

    //object archetypes are serialized upon recipt of this message
    gCore->Events.Subscribe(H("LoadObjectLibrary"), this);

    srand((unsigned)time(0));

    m_loadingArchetypes = false;
    //listen for when graphics gets the ANT system initialized
    ANT__LISTEN(this);

    // ************** DEFINE COMPONENTS HERE **************** //
    
    DEFINE_COMPONENT(Transform);
    DEFINE_COMPONENT(Body);
    DEFINE_COMPONENT(gfxModel);
    DEFINE_COMPONENT(gfxShape);
    DEFINE_COMPONENT(Data);
    DEFINE_COMPONENT(Speaker);

    // ****************************************************** //
  }

  void Factory::vUpdateSystem( void )
  {
    if(m_loadingArchetypes)
      ConstructTemplates();

    if(m_nextLevel != "")
    {
      switchLevel(m_nextLevel);
      m_nextLevel = "";
    }

    CleanupDeletedObjects();

    if(!gCore->GamePaused)
    {
      updateAttachedObjects();
      updateHUD();
    }

    RawStore dt;
    dt.AddF32("dt", 1);
    
    ObjectMap::iterator it = m_objects.begin();
    for(;it != m_objects.end(); ++it)
    {
      it->second->scriptCall("update", dt);
    }
  }

  void Factory::vUnloadSystem( void )
  {
    unloadLevel();
    ErrorIf(m_objects.size(), "Factory shutdown with %d unreleased objects.\n", m_objects.size());

    while(m_objects.size())
    {
      delete m_objects.begin()->second;
      m_objects.erase(m_objects.begin());
    }
    while(m_archetypes.size())
    {
      delete m_archetypes.begin()->second;
      m_archetypes.erase(m_archetypes.begin());
    }
    while(m_behaviors.size())
    {
      delete m_behaviors.begin()->second;
      m_behaviors.erase(m_behaviors.begin());
    }

    gFactory = NULL;
  }

  hstring Factory::vGetSystemID( void ) const
  {
    return m_IDName;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //IListener Interface
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool Factory::handleEvent( hstring const& message, RawStore const& args, MessageType type, f32 timestamp )
  {
    MSG_StartHandling();
    MSG_ConsumeIf("switchLevel")
    {
      m_loadingArchetypes = true;
      m_nextLevel = args.GetString("level");
    }
    MSG_ConsumeIf("EventsReady")
    {
      preloadLevels();
    }
    MSG_PeekIf("DragBehavior")
    {
      Text file = args.GetString(H("file"));
      if(file != "")
      {
        recompileBehavior(file);
        return true;
      }
    }
    MSG_ConsumeIf("LoadObjectLibrary")
    {
      m_loadingArchetypes = true;
    }
    ANT__MSG_HANDLE()
    {
      ANT__Bar_Init(bar, "Factory", false);
      //add self to core bar
      ANT__Bar_AddButton_P(ANT__GetBarP(bar, gCore), "Factory", ANT__Toggle, ANT__PBar(ANT__GetBarP(bar, gFactory)), "group=Systems" );
      //setup hide button
      ANT__Bar_AddButton(bar, "HIDE", ANT__Hide, ANT__PBar(bar));
      ANT__Bar_AddSeperatorAny(bar);//================================================
      ANT__Bar_AddButton(bar, "SAVE", ANT__FacSaveLevel, NULL);
      ANT__Bar_AddSeperatorAny(bar);//================================================
      ANT__Bar_AddButton_P(bar, "BMG", ANT__FacRecordModel, NULL, "label='GROUP MODELS'");
      ANT__Bar_AddSeperatorAny(bar);//================================================
      ANT__Bar_AddButton_P(bar, "rfshbeh", ANT__FacRefreshBehaviors, NULL, "label='Reload Behaviors'");
      ANT__Bar_AddSeperatorAny(bar);//================================================
      ANT__Bar_AddButton_P(bar, "AO_object", NULL, NULL, "group=AOMain label='<None Set>'");//Active Object button
      ANT__Define(bar, "/AOMain label='Active Object' opened=false");//Active Object group
      ANT__Bar_AddButton_P(bar, "AO_BehaviorList", NULL, NULL, "group=AOBehaviors label='Available Behaviors'");//Active Object button
      ANT__Define(bar, "/AOBehaviors group=AOMain label='Add|Remove Behaviors' opened=false");//Behaviors Active Object subgroup
      ANT__Bar_AddSeperatorAny(bar);//================================================
    }
    
    MSG_DoneHandling();
  }

  Grimelins::u1 Factory::handleRequest( hstring const& request, RawStore& data )
  {
    REQ_StartHandling();
    REQ_HandleIf("newObj")
    {

    }
    REQ_DoneHandling();
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Public Interface (use this stuff)
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Factory::dumpObjects( void )
  {
    printDebug("\n*** Factory Objects ***\n");
    printDebug("  -- Object Archetypes --");
    ArchetypeMap::iterator archIt = m_archetypes.begin();
    for(;archIt!=m_archetypes.end(); ++archIt)
    {
      std::stringstream ss;
      Object*& obj = archIt->second;
      ss << "\n    " << obj->m_archetype << " (" << obj->m_id << "::0x" << RCAST(int, obj) << ")\n";
      Object::ComponentMap::iterator cmpIt = obj->m_components.begin();
      for(;cmpIt!=obj->m_components.end(); ++cmpIt)
        ss << "        [" << cmpIt->second->getTypeID() << "] " << cmpIt->second->getTypeName() << "(" << cmpIt->second->getInstanceID() << "::0x" << RCAST(int, cmpIt->second) << ")\n";
      ss << "\n";
      printDebug(ss.str().c_str());
    }
  }

  Object* Factory::createObject( Archetype const& archName )
  {
      Object* newObj = partialCreateObject(archName);
      if(newObj!=NULL) invokeCreateForObject(*newObj);
      return newObj;
  }

  Object* Factory::partialCreateObject( Archetype const& archName )
  {
    if(archetypeOnDemand(archName))
    {
      Object* newObj = allocateAndSerializeObject(*m_archetypes[archName]);
      return newObj;
    }
    else
    {
      ErrorIf(true, "ERROR Archetype aquisition failure.");
      return NULL;
    }  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Object Direct Methods (that means don't use them!)
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void Factory::o_Delete( Object const& object )
  {
    ErrorIf(object.getInstanceName() == OBJECT_ARCHTYPE_NAME, "destroy() called on an object archetype (or it somehow being directly passes to o_Delete(). Deleting archetypes this way is not allowed.");
    ANT__Bar_Hide(ANT__GetBar(bar, object));
    m_deletedObjects.insert(object.m_id);
  }

  Object* Factory::o_New( Object const& source )
  {
    if(archetypeOnDemand(source.m_archetype))
    {
      Object* newObj = allocateAndSerializeObject(source);
      invokeCreateForObject(*newObj);
      return newObj;
    }
    else
    {
      ErrorIf(true, "ERROR Archetype aquisition faulure.");
      return NULL;
    }
  }

  Component* Factory::o_Cmp( Component const& cmp )
  {
    ErrorIf(m_createByID.find(cmp.getTypeID())==m_createByID.end(), "Factory recieved a request to create an invalid component type.\nComponent: %s\nID %u (not found)\n", cmp.getTypeName().c_str(), cmp.getTypeID());
    Component* newCmp = m_createByID[cmp.getTypeID()]();
    ErrorIf(newCmp==NULL, "Factory was returned a NULL pointer from a component creator for '%s'!", cmp.getTypeName().c_str());
    newCmp->m_id = ++m_componentIDs;
    newCmp->serialize(&cmp);
    return newCmp;
  }

  Component* Factory::o_Cmp( Text const& name, Scope& scope )
  {
    ErrorIf(m_createByName.find(name)==m_createByName.end(), "Factory recieved a request to create an invalid component type '%s'.\n", name.c_str());
    Component* newCmp = m_createByName[name]();
    ErrorIf(newCmp==NULL, "Factory was returned a NULL pointer from a component creator for '%s'!", name.c_str());
    newCmp->m_id = ++m_componentIDs;
    newCmp->serialize(scope, true);
    return newCmp;
  }

  Behavior* Factory::o_Behavior( BehaviorID const& name )
  {
    Behavior* b;
    if(m_behaviors.find(name)==m_behaviors.end())
    {
      b = new Behavior(name);
      m_behaviors[name] = b;

      #ifdef ANT_TWEAK_ENABLED
        std::stringstream properties;
        properties << "group=AOBehaviors label='" << name.c_str() << "'";
        ANT__Bar_AddButton_P(bar, name.c_str(), ANT__AddRemObjBehavior, b, properties.str().c_str());
      #endif

    }
    else
      b = m_behaviors[name];

    return b;
  }


  //////////////////////////////////////////////////////////////////////////
  //// INTERNAL USE METHODS
  //////////////////////////////////////////////////////////////////////////
  void Factory::defineComponent( Text const& name, TypeID const& id, componentCreator cmpCreationFn )
  {
    ErrorIf(m_createByName.find(name)!=m_createByName.end(), "Duplicate component creator definitions provided!\n (%s)\n",name.c_str());
    m_createByName[name] = cmpCreationFn;
    m_createByID[id] = cmpCreationFn;
  }

  void Factory::ConstructTemplates( void )
  {
    
    m_loadingArchetypes = false;
    loadObjectLibrary();
    return;

//     static bool started = false;
//     static TypeParser t;
//     if(!started) t.parseFile(OBJECT_ARCHTYPE_FILE);
// 
//     static Scope::ScopeList& objs = t.fileScope.children;
//     static Scope::ScopeList::iterator it = objs.begin();
// 
//     while(it!=objs.end())
//     {
//       Object* obj = new Object(0, OBJECT_ARCHTYPE_NAME, (*it)->name);
//       obj->serialize(**it, true);
//       ErrorIf(m_archetypes.find(obj->m_archetype)!=m_archetypes.end(), "Factory encountered an error:\nDuplicate object archetype definitions (%s) in %s.", obj->m_archetype.c_str(), OBJECT_ARCHTYPE_FILE);
//       m_archetypes[obj->m_archetype] = obj;
// 
//       //archetypes should not execute their behaviors
//       gCore->Events.UnsubscribeForever(obj);
// 
//       //break each time to allow a game loop to complete.
//       ++it;
//       break;
//     }
// 
//     //loading complete, next step
//     if(it==objs.end())
//     {
//       m_loadingArchetypes = false;
//       loadObjectLibrary();
//     }
// 
//       started = true;
// //     dumpObjects();
  }

  void Factory::CleanupDeletedObjects( void )
  {
    ObjectDeleteList::iterator it = m_deletedObjects.begin();
    for(;it!=m_deletedObjects.end(); ++it)
    {
      ErrorIf(m_objects.find(*it)==m_objects.end(), "Factory attempting to delete an object which does not exist. Someone must have called o_Delete() directly. Only and object should use that.");
      delete m_objects[*it];
      m_objects.erase(*it);
    }
    m_deletedObjects.clear();
  }

  void Factory::recompileBehavior( Text const& file )
  {
    BehaviorMap::iterator bIt = m_behaviors.find(file);
    //new behavior, just create it
    if(bIt==m_behaviors.end())
    {
      Behavior* b = new Behavior(file);
      m_behaviors[file] = b;

      #ifdef ANT_TWEAK_ENABLED
        std::stringstream properties;
        properties << "group=AOBehaviors label='" << file.c_str() << "'";
        ANT__Bar_AddButton_P(bar, file.c_str(), ANT__AddRemObjBehavior, b, properties.str().c_str());
      #endif
    }
    else //existing behavior
    {
      Behavior*& behavior = bIt->second;
      std::list<Object*> affectedObj;

      //strip all behaviors associate with this file from archetypes
      ArchetypeMap::iterator aIt = m_archetypes.begin();
      for(; aIt != m_archetypes.end(); ++aIt)
      {
        Object *& object = aIt->second;
        //copy paste from object->removeBehavior, but we need to track this
        //with affectedObj.push_back... *shrug*
        Object::BehaviorMap::iterator oBehIt = object->m_behaviors.find(file);
        if(oBehIt != object->m_behaviors.end())
        {
          object->invoke_detach(behavior);
          object->ignoreFor(behavior);
          object->m_behaviors.erase(oBehIt);
          affectedObj.push_back(object);
        }
      }

      //strip all behaviors associated with this file from object
      ObjectMap::iterator oIt = m_objects.begin();
      for(; oIt != m_objects.end(); ++oIt)
      {
        Object *& object = oIt->second;
        //copy paste from object->removeBehavior, but we need to track this
        //with affectedObj.push_back... *shrug*
        Object::BehaviorMap::iterator oBehIt = object->m_behaviors.find(file);
        if(oBehIt != object->m_behaviors.end())
        {
          object->invoke_detach(behavior);
          object->ignoreFor(behavior);
          object->m_behaviors.erase(oBehIt);
          affectedObj.push_back(object);
        }
      }


      //then recompile (which invalidates all existing behavior pointers
      //which is why we stripped them first
      behavior->recompile();

      //re-add the behavior to the objects
      while(affectedObj.size())
      {
        affectedObj.front()->addBehavior(behavior);
        affectedObj.pop_front();
      }
    }
  }

  void Factory::preloadLevels( void )
  {
    static const Text levelFile("_levels");
    static const Text levelFilePath(CONFIG_PATH_LEVELS + levelFile);
    m_curLevel = NO_LEVEL_LOADED;

    FileStream fs(levelFilePath);
    u32 counter = 0;
    while(fs.hasData())
    {
      m_levels.push_back(new Level(fs.read<Text>()));
      m_levelIndicies[m_levels.back()->m_name] = counter;
      ++counter;
    }
    if(m_levels.size())
    {
      m_levels[0]->unload();
      m_levels[0]->load();
      m_curLevel = 0;
      for(size_t i =1; i < m_levels.size(); ++i)
        m_levels[i]->unload();
    }
    else
      ANT__Bar_AddButton_P(bar, "<no levels loaded>", NULL, NULL, " group=Levels");

  }

  void Factory::refreshBehaviors( void )
  {
      BehaviorMap::iterator it = m_behaviors.begin();
    for(; it!=m_behaviors.end(); ++it)
      recompileBehavior(it->first);
  }

  void Factory::loadLevel( Text const& name )
  {
    ErrorIf(m_curLevel!=NO_LEVEL_LOADED, "attempt to load level while another level was loaded.");
    ErrorIf(m_levelIndicies.find(name)==m_levelIndicies.end(), "No such level indicie could be found for level '%s'\n", name.c_str());
    m_curLevel = m_levelIndicies.find(name)->second;
    m_levels[m_curLevel]->load();
  }

  void Factory::unloadLevel( void )
  {
    ErrorIf(m_curLevel==NO_LEVEL_LOADED, "attempt to unload level with no level loaded");
    m_deletedObjects.clear();
    ANT__Bar_Remove(bar, m_levels[m_curLevel]->m_name.c_str());
    m_levels[m_curLevel]->unload();
    while(m_objects.size())
    {
      m_objects.begin()->second->m_silentDestroy = true;
      delete m_objects.begin()->second;
      m_objects.erase(m_objects.begin());
    }
    while(m_archetypes.size())
    {
      m_archetypes.begin()->second->m_silentDestroy = true;
      delete m_archetypes.begin()->second;
      m_archetypes.erase(m_archetypes.begin());
    }
    while(m_behaviors.size())
    {
      delete m_behaviors.begin()->second;
      m_behaviors.erase(m_behaviors.begin());
    }

    m_curLevel = NO_LEVEL_LOADED;
    ErrorIf(m_objects.size(), "Factory failed to deallocate all objects during level unload. (%d objects still in memory)", m_objects.size());
  }

  void Factory::switchLevel( Text const& name )
  {
     if(m_curLevel!=NO_LEVEL_LOADED)
      unloadLevel();
    loadLevel(name);
  }


  void Factory::vRegisterComponent( TypeID id, Component* component )
  {
    printDebug("Trying to register component Factory could care less about...");
  }

  void Factory::vUnregisterComponent( TypeID id, Component* component )
  {

    printDebug("Trying to unregister component Factory shouldn't know about...");
  }

  
#ifdef ANT_TWEAK_ENABLED
  void Factory::setActiveAntObject( Object* obj )
  {
    activeAntObject = obj;
  }
#endif

  void Factory::saveStateAsLevel( Text const& LevelName )
  {
    Text name;
    if(LevelName=="")
    {
      std::stringstream levelSS;
      levelSS << "level" << m_levels.size();
      name = levelSS.str();
    }
    else
      name = LevelName;

    
    static const Text levelListFile(Text(CONFIG_PATH_LEVELS) + "_levels");
    const Text levelFilename(CONFIG_PATH_LEVELS + name);
    static const Text lsBaseFile(Text(CONFIG_PATH_LEVELS) + "_lsBase");

    //Create 'levelX' file (where x is the index)
    std::ofstream newLevelFS;
    newLevelFS.open(levelFilename);
    //write each object
    for(ObjectMap::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
    {
      Body* body = it->second->getComponentT<Body>();
      ErrorIf(!body, "All game object have to have Body cmp. Can't save the level...");

      newLevelFS << it->second->getArchetype() << "\n";
      newLevelFS << "{\n";
      newLevelFS << "  Text:instance=" << it->second->m_instanceName << "\n";
      newLevelFS << "  vec3:position=" << body->m_current.m_position << "\n";
      newLevelFS << "  vec3:scale=" << body->m_current.m_xSize << "," << body->m_current.m_ySize << "," << body->m_current.m_zSize << "\n";
      newLevelFS << "  Quat:orient=" << body->m_current.m_orientation << "\n";

      if(it->second->m_behaviors.size())
      {
        newLevelFS << "  behaviors|Text\n";
        newLevelFS << "  {\n";
        for(Object::BehaviorMap::iterator bit = it->second->m_behaviors.begin(); bit != it->second->m_behaviors.end(); ++bit)
          newLevelFS << "    " << bit->first << "\n";
        newLevelFS << "  }\n";
      }
      newLevelFS << "}\n";
    }

    newLevelFS.close();

    //write level script file
    std::ofstream newLSFS(levelFilename + ".nut");
    std::ifstream lsTemplateFS(lsBaseFile);
    Text t;
    while(lsTemplateFS.good())
    {
      ch c;
      t.clear();
      do 
      { 
        c = lsTemplateFS.get();
        if(c=='\n' || c==EOF) break;
        t += c;
      } while (lsTemplateFS.good());

      newLSFS << t << "\n";
    }
    lsTemplateFS.close();
    newLSFS.close();
    

    //append the new level to the _levels file
    std::ofstream levelListFS(levelListFile, std::ofstream::app);
    levelListFS << name << "\n";
    levelListFS.close();

    //add the new level to the current factory levels available
    m_levels.push_back(new Level(name));
    m_levelIndicies[m_levels.back()->m_name] = m_levels.size()-1;
    m_levels.back()->unload();
  }
  void Factory::saveLevelAs(Text const& name)
  {
    saveStateAsLevel(name);
  }

  Object* Factory::allocateAndSerializeObject( Object const& source )
  {
    Text const& si = source.m_instanceName;
    Text const& sa = source.m_archetype;
    ++m_objectIDs;
    std::stringstream ss;
    ss << (si==OBJECT_ARCHTYPE_NAME?sa:si) << "_cpy" << m_objectIDs;
    Object* newObj = new Object(m_objectIDs, ss.str(), sa);
    newObj->serialize(source);
    m_objects[newObj->m_id] = newObj;
    return newObj;
  }

  void Factory::invokeCreateForObject( Object & obj )
  {    
    //Invoke script events for object
    obj.invoke_create();
    //invoke add component for all components
    for(Object::ComponentMap::iterator cit=obj.m_components.begin(); cit!=obj.m_components.end(); ++cit)
      obj.invoke_addComponent(cit->second->getTypeName());
    //invoke attach for all behaviors
    for(Object::BehaviorMap::iterator bit=obj.m_behaviors.begin(); bit!=obj.m_behaviors.end(); ++bit)
      obj.invoke_attach(bit->second);
  }

  void Factory::loadObjectLibrary( void )
  {
    //DEFAULT LEVEL
    switchLevel("menu");
    //switchLevel("a3_death");
  }

  //performs object archetype loading on demand. The name of the object is
  //expected to be the same as the filename.gof
  bool Factory::archetypeOnDemand( Text const& name )
  {
    if(m_archetypes.find(name)!=m_archetypes.end())
      return true;

    TypeParser t;
    bool fileFound = t.parseFile(OBJECT_ARCHETYPE_FILE_DIR + name + ".gof");
    ErrorIf(!fileFound, "Factory::archetypeOnDemand failed.\n No file found for \"%s\" archetype.\n\nEnsure the specified archetype is located in \"bin\\%s\\\" and named \"%s.gof\"", name.c_str(), OBJECT_ARCHETYPE_FILE_DIR, name.c_str());
    if(!fileFound) return false;

    Scope::ScopeList& objs = t.fileScope.children;
    Scope::ScopeList::iterator it = objs.begin();
    ErrorIf(objs.empty(), "Factory::archetypeOnDemand failed.\n No objects were defined in the specifed file \"%s\\%s.gof\" ", OBJECT_ARCHETYPE_FILE_DIR, name.c_str());
    ErrorIf(objs.size()!=1, "Factory::archetypeOnDemand failed.\n There was more than one object defined in the specified file.\nDir: %s\nFile: %s\nObject Count: %d\n", OBJECT_ARCHETYPE_FILE_DIR, name.c_str(), objs.size());
    while(it!=objs.end())
    {
      //only load the object requested...
      if((*it)->name == name)
      {
        Object* obj = new Object(0, OBJECT_ARCHTYPE_NAME, (*it)->name);
        obj->serialize(**it, true);
        m_archetypes[obj->m_archetype] = obj;
        //archetypes should not execute their behaviors
        gCore->Events.UnsubscribeForever(obj);
        return true;
      }
        ++it;
    }
    return false;
  }



//   //creates a single object instance callable from script. It is a special function for art testing.
//   //This function does NOT require object template file updates, it can compile on demand object archetypes
//   //provided the archetype is listed in a file in the assets folder named to match the object name.
//   //for example, object type "GoblinMaster" would be defined in a file in "assets/GoblinMaster.gof"
//   //this file would define an object EXACTLY as you would in object_templates.gof
//   Object* Factory::scriptCreateObject( Text const& name, Text const& instance)
//   {
//     Object* o = FACTORY->createObject(name);
//     ErrorIf(o==NULL, "scriptCreateObject: Failed to create object of type %s\n", name.c_str() );
// 
//     o->ANT_reset();
//     //rename the instance
// 
//     if(!instance.empty())
//       o->m_instanceName = instance;
// 
//     o->ANT_setup();
//     Object::ComponentMap::iterator ocit = o->m_components.begin();
//     for(;ocit!=o->m_components.end(); ++ocit)
//       o->ANT_AddCmp(ocit->second);
// 
//     gCore->Events.UnsubscribeFromAllNow(o);
//     o->m_behaviors.clear();
//     o->m_eventListeners.clear();
// 
//     return o;
//   }

  void Factory::buildModelViewLevel( Text const& file, bool writeGMDL )
  {
//     if(GMDL_record)
//     {
//       GMDL_modelList.push_back(file);
//       return;
//     }
//     if(writeGMDL && GMDL_modelList.empty())
//       return;
// 
//     size_t delimit1 = file.find_first_of('_');
//     size_t delimit2 = file.find_last_of('_');
//     size_t delimit3 = file.find_first_of('.');
// 
//     ErrorIf(delimit1 == Text::npos, "ERROR: File name improperly formatted. < %s >\n\n Expected: 'Model_inst_type.bin'\n\n'Model' is the object's name\n'inst' is any identifier, typically the animation name, or the number 1 for static models.\n'type' is one of:\n    simple - no bones, no normals\n    static - no animations\n    skinned - animated, bones, etc\n\n", file.c_str()); 
//     ErrorIf(delimit2 == Text::npos, "ERROR: File name improperly formatted. < %s >\n\n Expected: 'Model_inst_type.bin'\n\n'Model' is the object's name\n'inst' is any identifier, typically the animation name, or the number 1 for static models.\n'type' is one of:\n    simple - no bones, no normals\n    static - no animations\n    skinned - animated, bones, etc\n\n", file.c_str()); 
//     ErrorIf(delimit3 == Text::npos, "ERROR: File name improperly formatted. < %s >\n\n Expected: 'Model_inst_type.bin'\n\n'Model' is the object's name\n'inst' is any identifier, typically the animation name, or the number 1 for static models.\n'type' is one of:\n    simple - no bones, no normals\n    static - no animations\n    skinned - animated, bones, etc\n\n", file.c_str()); 
//     ErrorIf(delimit1 == delimit2, "ERROR: File name improperly formatted. < %s >\n\n Expected: 'Model_inst_type.bin'\n\n'Model' is the object's name\n'inst' is any identifier, typically the animation name, or the number 1 for static models.\n'type' is one of:\n    simple - no bones, no normals\n    static - no animations\n    skinned - animated, bones, etc\n\n", file.c_str()); 
//     ErrorIf(delimit1 == delimit3, "ERROR: File name improperly formatted. < %s >\n\n Expected: 'Model_inst_type.bin'\n\n'Model' is the object's name\n'inst' is any identifier, typically the animation name, or the number 1 for static models.\n'type' is one of:\n    simple - no bones, no normals\n    static - no animations\n    skinned - animated, bones, etc\n\n", file.c_str()); 
//     ErrorIf(delimit2 == delimit3, "ERROR: File name improperly formatted. < %s >\n\n Expected: 'Model_inst_type.bin'\n\n'Model' is the object's name\n'inst' is any identifier, typically the animation name, or the number 1 for static models.\n'type' is one of:\n    simple - no bones, no normals\n    static - no animations\n    skinned - animated, bones, etc\n\n", file.c_str()); 
//     
//     Text modelName = file.substr(0, delimit1);
//     Text modelInst = file.substr(delimit1, delimit2);
//     Text modelType = file.substr(delimit2, delimit3);
//     Text modelExt = file.substr(delimit3);
//     
//     //******
//     //Search assets/texture/ for objects matching modelName_?.png
//     //******
//     Text has_a = "false";
//     Text has_d = "false";
//     Text has_e = "false";
//     Text has_n = "false";
//     Text has_s = "false";
//     {//Alpha exists?
//       std::ifstream ifile(CONFIG_PATH_TEXTURE + modelName + "_a.png");
//       if(ifile.is_open())
//         has_a = "true";
//     }{//Diffuse exists?
//       std::ifstream ifile(CONFIG_PATH_TEXTURE + modelName + "_d.png");
//       if(ifile.is_open())
//         has_d = "true";
//     }{//Emissive exists?
//       std::ifstream ifile(CONFIG_PATH_TEXTURE + modelName + "_e.png");
//       if(ifile.is_open())
//         has_e = "true";
//     }{//normal exists?
//       std::ifstream ifile(CONFIG_PATH_TEXTURE + modelName + "_n.png");
//       if(ifile.is_open())
//         has_n = "true";
//     }{//spec exists?
//       std::ifstream ifile(CONFIG_PATH_TEXTURE + modelName + "_s.png");
//       if(ifile.is_open())
//         has_s = "true";
//     }
// 
// 
//     //  write archetype:  objects/ModelName.gof
//     std::ofstream os;
//     //identify type, assume not skinned
//     Text filename = file;
//     Text has_multiAnim = "false";
//     if(writeGMDL)
//     {
//       filename = modelName + "__pack.gmdl";
//       has_multiAnim = "true";
// 
//       //write the GMDL file
//       os.open(CONFIG_PATH_MESH + filename, std::ofstream::trunc | std::ofstream::out);
//       ErrorIf(!os.is_open() || !os.good(), "ERROR WRITING %s, the GMDL file for %s model.", (CONFIG_PATH_MESH + filename).c_str(), modelName.c_str());
//       while(GMDL_modelList.size())
//       {
//         os << GMDL_modelList.front() << "\n";
//         GMDL_modelList.pop_front();
//       }
//       os.close();
//     }
// 
//     os.open(CONFIG_PATH_OBJECTS + modelName + ".gof", std::ofstream::trunc | std::ofstream::out);
//     ErrorIf(!os.is_open() || !os.good(), "ERROR WRITING %s, the object file for %s model.", (CONFIG_PATH_OBJECTS + modelName + ".gof").c_str(), modelName.c_str());
//     os << modelName                                 << "\n";
//     os << "{"                                       << "\n";
//     os << "  gfxModel"                              << "\n";
//     os << "  {"                                     << "\n";
//     os << "    u1:hasmultanims=" << has_multiAnim   << "\n";
//     os << "    Text:modelfile=" << filename         << "\n";
//     os << "    u1:hasd=" << has_d                   << "\n";
//     os << "    u1:hasn=" << has_n                   << "\n";
//     os << "    u1:hase=" << has_e                   << "\n";
//     os << "    u1:hass=" << has_s                   << "\n";
//     os << "    u1:hasa=" << has_a                   << "\n";
//     os << "    vec3:scale=10,10,10"                 << "\n";
//     os << "    vec3:startDir=0,0,1"                 << "\n";
//     os << "  }"                                     << "\n";
//     os << "  Transform"                             << "\n";
//     os << "  {}"                                    << "\n";
//     os << "  Body"                                  << "\n";
//     os << "  {"                                     << "\n";
//     os << "     u1:motionEnabled=true"              << "\n";
//     os << "     u1:collisionEnabled=true"           << "\n";
//     os << "     NarrowPhaseType:narrowType=Obb"     << "\n";
//     os << "     CollisionType:coltype=Boss"         << "\n";
//     os << "     current"                            << "\n";
//     os << "     {"                                  << "\n";
//     os << "       vec3:scale=1,1,1"                 << "\n";
//     os << "       f32:mass=0"                       << "\n";
//     os << "       f32:invMass=0"                    << "\n";
//     os << "       f32:inerTensor=0"                 << "\n";
//     os << "       f32:invInerTensor=0"              << "\n";
//     os << "       f32:static=0"                     << "\n";
//     os << "       f32:drag=0"                       << "\n";
//     os << "       vec3:position=0,0,0"              << "\n";
//     os << "       vec3:velocity=0,0,0"              << "\n";
//     os << "       vec3:momentum=0,0,0"              << "\n";
//     os << "       vec3:deltaPos=0,0,0"              << "\n";
//     os << "       vec3:angMomentum=0,0,0"           << "\n";
//     os << "       vec3:angVelocity=0,0,0"           << "\n";
//     os << "       Quat:orientation=1,0.5,0.5,0"     << "\n";
//     os << "       Quat:spin=0,0,0,0"                << "\n";
//     os << "    }"                                   << "\n";
//     os << "  }"                                     << "\n";
//     os << "}"                                       << "\n";
//     os.close();
// 
// 
// 
//     //  write level:  levels/ModelName
//     os.open(CONFIG_PATH_LEVELS + modelName, std::ofstream::trunc | std::ofstream::out);
//     ErrorIf(!os.is_open() || !os.good(), "ERROR WRITING %s, the level file for %s model.", (CONFIG_PATH_LEVELS + modelName).c_str(), modelName.c_str());
//     os << modelName                                  << "\n";
//     os << "{"                                        << "\n";
//     os << "  Text:instance=" << modelName << "_test" << "\n";
//     os << "  vec3:position=0,0,0"                    << "\n";
//     os << "  vec3:scale=1,1,1"                       << "\n";
//     os << "  Quat:orient=0.4164,0.5082,0.6088,0"     << "\n";
//     os << "}"                                        << "\n";
//     os.close();
// 
//     //  write level script:  levels/ModelName.nut
//     os.open(CONFIG_PATH_LEVELS + modelName + ".nut", std::ofstream::trunc | std::ofstream::out);
//     ErrorIf(!os.is_open() || !os.good(), "ERROR WRITING %s, the level script file for %s model.", (CONFIG_PATH_LEVELS + modelName + ".nut").c_str(), modelName.c_str());
//     os << "\n";
//     os.close();
// 
//     //add the level to the runtime list of levels
//     m_levels.push_back(new Level(modelName));
//     m_levelIndicies[m_levels.back()->m_name] = m_levels.size()-1;
//     m_levels.back()->unload();
// 
//     USERINTERFACE->openBaseMenu("");
//     switchLevel(modelName);

  }

  Object* Factory::getObjectByID( ObjectID id )
  {
    ObjectMap::iterator it = m_objects.find(id);
    if(it!=m_objects.end())
      return it->second;
    else
      return NULL;
  }


  void Factory::updateAttachedObjects( void )
  {
    ObjectMap::iterator begin = m_objects.begin();
    ObjectMap::iterator end = m_objects.end();

    //This list is guaranteed to only contain up to date links.
    for(; begin != end; ++begin)
    {
      Object& obj = *begin->second;

      //If this object is attached to anything, we should update it's position to be the vector specified.
      if(obj.parentLink.link)
      {
        obj.position = *obj.parentLink.pos;
      }
    }
  }

  void Factory::attachObjectToBone( Object& obj, Text const& boneName, bool bound )
  {
    BoneBinding bind = GRAPHICS->getBoneBinding(boneName);

    //If we found a bone to attach to (meaning the object holding it as well)
    if(bind.link)
    {
      //If we're binding, set that flag to true, otherwise, we're attaching
      if(bound)
        bind.bound = true;

      //We're attaching the object to the other, so this is it's one attachment.
      obj.linkToParent(bind);

      //Also, make sure the parent knows it has an object attached to it.
      bind.link->attachObjectToMe(&obj);
    }
  }

  void Factory::updateHUD()
  {
    updatePlayerHUD();
  }

  void Factory::updateBossHUD()
  {
    return;
    static DX9Texture* bossHealthBottom   = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "ui/hud_bossbar_0.dds");
    static DX9Texture* bossHealthBar      = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "ui/hud_bossbar_1.dds");
    static DX9Texture* bossHealthBarTop   = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "ui/hud_bossbar_2.dds");
    static DX9Texture* bossHealthBarShine = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "ui/hud_bossbar_3.dds");

    static UIElement healthElem;
    static vec2 firstPos(0.75f,0.25f);
    static vec2 shiftPos;
    static vec2 dimensions(0.5f,0.5f);
    static f32 healthUnitPos = 0.003f;

    Object* boss = SCRIPTGLOBALS->getBoss();

    if(boss)
    {
      Data* bData = boss->getComponentT<Data>();

      if(bData)
      {
        static s32 startHealth = bData->gets32("health");

        s32 health = bData->gets32("health");

        //Draw the health based on how much health you have remaining.
        healthElem.tex = bossHealthBottom;
        healthElem.pos = firstPos;
        healthElem.dim = dimensions;
        healthElem.type = HUD;

        GRAPHICS->mUISystem->AddUIElement(healthElem);

        //Shift the health bar itself based on how much percentage health is lost
        shiftPos = firstPos;

        f32 healthDiff = (1.0f - (SCAST(f32, health) / startHealth)) * 100.0f;

        shiftPos.x += healthDiff * healthUnitPos;

        healthElem.pos = shiftPos;
        healthElem.tex = bossHealthBar;
        GRAPHICS->mUISystem->AddUIElement(healthElem);

        healthElem.pos = firstPos;
        healthElem.tex = bossHealthBarTop;
        GRAPHICS->mUISystem->AddUIElement(healthElem);

        healthElem.tex = bossHealthBarShine;
        GRAPHICS->mUISystem->AddUIElement(healthElem);
      }
    }
  }

  void Factory::updatePlayerHUD()
  {
    static DX9Texture* playerHealthBottom   = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "ui/hud_playerbar_0.png");
    static DX9Texture* playerHealthBar      = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "ui/hud_playerbar_1.png");
    static DX9Texture* playerHealthBarTop   = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "ui/hud_playerbar_2.png");

    static UIElement healthElem;
    static vec2 firstPos(0.135f,0.035f);
    static vec2 shiftPos;
    static vec2 dimensions(0.30f,0.085f);
    static f32 healthUnitPos = 0.0028f;


    Object* player = SCRIPTGLOBALS->getPlayer1();

    if(player)
    {
      Data* pData = player->getComponentT<Data>();

      if(pData)
      {
        static s32 startHealth = pData->gets32("health");

        s32 health = pData->gets32("health");

        //Draw the health based on how much health you have remaining.
        healthElem.tex = playerHealthBottom;
        healthElem.pos = firstPos;
        healthElem.dim = dimensions;
        healthElem.type = HUD;

        GRAPHICS->mUISystem->AddUIElement(healthElem);

        //Shift the health bar itself based on how much percentage health is lost
        shiftPos = firstPos;
        f32 healthDiff = (1.0f - (SCAST(f32, health) / startHealth)) * 100.0f;

        shiftPos.x -= healthDiff * healthUnitPos;
        
        healthElem.pos = shiftPos;
        healthElem.tex = playerHealthBar;
        GRAPHICS->mUISystem->AddUIElement(healthElem);

        healthElem.pos = firstPos;
        healthElem.tex = playerHealthBarTop;
        GRAPHICS->mUISystem->AddUIElement(healthElem);

        GRAPHICS->mUISystem->AddUIElement(healthElem);
      }
    }
  }

  Grimelins::ScriptVec3 Factory::getBonePos(Text const& boneName ) const
  {
    return *(GRAPHICS->getBoneBinding(boneName).pos);
  }



}//namespace Virtus
