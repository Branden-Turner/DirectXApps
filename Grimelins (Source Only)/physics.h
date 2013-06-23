/*! ====================================================================================================================
  @file physics.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.9.2011

  @brief 
    Implementation of Physics class, holds all the bodies in the physics world, applies constraints

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_PHYSICS_H
#define VIRTUS_PHYSICS_H
#include "component.h"

namespace Grimelins
{
  class Integrator;
  class Body;
  class ProjectileComponent;
  class Constraint;   
  class Collision;

  // typedef std::vector<CollisionPoint *> CollPntList;
  SCRIPT_HeaderDefine(Physics);

  //------------------------------------------------
  // Class Physics
  // This is the physics manager for the game
  //------------------------------------------------
  class Physics: public ISystem, public IListener
  {
  public:
    Physics(void); // constructor
    virtual ~Physics(void); // destructor

    //-------------------------------------------
    // ISystem interface
    //-------------------------------------------
    virtual void vLoadSystem();
    virtual void vUpdateSystem();
    virtual void vUnloadSystem();
    virtual hstring vGetSystemID() const { return mIDName; }
    virtual void vRegisterComponent(TypeID id, Component* component);
    virtual void vUnregisterComponent(TypeID id, Component* component);

    //--------------------------------------------
    // Functions
    //--------------------------------------------
    typedef std::vector<Body*> BodyList;
    typedef BodyList::iterator BodyIt;
    void addBody(Body* body);                 // add a body to the physics world
    void removeBody(Body const* const body);  // remove a body from the physics world
    void clearBodies();                       // clear the addBodies and removedBodies sets
    BodyList& getAddedBodies() { return m_addedBodies; }      // getter of the list
    BodyList& getRemovedBodies() { return m_removedBodies; }  // getter of the list

    // moving our object
    void moveCCForward(Object& obj, f32 speed);
    void strafeCC( Object& obj, f32 speed );
    Body* getFirstOfColType( CollisionType type);
    Body* getBodyWithinDistOfType( Body* body, CollisionType type, f32 sqDist);
    Body* getClosestBodyOfType( Body* body, CollisionType type );

    // constraints
    void addConstraint(Constraint* constraint);    // add a constraint into the physics world
    void removeConstraint(Constraint* constraint); // remove a constraint and free its memory 
    void removeAllCollPtConstraints();             // remove all collision point constraints
    void removeAllConstraints();                   // remove all constraints and delete them
    std::vector<Constraint*>::iterator getConstraintBegin() { return m_constraints.begin(); }  // constraint begin iterator
    std::vector<Constraint*>::iterator getConstraintEnd() { return m_constraints.end(); }      // constraint end iterator
      
  private:
    //--------------------------------------------
    // Data
    //--------------------------------------------
    BodyList m_bodies;              //list that contains all bodies of the physics world
    BodyList m_addedBodies;         // added bodies since last update
    BodyList m_removedBodies;       // removed bodies since last update
    std::vector<Constraint*> m_constraints; // list that contains all the current constraints
    std::vector<Constraint*>::iterator it;  // constraint iterator
    Integrator* m_integrator;
    Collision* m_collider;
      
    //----------------------------------------------
    // ISubscriber interface
    //----------------------------------------------
    hstring mIDName;
    virtual u1 handleEvent(hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp);
    typedef std::map<ComponentID, Body*> BodyComponentRegistry;
    BodyComponentRegistry m_bodyRegistry;

  };
}; // namespace Virtus

#endif // VIRTUS_PHYSICS_H