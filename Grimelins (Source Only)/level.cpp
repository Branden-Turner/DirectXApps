/*! ====================================================================================================================
  @file level.cpp

  Codename Grimelins

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date Nov.15.2011

  @brief 
  Level data storage class. When a level is loaded the files and scripts that it needs are invoked through this.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "level.h"
#include "factory.h"
#include "object.h"
#include "compbody.h"

namespace Grimelins
{
  ANT__RETVAL ANT__FacLoadLevel( void* vlevel)
  {
    Level* l = SCAST(Level*, vlevel);
    FACTORY->unloadLevel();
    FACTORY->loadLevel(l->m_name);
  }


  Level::Level( Text const& name )
    : m_name(name), m_pathfile(CONFIG_PATH_LEVELS), m_script(NULL), m_load(NULL), m_unload(NULL), m_loaded(false)
  {
    m_pathfile += m_name;
    loadScript();
  }

  Level::~Level( void )
  {
    if(m_script) delete m_script;
    if(m_load) delete m_load;
    if(m_unload) delete m_unload;
  }

  //assumes factory unloaded last level
  void Level::load( void )
  {
    ErrorIf(m_loaded, "Attempting to load a level while it is loaded!\n Level: %s\n", m_name.c_str());
    loadObjects();
    ANT__Bar_Remove(ANT__GetBarP(bar,FACTORY), m_name.c_str());//Remove Level button from factory levels list
    ANT__Bar_AddButton_P(ANT__GetBarP(bar,FACTORY), m_name.c_str(), NULL, NULL, " group=Levels");//add "green" button in factory levels list
    gCore->GameRunning = true;
    gCore->GamePaused = false;
    if(m_load) 
      m_load->Execute();
    m_loaded=true;

  }

  //assumes factory destroys all the objects...
  void Level::unload( void )
  {
    ANT__Bar_AddButton_P(ANT__GetBarP(bar,FACTORY), m_name.c_str(), ANT__FacLoadLevel, this, " group=Levels");//readd it as a clickable button

    if(m_loaded && m_unload) m_unload->Execute();
    m_loaded=false;
  }

  void Level::loadScript( void )
  {
    ErrorIf(m_load||m_unload, "Level attempted to compile its script twice.\nLevel: %s\n", m_name.c_str());
    m_script = new Sqrat::Script(gCore->Scripter);
    ErrorIf(!m_script, "Failed to allocate memory in Level::LoadScript for script object.");

    Text filepath = m_pathfile + ".nut";
    
    try { m_script->CompileFile(filepath); }
    catch (Sqrat::Exception e) { printDebug("Error compiling file '%s' in squirrel virtual machine for level %s.\n", m_name.c_str()); return;}

    ErrorIf(!m_script->IsCompiled(), "Script Failed to compile '%s'\n\nCheck the filepath and filename.\n If you are sure it's right, check Squirrel syntax in the file.\n", m_name.c_str());    
    if(!m_script->IsCompiled())
    {
      delete m_script;
      m_script = NULL;
      return;
    }

    try { m_script->Run(); }
    catch (Sqrat::Exception e) { printDebug("Error in squirrel virtual machine running script '%s' for Level script.\n", m_name.c_str()); return;}


    Sqrat::Function* fn = new Sqrat::Function(Sqrat::RootTable(gCore->Scripter).GetSlot("level"), "load");
    ErrorIf(!fn, "Failed to allocate memory in Level::LoadScript for script function object.");
    if(!fn->IsNull())
      m_load = fn;
    else
      delete fn;
    
    fn = new Sqrat::Function(Sqrat::RootTable(gCore->Scripter).GetSlot("level"), "unload");
    ErrorIf(!fn, "Failed to allocate memory in Level::LoadScript for script function object.");
    if(!fn->IsNull())
      m_unload = fn;
    else
      delete fn;

    gCore->Scripter.runString("level.clear();");

  }

  void Level::loadObjects( void )
  {
    TypeParser t;
    t.parseFile(m_pathfile);
    Scope::ScopeList& objs = t.fileScope.children;
    Scope::ScopeList::iterator it = objs.begin();

    for(u32 objNum = 1;it!=objs.end(); ++it, ++objNum)
    {
      //**it.name is the archetype to create
      //**it is the scope of that object
      Object* o = FACTORY->partialCreateObject((*it)->name);
      ErrorIf(o==NULL, "LEVEL LOAD: Failed to create object of type %s\n",(*it)->name.c_str() );

      Body* b = o->getComponentT<Body>();
      ErrorIf(b==NULL, "LEVEL LOAD: Failed to obtain a body from the object of type %s\n",(*it)->name.c_str());

      ErrorIf((*it)->Text_.empty(), "LEVEL LOAD:  Level %s expected 'Text' parameter 'instance' for archetype %s.\n", m_name.c_str(), o->m_archetype.c_str());
      ErrorIf((*it)->Text_.front().name != "instance", "LEVEL LOAD:  Level %s expected 'instance', instead found '%s'.\n", m_name.c_str(),(*it)->Text_.front().name.c_str());

      o->ANT_reset();
      //rename the instance
      o->m_instanceName = (*it)->Text_.front().data;
      (*it)->Text_.pop_front();
      o->ANT_setup();
      Object::ComponentMap::iterator ocit = o->m_components.begin();
      for(;ocit!=o->m_components.end(); ++ocit)
        o->ANT_AddCmp(ocit->second);

      ErrorIf((*it)->vec3_.empty(), "LEVEL LOAD:  Level %s expected 'vec3' parameter 'position' for instance %s.\n", m_name.c_str(), o->m_instanceName.c_str());
      ErrorIf((*it)->vec3_.front().name != "position", "LEVEL LOAD:  Level %s expected 'position', instead found '%s' for instance %s.\n", m_name.c_str(),(*it)->vec3_.front().name.c_str(), o->m_instanceName.c_str());
      o->m_position = b->m_current.m_position = (*it)->vec3_.front().data;
      (*it)->vec3_.pop_front();

      ErrorIf((*it)->vec3_.empty(), "LEVEL LOAD:  Level %s expected 'vec3' parameter 'scale' for instance %s.\n", m_name.c_str(), o->m_instanceName.c_str());
      ErrorIf((*it)->vec3_.front().name != "scale", "LEVEL LOAD:  Level %s expected 'scale', instead found '%s' for instance %s.\n", m_name.c_str(),(*it)->vec3_.front().name.c_str(), o->m_instanceName.c_str());
      vec3& scale = (*it)->vec3_.front().data;
      o->m_scale.x = b->m_current.m_xSize = scale.x;
      o->m_scale.y = b->m_current.m_ySize = scale.y;
      o->m_scale.z = b->m_current.m_zSize = scale.z;
      b->m_current.m_size = (scale.x + scale.y + scale.z) /3.0f;
      b->createBoundingVolumes();
      (*it)->vec3_.pop_front();
      

      ErrorIf((*it)->Quat_.empty(), "LEVEL LOAD:  Level %s expected 'quat' parameter 'orient' for instance %s.\n", m_name.c_str(), o->m_instanceName.c_str());
      ErrorIf((*it)->Quat_.front().name != "orient", "LEVEL LOAD:  Level %s expected 'orient', instead found '%s' for instance %s.\n", m_name.c_str(),(*it)->Quat_.front().name.c_str(), o->m_instanceName.c_str());
      o->m_rotation = b->m_current.m_orientation = (*it)->Quat_.front().data;
      
      (*it)->Quat_.pop_front();

//       gCore->Events.UnsubscribeFromAllNow(o);
//       o->m_behaviors.clear();
//       o->m_eventListeners.clear();

      //for each behavior listed
      if((*it)->TextList_.size() && (*it)->TextList_.front().name=="behaviors")
      {
        std::list<Text>& behaviors = (*it)->TextList_.front().data;
        std::list<Text>::iterator bIt = behaviors.begin();
        for(;bIt!=behaviors.end(); ++bIt)
        {
          Behavior* behv = FACTORY->o_Behavior(*bIt);
          o->addBehavior(behv);
        }
      }

      //invoke creation and attach script functions
      o->invoke_create();
      o->invoke_attachAll();
    }
  }

}; // namespace Grimelins