/*! ====================================================================================================================
  @file object.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.16.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#ifndef VIRTUS_OBJECT_H
#define VIRTUS_OBJECT_H


#define OBJ_DEBUG_SCRIPT_ENABLED //comment out if undesired.



#define OBJECT_ARCHTYPE_NAME "Archetype Base"

#define ifBehavior(var, obj, name) Behavior* var = obj.getBehavior_(name); if(var)
#define ifComponent(var, obj, typ) Behavior* var = obj.getComponent_<typ>(); if(var)

#include "behavior.h"

namespace Grimelins
{
    class Component;

    typedef u32 ObjectID;
    typedef Text Archetype;

    class BoneBinding
    {
      public:
        BoneBinding();

        //This is the link if there is one present.
        Object* link;

        //If there's a link, is this the bound object?  We're a part of the other object now.
        bool bound;

        //The thing that's being updated every frame.
        vec3* pos;
    };

    SCRIPT_HeaderDefine(Object);

    class Object : public IListener
    {
    public://ANT section
      ANT__Bar(bar);
      void ANT_AddCmp(Component* cmp);
      void ANT_RemCmp(Component* cmp);
      void ANT_setup(void);
      void ANT_reset(void);
    public:
      ~Object(void);

      //IListener Interface
      virtual u1 handleEvent( hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp );

//       Behavior* getBehavior_(Text const& name);
      template <typename T>
      T* getComponentT(void)
      {
        TypeID type = TYPE_ID(T);
        if(m_components.find(type)!=m_components.end())
          return SCAST(T*, m_components[type]);
        else
          return NULL;
      };
      
      Text getInstanceName(void) const;
      Text getArchetype(void) const;
      ObjectID getID(void) const;

      void destroy(void);
      Object* clone(void);
      

      //Replacement for transform component
      PROPERTY(vec3, scale);
      PROPERTY(vec3, position);
      PROPERTY(Quat, rotation);
      //for script
      ScriptVec3 getPosition(void) const;
      ScriptVec3 getScale(void) const;
      Quaternion getRotation(void) const;
      void setPosition(ScriptVec3 const& pos);
      void setScale(ScriptVec3 const& scale);
      void setRotation(Quaternion const& rot);

      void scriptAddBehavior(Text const& name);
      void scriptRemoveBehavior(Text const& name);

      void addBehavior(BehaviorID const& name);
      void addBehavior(Behavior* behavior);
      void removeBehavior(BehaviorID const& name);
      void removeBehavior(Behavior* behavior);
      void removeAllBehaviors(void);
      bool hasBehavior(BehaviorID const& name);

      void addComponent(Component const& source);
      void removeComponent(TypeID const& id);
      void removeComponent(Component* component);
      void removeAllComponents(void);
      void detachMeFromObject( Object* obj );
      void attachObjectToMe( Object* obj );
      void linkToParent( BoneBinding& link );
      
      void scriptCall(Text const& message, RawStore const& data);
      void scriptCallSpecial(Text const& message);
      void REQ_CMP( Text const& component );
      void REQ_VAL(Text const& varaible, Text const& type );


      //these methods are called during special object events
      //they call the corresponding script function for all
      //attached behaviors
      void invoke_create(void); //script: on::create(obj)
      void invoke_destroy(void); //script: on::destroy(obj)
      void invoke_attach( Behavior* behavior ); //script: on::atach(obj)
      void invoke_attachAll(void); //same as invoke_attach except for all scripts at once
      void invoke_detach( Behavior* behavior ); //script: on::detach(obj)
      void invoke_addComponent(Text const& component); //script: on::addComponent(obj, compName)
      void invoke_removeComponent(Text const& component); //script: on::removeComponent(obj, compName)

    private:
      friend class Factory;
      friend class Level;
      Object(void);//not implemented.
      Object(ObjectID id, Text const& inst, Archetype const& arch);//only factory can create an object

      void serialize(Object const& source);
      void serialize(Scope & s, bool read);

      //listen or ignore messages on behalf of a script being added/removed
      void listenFor(Behavior* b);
      void ignoreFor(Behavior* b);

      typedef std::map<TypeID, Component*> ComponentMap;//Object OWNS component memory
      typedef std::map<BehaviorID, Behavior*> BehaviorMap;//Object DOES NOT OWN behavior memory
      typedef std::list<Behavior*> BehaviorList;//list of behaviors that implement an event
      typedef std::map<hstring, BehaviorList> EventListenMap;//a list of behaviors for each event function
      ObjectID m_id;
      vec3 m_position;
      vec3 m_scale;
      Quat m_rotation;

      Text m_instanceName;
      Archetype m_archetype;

      ComponentMap m_components;
      BehaviorMap m_behaviors;
      EventListenMap m_eventListeners;

      //So this is if we attach an object to something, or objects are attached to us.
      BoneBinding parentLink;
      std::vector<BoneBinding> children;

      #ifdef OBJ_DEBUG_SCRIPT_ENABLED
        public: Behavior* m_currentScript;
        public: Text m_currentScriptFunction;
      #endif
      
      bool m_iterInvalid;
      bool m_silentDestroy;
    };
};//Virtus namespace
#endif //VIRTUS_OBJECT_H
