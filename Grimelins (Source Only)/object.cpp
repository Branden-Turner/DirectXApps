/*! ====================================================================================================================
  @file object.cpp

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.16.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "object.h"
#include "component.h"
#include "factory.h"
#include "comptransform.h"
#include "compbody.h"
#include "cmpcontroller.h"
#include "cmpgfx.h"
#include "cmpplayer.h"
#include "cmpproj.h"
#include "cmpData.h"

namespace Grimelins
{

  SCRIPT_ClassStart(Object);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(getInstanceName);
  SCRIPT_Method(getArchetype);
  SCRIPT_Method(getID);
  SCRIPT_Method(destroy);
  SCRIPT_Method(clone);
  SCRIPT_Method(removeAllBehaviors);
  SCRIPT_MethodOverload(scriptAddBehavior, "addBehavior", void, (Text const& name));
  SCRIPT_MethodOverload(scriptRemoveBehavior, "removeBehavior", void, (Text const& name));
  SCRIPT_Method(hasBehavior);
  SCRIPT_Method(setPosition);
  SCRIPT_Method(setRotation);
  SCRIPT_Method(setScale);
  SCRIPT_Method(getPosition);
  SCRIPT_Method(getRotation);
  SCRIPT_Method(getScale);
  SCRIPT_Method(scriptCall);
  SCRIPT_Method(scriptCallSpecial);
  SCRIPT_Method(REQ_CMP);
  SCRIPT_Method(REQ_VAL);

  SCRIPT_ClassEnd();

  ANT__RETVAL ANT__ObjDeleteThis(void* obj)
  {
#ifdef ANT_TWEAK_ENABLED
    SCAST(Object*, obj)->destroy();
#endif
  }

  ANT__RETVAL ANT__ObjCopyThis(void* obj)
  {
#ifdef ANT_TWEAK_ENABLED
    SCAST(Object*, obj)->clone();
#endif
  }

  ANT__RETVAL ANT__ObjSetActive(void* obj)
  {
#ifdef ANT_TWEAK_ENABLED
    Object* o = SCAST(Object*, obj);
    if(gFactory->activeAntObject == o)
    {
      gFactory->activeAntObject = NULL;
      ANT__Bar_Remove(ANT__GetBarP(bar, gFactory), "AO_object");
      ANT__Bar_AddButton_P(ANT__GetBarP(bar, gFactory), "AO_object", NULL, NULL, "group=AOMain label='<None Set>'");//Active Object button
    }
    else
    {
      gFactory->activeAntObject = o;
      ANT__Bar_Remove(ANT__GetBarP(bar, gFactory), "AO_object");
      std::stringstream properties;
      properties << "group=AOMain label='" << o->getInstanceName().c_str() << "'";
      ANT__Bar_AddButton_P(ANT__GetBarP(bar, gFactory), "AO_object", ANT__Toggle, ANT__PBar(ANT__GetBarP(bar,o)), properties.str().c_str());//Active Object button
    }
#endif
  }

//   SCRIPT_MethodOverload(addBehavior, addBehavior, void, (BehaviorID const& name) );
//   SCRIPT_MethodOverload(removeBehavior, removeBehavior, void, (BehaviorID const& name) );

    Object::Object( void )
      :m_id(-1), m_instanceName("undefined"), m_archetype("undefined"), m_silentDestroy(false)
    {
      ANT_setup();
      gCore->Events.Subscribe("obtainListenHandle", this);
      gCore->Events.UnsubscribeNow("obtainListenHandle", this);

    }

    Object::Object( ObjectID id, Text const& inst, Archetype const& arch )
      :m_id(id), m_instanceName(inst), m_archetype(arch), m_silentDestroy(false)
    {
      ANT_setup();
      gCore->Events.Subscribe("obtainListenHandle", this);
      gCore->Events.UnsubscribeNow("obtainListenHandle", this);

    }
    Object::~Object( void )
    {
      #ifdef ANT_TWEAK_ENABLED
        if(gFactory->activeAntObject == this)
          ANT__ObjSetActive(this);
      #endif

      if(!m_silentDestroy)
      {
        ComponentMap::iterator cmpIt = m_components.begin();
        for(;cmpIt!=m_components.end(); ++cmpIt)
          invoke_removeComponent(cmpIt->second->getTypeName());

        BehaviorMap::iterator it = m_behaviors.begin();
        for(; it != m_behaviors.end(); ++it)
          invoke_detach(it->second);
        invoke_destroy();
      }

      while(m_components.size())
      {
        ANT_RemCmp(m_components.begin()->second);
        delete m_components.begin()->second;
        m_components.erase(m_components.begin());
      }

      ANT_reset();


      //Detach all my children from me
      for(u32 i = 0; i < children.size(); ++i)
      {
        BoneBinding& bind = children[i];
        detachMeFromObject(bind.link);

        //Make sure they no longer have a pointer to me
        bind.link->linkToParent(BoneBinding());

        //If the object was bound to me, kill it now.
        if(bind.bound)
          bind.link->destroy();
      }

      //If I'm attached to something else at all, detach me from it.
      if(parentLink.link)
        detachMeFromObject(parentLink.link);
    }

    Text Object::getInstanceName( void ) const
    {
      return m_instanceName;
    }

    Text Object::getArchetype( void ) const
    {
      return m_archetype;
    }

    Grimelins::ObjectID Object::getID( void ) const
    {
      return m_id;
    }

    void Object::destroy( void )
    {
      gFactory->o_Delete(*this);
    }

    Object* Object::clone( void )
    {
      return gFactory->o_New(*this);
    }

    SET(Object, position)
    {
      m_position = value;
      Body* b = gFactory->getBody(*this);
      if(b) b->setPos(position);
    }
    GET(Object, position)
    {
      return m_position;
    }

    SET(Object, scale)
    {
      m_scale = value;
    }
    GET(Object, scale)
    {
      return m_scale;
    }


    SET(Object, rotation)
    {
      m_rotation = value;
    }
    GET(Object, rotation)
    {
      return m_rotation;
    }



    Grimelins::ScriptVec3 Object::getPosition( void ) const
    {
      return position;
    }

    Grimelins::ScriptVec3 Object::getScale( void ) const
    {
      return scale;
    }

    Grimelins::Quaternion Object::getRotation( void ) const
    {
      return rotation;
    }

    void Object::setPosition( ScriptVec3 const& _pos )
    {
      position = _pos;
    }

    void Object::setScale( ScriptVec3 const& _sca )
    {
      scale = _sca;
    }

    void Object::setRotation( Quaternion const& _rot )
    {
      rotation = _rot;
    }

    void Object::serialize( Object const& source )
    {
      removeAllComponents();
      removeAllBehaviors();

      scale = source.scale;
      rotation = source.rotation;
      position = source.position;

      //duplicate source components
      ComponentMap::const_iterator it = source.m_components.begin();
      for(; it!=source.m_components.end(); ++it)
        addComponent(*it->second);

      //duplicate source behaviors
      BehaviorMap::const_iterator bit = source.m_behaviors.begin();
      for(; bit!=source.m_behaviors.end(); ++bit)
        addBehavior(bit->second);
    }

    void Object::serialize( Scope& s, bool read)
    {
      if(read)
      {
        m_archetype = s.name;

        //behavior list
        if(s.TextList_.size() && s.TextList_.front().name == "behaviors")
        {
          //for each behavior listed
          std::list<Text>& behaviors = s.TextList_.front().data;
          std::list<Text>::iterator bIt = behaviors.begin();
          for(;bIt!=behaviors.end(); ++bIt)
          {
            //obtain the behavior from the factory (keeper of all behaviors)
            //THIS MEMORY IS NOT OWNED (factory takes care of it)
            ErrorIf(m_behaviors.find(*bIt)!=m_behaviors.end(), "Duplicate behavior assigned to a single object!");
            Behavior* b = gFactory->o_Behavior(*bIt);
            ErrorIf(!b, "Invalid or unrecognized behavior assigned to object.\n Archetype: %s\n Instance: %s\n Behvaior: %s\n", m_archetype.c_str(), m_instanceName.c_str(), bIt->c_str());
            m_behaviors[*bIt] = b;
            listenFor(b);
          }
        }

        //components
        Scope::ScopeList::iterator it = s.children.begin();
        for(;it!=s.children.end(); ++it)
        {
          //THIS MEMORY IS OWNED - this object must deallocate it!
          Component* newCmp = gFactory->o_Cmp((*it)->name, **it);
          ErrorIf(newCmp==NULL, "Factory returned NULL when a new component was requested!\n Object Archetype: %s\n ObjectID: %u\n Instance: %s\n Component Name: %s\n", m_archetype.c_str(), m_id, m_instanceName.c_str(), (*it)->name.c_str());
          m_components[newCmp->getTypeID()] = newCmp;
          newCmp->m_parent = this;
          ANT_AddCmp(newCmp);
          //hack
          if((*it)->name == "Body")
          {
            m_scale.x = DCAST(Body*, newCmp)->getXScale();
            m_scale.y = DCAST(Body*, newCmp)->getYScale();
            m_scale.z = DCAST(Body*, newCmp)->getZScale();
          }
        }
      }


    }

    void Object::addBehavior( BehaviorID const& name )
    {
      addBehavior(gFactory->o_Behavior(name));
    }

    void Object::addBehavior( Behavior* behavior )
    {
      ErrorIf(behavior==NULL, "Cannot add a NULL behavior to an object, that doesn't make sense.\n Object Archetype: %s\n ObjectID: %u\n Instance: %s\n", m_archetype.c_str(), m_id, m_instanceName.c_str());
      ErrorIf(m_behaviors.find(behavior->id)!=m_behaviors.end(), "Cannot add Behavior!\n%s is already assigned to this %s (%s)\n", behavior->id.c_str(), m_archetype.c_str(), m_instanceName.c_str());
      m_behaviors[behavior->id] = behavior;
      listenFor(behavior);
      m_iterInvalid = true;
    }

    void Object::removeBehavior( BehaviorID const& name )
    {
      ErrorIf(m_behaviors.find(name)==m_behaviors.end(), "Cannot remove a Behavior!\n%s is not assigned to this %s (%s)\n", name.c_str(), m_archetype.c_str(), m_instanceName.c_str());
      BehaviorMap::iterator it = m_behaviors.find(name);
      if(it!=m_behaviors.end())
      {
        invoke_detach(it->second);
        ignoreFor(it->second);
        m_behaviors.erase(it);
        m_iterInvalid = true;
      }
    }

    void Object::removeBehavior( Behavior* behavior )
    {
      ErrorIf(behavior==NULL, "Cannot remove a NULL behavior from an object, that doesn't make sense.\n Object Archetype: %s\n ObjectID: %u\n Instance: %s\n", m_archetype.c_str(), m_id, m_instanceName.c_str());
      removeBehavior(behavior->id);
      m_iterInvalid = true;
    }

    void Object::removeAllBehaviors( void )
    {
      gCore->Events.UnsubscribeFromAllNow(this);

      BehaviorMap::iterator it = m_behaviors.begin();
      for(; it!=m_behaviors.end(); ++it)
        invoke_detach(it->second);

      m_behaviors.clear();
      m_eventListeners.clear();
      m_iterInvalid = true;
    }

    void Object::addComponent( Component const& source )
    {
      Component* newcmp = gFactory->o_Cmp(source);
      ErrorIf(newcmp==NULL, "NULL component returned from factory when adding Component to Object.\n Object Archetype: %s\n ObjectID: %u\n Instance: %s\n", m_archetype.c_str(), m_id, m_instanceName.c_str());
      m_components[newcmp->getTypeID()] = newcmp;
      newcmp->m_parent = this;
      ANT_AddCmp(newcmp);
      newcmp->addRegistry();
    }

    void Object::removeComponent( TypeID const& id )
    {
      ComponentMap::iterator it = m_components.find(id);
      if(it!=m_components.end())
      {
        invoke_removeComponent(it->second->getTypeName());
        ANT_RemCmp(it->second);
        delete it->second;
        m_components.erase(it);
      }

    }

    void Object::removeComponent( Component* component )
    {
      ErrorIf(component==NULL, "Cannot remove a NULL component from an object, that doesn't make sense.\n Object Archetype: %s\n ObjectID: %u\n Instance: %s\n", m_archetype.c_str(), m_id, m_instanceName.c_str() );
      removeComponent(component->getTypeID());
    }

    void Object::removeAllComponents( void )
    {
      while(m_components.size())
      {
        invoke_removeComponent(m_components.begin()->second->getTypeName());
        ANT_RemCmp(m_components.begin()->second);
        delete m_components.begin()->second;
        m_components.erase(m_components.begin());
      }
    }

    void Object::listenFor( Behavior* b )
    {
      //Obtain a list of all functions this behavior defines, iterate over them
      Behavior::BehaviorFnMap const& fnMap = b->getDefinedFn();
      Behavior::BehaviorFnMap::const_iterator fnIt = fnMap.begin();
      for(;fnIt!=fnMap.end(); ++fnIt)
      {
       //if this is the first, subscribe to listen for this event
        if(m_eventListeners.find(fnIt->first)==m_eventListeners.end())
          gCore->Events.Subscribe(fnIt->first, this);

        //Add to the list, which acts as a refcount
        m_eventListeners[fnIt->first].push_back(b);
      }
    }

    void Object::ignoreFor( Behavior* b )
    {
      //Obtain a list of all functions this behavior defines, iterate over them
      Behavior::BehaviorFnMap const& fnMap = b->getDefinedFn();
      Behavior::BehaviorFnMap::const_iterator fnIt = fnMap.begin();
      for(;fnIt!=fnMap.end(); ++fnIt)
      {
        hstring const& eName = fnIt->first;
        EventListenMap::iterator listenEvent = m_eventListeners.find(eName);
        ErrorIf(listenEvent==m_eventListeners.end(), "Object trying to ignore messages for a behavior it doesn't have?");

        BehaviorList& eListeners = listenEvent->second;

        //remove this behavior from the event listener list
        eListeners.remove(b);

        //if the list is empty, no more behaviors use this event, un-subscribe
        if(eListeners.empty())
        {
          gCore->Events.UnsubscribeNow(eName, this);
          m_eventListeners.erase(listenEvent);
        }
      }
    }

    Grimelins::u1 Object::handleEvent( hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp )
    {
      if(gCore->GamePaused)
        return false;

      EventListenMap::iterator eventIndex = m_eventListeners.find(message);
      if(eventIndex!=m_eventListeners.end() && eventIndex->second.size() > 0)
      {
        RawStore args(arguments);
        BehaviorList& behaviors = eventIndex->second;
        BehaviorList::iterator bit = behaviors.begin();
        m_iterInvalid = false;
        //the test for unchanged size ensures that if behavior list is
        //invalidated by a script during a script call, the loop breaks out.
        //IF A LOOP TERMINATES EARLY remaining functions are not called this frame
        #ifdef OBJ_DEBUG_SCRIPT_ENABLED
          m_currentScriptFunction = message.str();
        #endif

        while(bit!=behaviors.end())
        {
          #ifdef OBJ_DEBUG_SCRIPT_ENABLED
            m_currentScript = *bit;
          #endif
          try
          {
            (*bit)->call(message).Execute(this, &args);
          }
          catch (...)
          {
          	ErrorIf(true, 
              "Script crashed while attempting to process a behavior.\n\nObject: %s\nInstance: %s\nBehavior: %s%s\nFunction: %s%s\nArg Data: %s\nMessage: %s\n\n", 
              m_archetype.c_str(),
              m_instanceName.c_str(),
              (*bit)->id.c_str(), ".nut",
              "on::", message.str(),
              (args.HasData()?"true":"false"),
              message.str()
            );
          #ifdef OBJ_DEBUG_SCRIPT_ENABLED
            m_currentScript = NULL;
          #endif

            return false;
          }
          #ifdef OBJ_DEBUG_SCRIPT_ENABLED
            m_currentScript = NULL;
          #endif
          if(m_iterInvalid)
            break;
          ++bit;
        }
        #ifdef OBJ_DEBUG_SCRIPT_ENABLED
          m_currentScriptFunction.clear();
        #endif

      }

      return false;//objects NEVER consume
    }

    //NOTE: invoke create/attach are NOT automatically invoked, to allow the factory to create objects, and manipulate them, and manually invoke these methods when it's done
    // where as destroy/detach are automatic.

    // script func signature:
    //       function on::create(Object)
    void Object::invoke_create( void )
    {
      static const BehaviorFnName scriptFn = "create";
      BehaviorMap::iterator it = m_behaviors.begin();
      for(;it!=m_behaviors.end(); ++it)
        if(it->second->defines(scriptFn))
          it->second->call(scriptFn).Execute(this);
    }

    // script func signature:
    //       function on::destroy(Object)
    void Object::invoke_destroy( void )
    {
      static const BehaviorFnName scriptFn = "destroy";
      BehaviorMap::iterator it = m_behaviors.begin();
      for(;it!=m_behaviors.end(); ++it)
        if(it->second->defines(scriptFn))
          it->second->call(scriptFn).Execute(this);
    }

    // script func signature:
    //       function on::attach(Object)
    void Object::invoke_attach( Behavior* behavior )
    {
      ErrorIf(!behavior, "invoke_attach() passed NULL behavior.\n Object Archetype: %s\n ObjectID: %u\n Instance: %s\n", m_archetype.c_str(), m_id, m_instanceName.c_str() );
      ANT__Bar_AddButton_P(bar, behavior->id.c_str(), ANT__Toggle, ANT__PBar(ANT__GetBarP(bar, behavior)), " group=Behaviors");
      static const BehaviorFnName scriptFn = "attach";
      if(behavior->defines(scriptFn))
      {
        #ifdef OBJ_DEBUG_SCRIPT_ENABLED
          m_currentScriptFunction = "auto: attach";
          m_currentScript = behavior;
        #endif
        behavior->call(scriptFn).Execute(this);
        #ifdef OBJ_DEBUG_SCRIPT_ENABLED
          m_currentScriptFunction.clear();
          m_currentScript = NULL;
        #endif
      }

    }

    void Object::invoke_attachAll( void )
    {
      BehaviorMap::iterator it = m_behaviors.begin();
      for(; it != m_behaviors.end(); ++it)
        invoke_attach(it->second);
    }

    // script func signature:
    //       function on::detach(Object)
    void Object::invoke_detach( Behavior* behavior )
    {
      ErrorIf(!behavior, "invoke_detach() passed NULL behavior.\n Object Archetype: %s\n ObjectID: %u\n Instance: %s\n", m_archetype.c_str(), m_id, m_instanceName.c_str() );
      ANT__Bar_Remove(bar,  behavior->id.c_str());
      static const BehaviorFnName scriptFn = "detach";
      if(behavior->defines(scriptFn))
      {
        #ifdef OBJ_DEBUG_SCRIPT_ENABLED
          m_currentScriptFunction = "auto: detach";
          m_currentScript = behavior;
        #endif
        behavior->call(scriptFn).Execute(this);
        #ifdef OBJ_DEBUG_SCRIPT_ENABLED
          m_currentScriptFunction.clear();
          m_currentScript = NULL;
        #endif
      }
    }

    // script func signature:
    //       function on::addComponent(Object, ComponentName)
    void Object::invoke_addComponent( Text const& component )
    {
      static const BehaviorFnName scriptFn = "addComponent";
      BehaviorMap::iterator it = m_behaviors.begin();
      for(;it!=m_behaviors.end(); ++it)
      {
        if(it->second->defines(scriptFn))
        {
          #ifdef OBJ_DEBUG_SCRIPT_ENABLED
            m_currentScriptFunction = "auto: addComponent";
            m_currentScript = it->second;
          #endif
          BehaviorFn& func = it->second->call(scriptFn);
          func.Execute(this, component);//NOTICE THIS FUNCTION TAKES 2 PARAMS!!!
          #ifdef OBJ_DEBUG_SCRIPT_ENABLED
            m_currentScriptFunction.clear();
            m_currentScript = NULL;
          #endif

        }
      }
    }

    // script func signature:
    //       function on::removeComponent(Object, ComponentName)
    void Object::invoke_removeComponent( Text const& component )
    {
      static const BehaviorFnName scriptFn = "removeComponent";
      BehaviorMap::iterator it = m_behaviors.begin();
      for(;it!=m_behaviors.end(); ++it)
        if(it->second->defines(scriptFn))
        {
          #ifdef OBJ_DEBUG_SCRIPT_ENABLED
            m_currentScriptFunction = "auto: removeComponent";
            m_currentScript = it->second;
          #endif
          it->second->call(scriptFn).Execute(this, component);//NOTICE THIS FUNCTION TAKES 2 PARAMS!!!
          #ifdef OBJ_DEBUG_SCRIPT_ENABLED
            m_currentScriptFunction.clear();
            m_currentScript = NULL;
          #endif
        }

    }


#ifdef ANT_TWEAK_ENABLED
#define ANTENUM_Val(v) {v, #v}
//     TwEnumVal ANT_ProjectileState[] =
//     {
//       ANTENUM_Val(notShot),
//       ANTENUM_Val(caughtByPlayer),
//       ANTENUM_Val(shotByBoss),
//       ANTENUM_Val(shotByPlayer)
//     };
    TwEnumVal ANT_NarrowPhaseType[] =
    {
      ANTENUM_Val(Obb), 
      ANTENUM_Val(BoundingSphere)
    };
    TwEnumVal ANT_CollisionType[] =
    {
      ANTENUM_Val(Player1),
      ANTENUM_Val(Boss),
      ANTENUM_Val(DamageSource),
      ANTENUM_Val(Environment)
    };
//     TwEnumVal ANT_PlayerState[] =
//     {
//       ANTENUM_Val(HasAmmo),
//       ANTENUM_Val(Empty),
//       ANTENUM_Val(Catching)
//     };
#endif

    ANT__RETVAL PlayAnimation(void* ud)
    {
#ifdef ANT_TWEAK_ENABLED
      Object* active = gFactory->activeAntObject;
      if(active)
      {
        gfxModel* model = SCAST(gfxModel*, active->getComponentT<gfxModel>());
        if(model)
          model->ChangeAnimNOWIndex(model->blendIndex);
      }
#endif
    }

    ANT__RETVAL BlendAnimation(void* ud)
    {
#ifdef ANT_TWEAK_ENABLED
      Object* active = gFactory->activeAntObject;
      if(active)
      {
        gfxModel* model = SCAST(gfxModel*, active->getComponentT<gfxModel>());
        if(model)
          model->BlendToNextAnim();
      }
#endif
    }


    void Object::ANT_AddCmp( Component* cmp )
    {
#ifdef ANT_TWEAK_ENABLED

//       if(cmp->getTypeName() == "Transform")
//       {
//         Transform* c = (Transform*)cmp;
//         ANT__Bar_AddVarRW_P(bar, "t_pos", &(c->m_position), vec3, " group=Transform label=Position");
//         ANT__Bar_AddVarRW_P(bar, "t_sca", &(c->m_scale), vec3, " group=Transform label=Scale");
//         ANT__Bar_AddVarRW_P(bar, "t_rot", &(c->m_rotation), vec3, " group=Transform label=Rotation");
//         ANT__Define(bar, "/Transform opened=false");
//       }
//       else 
        if(cmp->getTypeName() == "Body")
      {
        Body* c = (Body*)cmp;

        ANT__Bar_AddVarRW_P(bar, "b_motion", &(c->m_isMotionEnabled), u1, " group=Body label='Motion Enabled'");
        ANT__Bar_AddVarRW_P(bar, "b_collision", &(c->m_isCollisionEnabled), u1, " group=Body label='Collision Enabled'");
        TwType TwNarrowPhaseType = TwDefineEnum("NarrowPhaseType", ANT_NarrowPhaseType, numNarrowTypes);
        ANT__Bar_AddVarRW_P_Oride(bar, "b_narrowp", &(c->m_narrowType), TwNarrowPhaseType, " group=Body label='Narrow Phase Type'");
        TwType TwCollisionType = TwDefineEnum("CollisionType", ANT_CollisionType, CollisionTypeCount);
        ANT__Bar_AddVarRW_P_Oride(bar, "b_colltype", &(c->m_collisionType), TwCollisionType, " group=Body label='Collision Type'");

        //CURRENT ============================================================================================================================
        ANT__Bar_AddVarRW_P(bar, "b_cmass", &(c->m_current.m_mass), f32, " min=0 step=0.01 group=B_Current label=mass");
        ANT__Bar_AddVarRW_P(bar, "b_cxsize", &(c->m_current.m_xSize), f32, " min=0.01 step=0.01 group=B_Current label='x size'");
        ANT__Bar_AddVarRW_P(bar, "b_cysize", &(c->m_current.m_ySize), f32, " min=0.01 step=0.01 group=B_Current label='y size'");
        ANT__Bar_AddVarRW_P(bar, "b_czsize", &(c->m_current.m_zSize), f32, " min=0.01 step=0.01 group=B_Current label='z size'");
        ANT__Bar_AddVarRW_P(bar, "b_cimass", &(c->m_current.m_invMass), f32, " min=0 step=0.01 group=B_Current label='inv. mass'");
        ANT__Bar_AddVarRW_P(bar, "b_ctensor", &(c->m_current.m_inerTensor), f32, " min=0 step=0.01 group=B_Current label='inner tensor'");
        ANT__Bar_AddVarRW_P(bar, "b_citensor", &(c->m_current.m_invInerTensor), f32, " min=0 step=0.01 group=B_Current label='inv. inner tensor'");
        ANT__Bar_AddVarRW_P(bar, "b_cstatic", &(c->m_current.m_us), f32, " min=0 step=0.01 group=B_Current label=static");
        ANT__Bar_AddVarRW_P(bar, "b_cdrag", &(c->m_current.m_ud), f32, " min=0 step=0.01 group=B_Current label=drag");
        ANT__Bar_AddVarRW_P(bar, "b_cpos", &(c->m_current.m_position), vec3, " group=B_Current label=Position");
        ANT__Bar_AddVarRW_P(bar, "b_cmom", &(c->m_current.m_momentum), vec3, " group=B_Current label=Momentum");
        ANT__Bar_AddVarRW_P(bar, "b_cdpos", &(c->m_current.m_deltaPos), vec3, " group=B_Current label='Delta Position'");
        ANT__Bar_AddVarRW_P(bar, "b_camom", &(c->m_current.m_angMomentum), vec3, " group=B_Current label='Angular Momentum'");
        ANT__Bar_AddVarRW_P(bar, "b_cavel", &(c->m_current.m_angVelocity), vec3, " group=B_Current label='Angular Velocity'");
        ANT__Bar_AddVarRW_P(bar, "b_corien", &(c->m_current.m_orientation), Quat, " group=BC_Orient label='visual' opened=true");
        ANT__Bar_AddVarRW_P(bar, "b_corienx", &(c->m_current.m_orientation.x), f32, "group=BC_Orient min=0 max=1 step=0.0001 label=' X'");
        ANT__Bar_AddVarRW_P(bar, "b_corieny", &(c->m_current.m_orientation.y), f32, "group=BC_Orient min=0 max=1 step=0.0001 label=' Y'");
        ANT__Bar_AddVarRW_P(bar, "b_corienz", &(c->m_current.m_orientation.z), f32, "group=BC_Orient min=0 max=1 step=0.0001 label=' Z'");
        ANT__Bar_AddVarRW_P(bar, "b_corienw", &(c->m_current.m_orientation.w), f32, "group=BC_Orient min=0 max=1 step=0.0001 label=' W'");
        ANT__Define(bar, "/BC_Orient group=B_Current label=Orientation opened=false");
        ANT__Bar_AddVarRW_P(bar, "b_cspin", &(c->m_current.m_spin), Quat, " group=BC_Spin label='visual' opened=true");
        ANT__Bar_AddVarRW_P(bar, "b_cspinx", &(c->m_current.m_spin.x), f32, "group=BC_Spin min=0 max=1 step=0.0001 label=' X'");
        ANT__Bar_AddVarRW_P(bar, "b_cspiny", &(c->m_current.m_spin.y), f32, "group=BC_Spin min=0 max=1 step=0.0001 label=' Y'");
        ANT__Bar_AddVarRW_P(bar, "b_cspinz", &(c->m_current.m_spin.z), f32, "group=BC_Spin min=0 max=1 step=0.0001 label=' Z'");
        ANT__Bar_AddVarRW_P(bar, "b_cspinw", &(c->m_current.m_spin.w), f32, "group=BC_Spin min=0 max=1 step=0.0001 label=' W'");
        ANT__Define(bar, "/BC_Spin group=B_Current label=Spin opened=false");

        ANT__Define(bar, "/B_Current group=Body label=Current opened=false");
        //PREVIOUS ============================================================================================================================
        ANT__Bar_AddVarRO_P(bar, "b_pmass", &(c->m_previous.m_mass), f32, " min=0 step=0.01 group=b_previous label=mass");
        ANT__Bar_AddVarRW_P(bar, "b_pxsize", &(c->m_previous.m_xSize), f32, " min=0.01 step=0.01 group=b_previous label='x size'");
        ANT__Bar_AddVarRW_P(bar, "b_pysize", &(c->m_previous.m_ySize), f32, " min=0.01 step=0.01 group=b_previous label='y size'");
        ANT__Bar_AddVarRW_P(bar, "b_pzsize", &(c->m_previous.m_zSize), f32, " min=0.01 step=0.01 group=b_previous label='z size'");
        ANT__Bar_AddVarRO_P(bar, "b_pimass", &(c->m_previous.m_invMass), f32, " min=0 step=0.01 group=b_previous label='inv. mass'");
        ANT__Bar_AddVarRO_P(bar, "b_ptensor", &(c->m_previous.m_inerTensor), f32, " min=0 step=0.01 group=b_previous label='inner tensor'");
        ANT__Bar_AddVarRO_P(bar, "b_pitensor", &(c->m_previous.m_invInerTensor), f32, " min=0 step=0.01 group=b_previous label='inv. inner tensor'");
        ANT__Bar_AddVarRO_P(bar, "b_pstatic", &(c->m_previous.m_us), f32, " min=0 step=0.01 group=b_previous label=static");
        ANT__Bar_AddVarRO_P(bar, "b_pdrag", &(c->m_previous.m_ud), f32, " min=0 step=0.01 group=b_previous label=drag");
        ANT__Bar_AddVarRO_P(bar, "b_ppos", &(c->m_previous.m_position), vec3, " group=b_previous label=Position");
        ANT__Bar_AddVarRO_P(bar, "b_pmom", &(c->m_previous.m_momentum), vec3, " group=b_previous label=Momentum");
        ANT__Bar_AddVarRO_P(bar, "b_pdpos", &(c->m_previous.m_deltaPos), vec3, " group=b_previous label='Delta Position'");
        ANT__Bar_AddVarRO_P(bar, "b_pamom", &(c->m_previous.m_angMomentum), vec3, " group=b_previous label='Angular Momentum'");
        ANT__Bar_AddVarRO_P(bar, "b_pavel", &(c->m_previous.m_angVelocity), vec3, " group=b_previous label='Angular Velocity'");
        ANT__Bar_AddVarRO_P(bar, "b_porien", &(c->m_previous.m_orientation), Quat, " group=BP_Orient label='visual' opened=true");
        ANT__Bar_AddVarRO_P(bar, "b_porienx", &(c->m_previous.m_orientation.x), f32, "group=BP_Orient min=0 max=1 step=0.0001 label=' X'");
        ANT__Bar_AddVarRO_P(bar, "b_porieny", &(c->m_previous.m_orientation.y), f32, "group=BP_Orient min=0 max=1 step=0.0001 label=' Y'");
        ANT__Bar_AddVarRO_P(bar, "b_porienz", &(c->m_previous.m_orientation.z), f32, "group=BP_Orient min=0 max=1 step=0.0001 label=' Z'");
        ANT__Bar_AddVarRO_P(bar, "b_porienw", &(c->m_previous.m_orientation.w), f32, "group=BP_Orient min=0 max=1 step=0.0001 label=' W'");
        ANT__Define(bar, "/BP_Orient group=b_previous label=Orientation opened=false");
        ANT__Bar_AddVarRO_P(bar, "b_pspin", &(c->m_previous.m_spin), Quat, " group=BP_Spin label='visual' opened=true");
        ANT__Bar_AddVarRO_P(bar, "b_pspinx", &(c->m_previous.m_spin.x), f32, "group=BP_Spin min=0 max=1 step=0.0001 label=' X'");
        ANT__Bar_AddVarRO_P(bar, "b_pspiny", &(c->m_previous.m_spin.y), f32, "group=BP_Spin min=0 max=1 step=0.0001 label=' Y'");
        ANT__Bar_AddVarRO_P(bar, "b_pspinz", &(c->m_previous.m_spin.z), f32, "group=BP_Spin min=0 max=1 step=0.0001 label=' Z'");
        ANT__Bar_AddVarRO_P(bar, "b_pspinw", &(c->m_previous.m_spin.w), f32, "group=BP_Spin min=0 max=1 step=0.0001 label=' W'");
        ANT__Define(bar, "/BP_Spin group=b_previous label=Spin opened=false");

        ANT__Define(bar, "/b_previous group=Body label=previous opened=false");
        //collapse body group
        ANT__Define(bar, "/Body opened=false");

      }
//       else if(cmp->getTypeName() == "BossComponent")
//       {
//         BossComponent* c = (BossComponent*)cmp;
//         ANT__Bar_AddVarRW_P(bar, "bc_health", &(c->health), s32, " group=Boss label=Health");
//         ANT__Bar_AddVarRW_P(bar, "bc_head", &(c->headPos), vec3, " group=Boss label='Head Position'");
//         ANT__Define(bar, "/Boss opened=false");
//       }
//       else if(cmp->getTypeName() == "ProjectileComponent")
//       {
//         ProjectileComponent* c = (ProjectileComponent*)cmp;
//         ANT__Bar_AddVarRW_P(bar, "pc_dir", &(c->dirVector), vec3, " group=Projectile label=Direction");
//         ANT__Bar_AddVarRW_P(bar, "pc_speed", &(c->speed), f32, " group=Projectile label=Speed");
//         ANT__Bar_AddVarRW_P(bar, "pc_shot", &(c->shot), u1, " group=Projectile label=Shot");
//         TwType TwProjectileState = TwDefineEnum("ProjectileState", ANT_ProjectileState, stateNum);
//         ANT__Bar_AddVarRW_P_Oride(bar, "pc_state", &(c->state), TwProjectileState, " group=Projectile label=State");
//         ANT__Define(bar, "/Projectile opened=false");
//       }
//       else if(cmp->getTypeName() == "PlayerComponent")
//       {
//         PlayerComponent* c = (PlayerComponent*)cmp;
//         ANT__Bar_AddVarRW_P(bar, "pl_health", &(c->health), s32, " group=Player label=Health");
//         TwType TwPlayerState = TwDefineEnum("ProjectileState", ANT_PlayerState, stateNum);
//         ANT__Bar_AddVarRW_P_Oride(bar, "pl_state", &(c->state), TwPlayerState, " group=Player label=State");
//         ANT__Define(bar, "/Player opened=false");
//       }
      else if(cmp->getTypeName() == "ShapeComponent")
      {
        ANT__Bar_AddButton_P(bar, "sc_noParams", NULL, NULL, " group=Shape label='<nothing to view>'");
      }
      else if(cmp->getTypeName() == "ModelComponent")
      {
        gfxModel* c = (gfxModel*)cmp;

        std::stringstream prop;
        prop << " group=Model label='" << c->modelname.c_str() << "'";
        ANT__Bar_AddVarRW_P(bar, "mc_scale", &(c->model->scale), vec3, " group=Model label='scale'");
        ANT__Bar_AddVarRW_P(bar, "mc_trans", &(c->model->trans), vec3, " group=Model label='trans'");
        ANT__Bar_AddVarRW_P(bar, "mc_dir", &(c->model->dir), vec3, " group=Model label='dir'");
        ANT__Bar_AddVarRW_P(bar, "mc_col", &(c->model->diffCol), vec3, " group=Model label='color'");

        if(c->model->controller_)
        {

          for(u32 i = 0; i < c->model->controller_->animations_.size(); ++i)
          {
            std::stringstream s;
            s << " group=Animations label='" << i << " " << c->model->controller_->animations_[i]->name_.c_str() << "'";
            Text settings = s.str();
            ANT__Bar_AddVarRW_P(bar, c->model->controller_->animations_[i]->name_.c_str(), &c->blendTime, f32, settings.c_str());
          }
           
           ANT__Bar_AddVarRW_P(bar, "mc_blend", &(c->blendTime), f32, " step=0.01 label='Blend Time First (s)'");
           ANT__Bar_AddVarRW_P(bar, "mc_blend1", &(c->blendNextTime), f32, " step=0.01 label='Blend Time Second (s)'");
           ANT__Bar_AddVarRW_P(bar, "mc_index", &(c->blendIndex), u32, " label='Index of Final Anim'");
           ANT__Bar_AddVarRW_P(bar, "mc_index1", &(c->betweenIndex), u32, " label='Index of Between Anim'");
           ANT__Bar_AddButton(bar, "Play Final Animation", PlayAnimation, NULL);
           ANT__Bar_AddButton(bar, "Blend To Final Animation", BlendAnimation, NULL);
           ANT__Define(bar, "/Animations opened=false");
        }

        ANT__Define(bar, "/Model opened=false");
      }
      else
      {
        ANT__Bar_AddButton_P(bar, cmp->getTypeName().c_str(), NULL, NULL, " group='Other Components'");
      }
#endif
    }


    void Object::ANT_RemCmp( Component* cmp )
    {
#ifdef ANT_TWEAK_ENABLED

      if(cmp->getTypeName() == "Transform")
      {
        ANT__Bar_Remove(bar, "t_pos");
        ANT__Bar_Remove(bar, "t_sca");
        ANT__Bar_Remove(bar, "t_rot");
      }
      else 
        if(cmp->getTypeName() == "Body")
      {
        ANT__Bar_Remove(bar, "b_motion");
        ANT__Bar_Remove(bar, "b_collision");
        ANT__Bar_Remove(bar, "b_narrowp");
        ANT__Bar_Remove(bar, "b_colltype");
        ANT__Bar_Remove(bar, "b_cmass");
        ANT__Bar_Remove(bar, "b_cxsize");
        ANT__Bar_Remove(bar, "b_cysize");
        ANT__Bar_Remove(bar, "b_czsize");
        ANT__Bar_Remove(bar, "b_cimass");
        ANT__Bar_Remove(bar, "b_ctensor");
        ANT__Bar_Remove(bar, "b_citensor");
        ANT__Bar_Remove(bar, "b_cstatic");
        ANT__Bar_Remove(bar, "b_cdrag");
        ANT__Bar_Remove(bar, "b_cpos");
        ANT__Bar_Remove(bar, "b_cmom");
        ANT__Bar_Remove(bar, "b_cdpos");
        ANT__Bar_Remove(bar, "b_camom");
        ANT__Bar_Remove(bar, "b_cavel");
        ANT__Bar_Remove(bar, "b_corien");
        ANT__Bar_Remove(bar, "b_corienx");
        ANT__Bar_Remove(bar, "b_corieny");
        ANT__Bar_Remove(bar, "b_corienz");
        ANT__Bar_Remove(bar, "b_corienw");
        ANT__Bar_Remove(bar, "b_cspin");
        ANT__Bar_Remove(bar, "b_cspinx");
        ANT__Bar_Remove(bar, "b_cspiny");
        ANT__Bar_Remove(bar, "b_cspinz");
        ANT__Bar_Remove(bar, "b_cspinw");
        ANT__Bar_Remove(bar, "b_pmass");
        ANT__Bar_Remove(bar, "b_pxsize");
        ANT__Bar_Remove(bar, "b_pysize");
        ANT__Bar_Remove(bar, "b_pzsize");
        ANT__Bar_Remove(bar, "b_pimass");
        ANT__Bar_Remove(bar, "b_ptensor");
        ANT__Bar_Remove(bar, "b_pitensor");
        ANT__Bar_Remove(bar, "b_pstatic");
        ANT__Bar_Remove(bar, "b_pdrag");
        ANT__Bar_Remove(bar, "b_ppos");
        ANT__Bar_Remove(bar, "b_pmom");
        ANT__Bar_Remove(bar, "b_pdpos");
        ANT__Bar_Remove(bar, "b_pamom");
        ANT__Bar_Remove(bar, "b_pavel");
        ANT__Bar_Remove(bar, "b_porien");
        ANT__Bar_Remove(bar, "b_porienx");
        ANT__Bar_Remove(bar, "b_porieny");
        ANT__Bar_Remove(bar, "b_porienz");
        ANT__Bar_Remove(bar, "b_porienw");
        ANT__Bar_Remove(bar, "b_pspin");
        ANT__Bar_Remove(bar, "b_pspinx");
        ANT__Bar_Remove(bar, "b_pspiny");
        ANT__Bar_Remove(bar, "b_pspinz");
        ANT__Bar_Remove(bar, "b_pspinw");
      }
//       else if(cmp->getTypeName() == "BossComponent")
//       {
//         ANT__Bar_Remove(bar, "bc_health");
//         ANT__Bar_Remove(bar, "bc_head");
//       }
//       else if(cmp->getTypeName() == "ProjectileComponent")
//       {
//         ANT__Bar_Remove(bar, "pc_dir");
//         ANT__Bar_Remove(bar, "pc_speed");
//         ANT__Bar_Remove(bar, "pc_shot");
//         ANT__Bar_Remove(bar, "pc_state");
//       }
//       else if(cmp->getTypeName() == "PlayerComponent")
//       {
//         ANT__Bar_Remove(bar, "pl_health");
//         ANT__Bar_Remove(bar, "pl_state");
//       }
      else if(cmp->getTypeName() == "ShapeComponent")
      {
        ANT__Bar_Remove(bar, "sc_noParams");
      }
      else if(cmp->getTypeName() == "ModelComponent")
      {
        ANT__Bar_Remove(bar, "mc_scale");
        ANT__Bar_Remove(bar, "mc_trans");
        ANT__Bar_Remove(bar, "mc_dir");
        ANT__Bar_Remove(bar, "mc_col");
      }
      else
      {
        ANT__Bar_Remove(bar, cmp->getTypeName().c_str());
      }
#endif
    }

    bool Object::hasBehavior( BehaviorID const& name )
    {
      return m_behaviors.find(name)!=m_behaviors.end();
    }

    void Object::ANT_setup( void )
    {
      ANT__Bar_Init(bar, (m_instanceName==OBJECT_ARCHTYPE_NAME?(m_archetype + "Archetype").c_str():m_instanceName.c_str()), false);

      ANT__Bar_AddButton(bar, "HIDE", ANT__Hide, ANT__PBar(bar));
      ANT__Bar_AddButton_P(bar, "Activater", ANT__ObjSetActive, this, "label='Toggle Active Object'");
      ANT__Bar_AddSeperatorAny(bar);//================================================
      if(m_instanceName!=OBJECT_ARCHTYPE_NAME) 
      {
        ANT__Bar_AddButton_P(ANT__GetBarP(bar, FACTORY), m_instanceName.c_str(), ANT__Toggle, ANT__PBar(bar), "group=Objects" );//add factory object button
        ANT__Bar_AddButton(bar, "Delete", ANT__ObjDeleteThis, this);
      }
      else
      {
        ANT__Bar_AddButton_P(ANT__GetBarP(bar, FACTORY), m_archetype.c_str(), ANT__Toggle, ANT__PBar(bar), "group=Archetypes" );//add factory archetype button
        ANT__Bar_AddButton(bar, "New Copy", ANT__ObjCopyThis, this);
      }
      ANT__Bar_AddSeperatorAny(bar);//================================================
      Text prop("label='Archetype: ");
      prop += m_archetype + "'";
      ANT__Bar_AddButton_P(bar, "ArchName", NULL, NULL, prop.c_str());

//       ANT__Bar_AddVarRW(bar, "Position", &(m_position), vec3);
//       ANT__Bar_AddVarRW(bar, "Scale", &(m_scale), vec3);
//       ANT__Bar_AddVarRW(bar, "Rotation", &(m_rotation), vec3);

    }

    void Object::ANT_reset( void )
    {
      if(m_instanceName!=OBJECT_ARCHTYPE_NAME) 
        ANT__Bar_Remove(ANT__GetBarP(bar, FACTORY), m_instanceName.c_str());//remove factory button for object
      else
        ANT__Bar_Remove(ANT__GetBarP(bar, FACTORY), m_archetype.c_str());//remove factory button for archetype

      ANT__Bar_Deinit(bar);
    }

    void Object::detachMeFromObject( Object* obj )
    {
      //search their attachments list for me, and remove myself
      Object* me = this;
      std::vector<BoneBinding>& bindings = obj->children;
      for(u32 i = 0; i < bindings.size(); ++i)
      {
        if(bindings[i].link == me)
        {
          bindings.erase(bindings.begin() + i);
          break;
        }
      }
    }

    //Attach an object to myself.
    void Object::attachObjectToMe( Object* obj )
    {
      BoneBinding bind;
      bind.link = obj;

      //Add this child to my list of attachments
      obj->children.push_back(bind);
    }

    void Object::linkToParent( BoneBinding& link )
    {
      parentLink = link;
    }

    void Object::scriptCall( Text const& message, RawStore const& data )
    {
      EventListenMap::iterator eventIndex = m_eventListeners.find(message);
      if(eventIndex!=m_eventListeners.end() && eventIndex->second.size() > 0)
      {
        RawStore args(data);
        BehaviorList& behaviors = eventIndex->second;
        BehaviorList::iterator bit = behaviors.begin();
        size_t origECount = m_eventListeners.size();
        size_t origBCount = m_behaviors.size();
#ifdef OBJ_DEBUG_SCRIPT_ENABLED
        m_currentScriptFunction = message.c_str();
#endif

        while(bit!=behaviors.end())
        {
#ifdef OBJ_DEBUG_SCRIPT_ENABLED
          m_currentScript = *bit;
#endif
          try
          {
            (*bit)->call(message).Execute(this, &args);
          }
          catch (...)
          {
            ErrorIf(true, 
              "Script crashed while attempting to process a behavior from *direct* call.\n\nObject: %s\nInstance: %s\nBehavior: %s%s\nFunction: %s%s\nArg Data: %s\nMessage: %s\n\n", 
              m_archetype.c_str(),
              m_instanceName.c_str(),
              (*bit)->id.c_str(), ".nut",
              "on::", message.c_str(),
              (args.HasData()?"true":"false"),
              message.c_str()
              );
#ifdef OBJ_DEBUG_SCRIPT_ENABLED
            m_currentScript = NULL;
#endif
             return;
          }
#ifdef OBJ_DEBUG_SCRIPT_ENABLED
          m_currentScript = NULL;
#endif

          if(m_eventListeners.size()!=origECount || m_behaviors.size()!=origBCount)
            break;
          ++bit;
        }
#ifdef OBJ_DEBUG_SCRIPT_ENABLED
        m_currentScriptFunction.clear();
#endif

      }


    }

    void Object::scriptCallSpecial( Text const& message )
    {
      EventListenMap::iterator eventIndex = m_eventListeners.find(message);
      if(eventIndex!=m_eventListeners.end() && eventIndex->second.size() > 0)
      {
        BehaviorList& behaviors = eventIndex->second;
        BehaviorList::iterator bit = behaviors.begin();
        size_t origECount = m_eventListeners.size();
        size_t origBCount = m_behaviors.size();
        while(bit!=behaviors.end())
        {
          try
          {
            (*bit)->call(message).Execute(this);
          }
          catch (...)
          {
            ErrorIf(true, 
              "Script crashed while attempting to process a behavior from *direct* call.\n\nObject: %s\nInstance: %s\nBehavior: %s%s\nFunction: %s%s\nArg Data: N/A\nMessage: %s\n\n", 
              m_archetype.c_str(),
              m_instanceName.c_str(),
              (*bit)->id.c_str(), ".nut",
              "on::", message.c_str(),
              message.c_str()
              );
            return;
          }
          if(m_eventListeners.size()!=origECount || m_behaviors.size()!=origBCount)
            break;
          ++bit;
        }
      }

    }

    void Object::scriptAddBehavior( Text const& name )
    {
      Behavior* b = gFactory->o_Behavior(name);
      ErrorIf(b==NULL, "Script request for '%s', behavior not found.\n",name.c_str());
      ErrorIf(m_behaviors.find(b->id)!=m_behaviors.end(), "Script cannot add Behavior!\n%s is already assigned to this %s (%s)\n", b->id.c_str(), m_archetype.c_str(), m_instanceName.c_str());
      addBehavior(b);
      invoke_attach(b);
    }

    void Object::scriptRemoveBehavior( Text const& name )
    {
      ErrorIf(m_behaviors.find(name)==m_behaviors.end(), "Script cannot remove a Behavior!\n%s is not assigned to this %s (%s)\n", name.c_str(), m_archetype.c_str(), m_instanceName.c_str());
      removeBehavior(name);
    }

    BoneBinding::BoneBinding() : link(NULL), bound(false), pos(NULL)
    {
    }

#ifdef OBJ_DEBUG_SCRIPT_ENABLED
#define IF_MISSING_COMPONENT(cmpName)                                   \
  if(component == #cmpName)                                             \
  ErrorIf(gFactory->get##cmpName(*this) == NULL, "REQUIRED COMPONENT MISSING!\n   A behavior was executed on an object with a missing dependacy.\n   - Missing Component: %s\n   - Behavior: %s\n   - Function: %s\n   - ObjectID: %u\n   - Archetype: %s\n   - Instance: %s\n\n", #cmpName, m_currentScript->id.c_str(), m_currentScriptFunction.c_str(), getID(), getArchetype().c_str(), getInstanceName().c_str())
#endif


    void Object::REQ_CMP( Text const& component )
    {
      #ifdef OBJ_DEBUG_SCRIPT_ENABLED
        IF_MISSING_COMPONENT(Body);
        else IF_MISSING_COMPONENT(Data);
        else IF_MISSING_COMPONENT(gfxModel);
        else IF_MISSING_COMPONENT(gfxShape);
        else ErrorIf(true, "REQUIRED COMPONENT INCORECTLY SPECIFIED!\n   %s is not a recognized component. Check your script requirement code.\n   - Improper Component Name: %s\n   - Behavior: %s\n   - Function: %s\n   - ObjectID: %u\n   - Archetype: %s\n   - Instance: %s\n\n", component.c_str(), component.c_str(), m_currentScript->id.c_str(), m_currentScriptFunction.c_str(), getID(), getArchetype().c_str(), getInstanceName().c_str());
      #endif
    }



    void Object::REQ_VAL(Text const& varaible, Text const& type )
    {
      #ifdef OBJ_DEBUG_SCRIPT_ENABLED
        static const Text component = "Data";
        IF_MISSING_COMPONENT(Data);
        Data* d = gFactory->getData(*this);
        ErrorIf(d==NULL, "REQ_VAL failed unexpetedly. Data component being retrieved came back null when it should have been fine. There is a logic error in C++ here....\n");
        ErrorIf(!d->exists(varaible, type), "REQUIRED DATA MISSING!\n   A behavior was executed on an object that required a variable in the data component.\n   - Missing Variable: %s\n   - Missing Type: %s\n   - Behavior: %s\n   - Function: %s\n   - ObjectID: %u\n   - Archetype: %s\n   - Instance: %s\n\n", varaible.c_str(), type.c_str(), m_currentScript->id.c_str(), m_currentScriptFunction.c_str(), getID(), getArchetype().c_str(), getInstanceName().c_str());
      #endif
    }
};//Virtus namespace
