/*! ====================================================================================================================
  @file component.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jul.1.2011

  @brief 

  How to create a new component called "Awesome":
  - in component_awesome.h Inherit from Component
  - in component_awesome.h friend Factory
  - in component_awesome.h declare a private static hstring named cmpType
  - in component_awesome.h declare a private static function named create, use signature: Component* create(void)
  - in component_awesome.cpp initialize cmpType to H("Awesome")
  - in component_awesome.cpp define the create method. This method must allocate an Awesome, then return it as a Component
  - in factory.cpp add #include "componenet_awesome.h"
  - in factory.cpp::vLoadSystem(void) add a call to defineComponent
    + Example: defineComponent(Awesome::cmpType, Awesome::create);


  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_COMPONENT_H
#define VIRTUS_COMPONENT_H


#define DEFINE_COMPONENT(cmp) gFactory->defineComponent( #cmp, gCore->getTypeID<cmp>(), cmp##::create )

namespace Grimelins
{

    typedef unsigned long ComponentID;

    class Object;

    class Component : public IListener
    {
    public://ANT section
      ANT__Bar(bar);

    public://Virtual interface
      virtual ~Component(void){};
      virtual void addRegistry(void)=0;
      virtual void remRegistry(void)=0;
      virtual Text getTypeName(void) const=0;
      virtual TypeID getTypeID(void) const=0;
      virtual void serialize( Scope& data, bool read )=0;
      virtual void serialize( Component const* source)=0;
      virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp)=0;

    public://Built In
      ComponentID getInstanceID(void) { return m_id;};
      Object* parent(void) const{return m_parent;};

    protected:
      friend class Factory;
      friend class Object;
      ComponentID m_id;
      Object* m_parent;
    };

     typedef Component* (*componentCreator)(void);

/* =====================================================================
 *  Use at the top of the implementation of serialize() for components.
 *  The name of the class being serialized is the only argument.
 *
 *  className - The name of the class implementing serialize()
 *
 *  Calling example: SERIAL_BeginSerializing(Foo);
 * =====================================================================*/
#define SERIAL_BeginSerializing(className)  \
  Scope* cScope= read ? &data : new Scope();    \
  if(!read){                                    \
    cScope->name=#className;                    \
    cScope->parent=&data;                       \
    data.children.push_back(cScope);            \
  }do{}while(0)

/* =====================================================================
 *  Use once for each variable that can be serialized. This macro must
 *  be called inside of the scope it should serialize in. The
 *  Serialize(...) method is the classes root scope. Use the macro
 *  SERIAL_OpenSubScope(...) to declare a scope, then use this macro
 *  to define serializing variable in that scope.
 *
 *  T - The data type of the data being serialized. WARNING: Only the
 *  types listed below can be serialized.
 *
 *  N - The C style string name of the variable.
 *
 *  D - The data-member or variable to be read from or written to.
 *
 *  Calling example: SERIAL_Var(f32, "hitpoints", m_health);
 *
 * =====================================================================
 * Allowable types are those defined in Scope struct (fileparser.h).
 * ch, Text, u1, u8, s8, s16, u16, s32, u32, s64, u64, f32, f64, vec2, vec3, vec4, Quat
 * =====================================================================*/
#define SERIAL_Var(T, N, D)                             \
  if(read){if(cScope){                                  \
    Scope::##T##It it = cScope->##T##_.begin();         \
    for(;it!=cScope->##T##_.end(); ++it){               \
      if(it->name == N) {D = it->data; break;}}}}       \
  else{cScope->##T##_.push_back(Variable<T>(D, N));}do{}while(0)

/* =====================================================================
 *  Use to declare a nested scope within your class serialization.
 *  Once declared, following calls to SERIAL_Var(...) will associate with
 *  this scope. BE SURE TO CALL SERIAL_CloseSubScope() when done.
 *
 *  nm - The C style string name of the scope itself.
 *
 *  Calling example:
 *  SERIAL_OpenSubScope("nested");
 *  //SERIAL_Var calls...
 *  SERIAL_CloseSubScope();
 * =====================================================================*/
#define SERIAL_OpenSubScope(nm){                          \
  Scope* pScope = cScope;                                 \
  if(read){cScope=NULL;                                   \
    if(pScope->children.size()){                          \
      Scope::ScopeList::const_iterator nexit =            \
                              pScope->children.begin();   \
      for(;nexit!=pScope->children.end(); ++nexit){       \
      if((*nexit)->name == nm){ cScope = *nexit; }}}}     \
  else{cScope = new Scope();                              \
    cScope->name = nm;                                    \
    cScope->parent = pScope;                              \
    pScope->children.push_back(cScope);}do{}while(0)

#define SERIAL_CloseSubScope() cScope = pScope;}


}//namespace Virtus

#endif