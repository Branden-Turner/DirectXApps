/*! ====================================================================================================================
  @file physics.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.16.2011

  @brief 
    Implementation of Physics class

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "physics.h"
#include "collision.h"
#include "compbody.h"
#include "integrator.h"
#include "integratorrk4.h"
#include "graphics.h"
#include "cmpproj.h"
#include "constraints.h"
#include "obb.h"

namespace Grimelins
{
  SCRIPT_ClassStart(Physics);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(moveCCForward);
  SCRIPT_Method(strafeCC);
  SCRIPT_ClassEnd();
  Physics* PHYSICS;

  Physics::Physics( void ) : mIDName(H("Physics")) 
  {

  }

  Physics::~Physics( void )
  {
    // remove our constraints
    removeAllConstraints();
  }

  void Physics::vLoadSystem()
  {
    // Loading integrator and telling the core!
    m_integrator = new Integrate_RK4();
    m_collider   = new Collision(this);

    gCore->Events.Subscribe(H("PHYS_NEWBDY"), this);
    gCore->Events.Subscribe(H("PHYS_DELBDYS"), this);
    SCRIPT_RegisterGlobal(Physics);

    ErrorIf(PHYSICS, "multiple instances of Physics");
    PHYSICS = this;
  }

  void Physics::vUpdateSystem()
  {
    if(gCore->GamePaused)
      return;

    // get the dt
    f32 dt = GetSystemDT();
     
    //clear collision flags used by gamelogic
    for(BodyList::iterator it = m_bodies.begin(); it != m_bodies.end(); ++it)
      (*it)->clearCollisions();

    //calculate new collisions
    m_collider->computeCollision();


    // iterate through the bodies
    for(BodyIt it = m_bodies.begin(); it != m_bodies.end(); ++it)
    {
      //Grab the body to work with
      Body& body = **it;
      vec3& pos1 = body.m_current.m_position;

      // integrate the bodies
      m_integrator->Integrate(body, dt);
      body.updateBV();
      body.SetTransform();

#ifdef _DEBUG
        
      // setting the position for the demo, this will not happen in general
      vec3& pos = body.m_current.m_position;
      u1 colliding = body.collidingWith(AnyCollisionType);
      f32 sizex = body.m_current.m_xSize;
      f32 sizey = body.m_current.m_ySize;
      f32 sizez = body.m_current.m_zSize;
      Quat& rot = body.m_current.m_orientation;
      if(colliding)
        GRAPHICS->DrawShape(pos, vec3(sizex, sizey, sizez), rot.matrix(), ShapeCube, Red, false);
      else
        GRAPHICS->DrawShape(pos, vec3(sizex, sizey, sizez), rot.matrix(), ShapeCube, Blue, false);

#endif
    }
      
  }

  void Physics::vUnloadSystem()
  {
    // clean up the pointers and bodies
    delete m_integrator;
    m_integrator = 0;
    BodyIt it = m_bodies.begin();
    for(; it != m_bodies.end(); ++it)
    {
      delete *it;
    }
    m_bodies.clear();

    PHYSICS = NULL;
  }

  u1 Physics::handleEvent(hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp)
  {
    // tell the system that we want a body!
    OnMessage(PHYS_NEWBDY)

    ConsumeMessage

    OnMessage(PHYS_DELBDYS)

    BodyIt it = m_bodies.begin();
    for(; it != m_bodies.end(); ++it)
    {
      delete *it;
    }
    m_bodies.clear();

    ConsumeMessage

    return false;
  }

  void Physics::addBody(Body* body)
  {
    BodyIt it;

    // see if the body is in the list
    it = std::find(m_bodies.begin(), m_bodies.end(), body);

    // The body value is on the list, we throw an exception
    ErrorIf(it != m_bodies.end(), "The body is already on the list");

    // its not on the list, so we add it
    m_bodies.push_back(body);
    m_addedBodies.push_back(body);

    // remove the body from the removed list if its there
    it = std::find(m_removedBodies.begin(), m_removedBodies.end(), body);
    if(it != m_removedBodies.end())
      m_removedBodies.erase(it);
  }

  void Physics::removeBody(Body const* const body)
  {
    BodyIt it;

    // if the body value is on the removed list, we throw an exception
    it = std::find(m_removedBodies.begin(), m_removedBodies.end(), body);
    ErrorIf((*it == body), "The body value is already in the removed list");
      
    // Otherwise, add it to the removed list
    m_removedBodies.push_back(*it);

    // remove the body from both of the other body lists
    it = std::find(m_bodies.begin(), m_bodies.end(), body);
    if(it != m_bodies.end())
      m_bodies.erase(it);

    it = std::find(m_addedBodies.begin(), m_addedBodies.end(), body);
    if(it != m_addedBodies.end())
      m_addedBodies.erase(it);
  }

  void Physics::clearBodies()
  {
    m_addedBodies.clear();
    m_removedBodies.clear();
  }

  void Physics::moveCCForward( Object& obj, f32 speed )
  {
      Body& body = *obj.getComponentT<Body>();
      gfxModel& model = *obj.getComponentT<gfxModel>();

    body.m_current.m_position += model.GetDirection() * speed * GetSystemDT();
  }

  static Timer strafeTimer(true);
  void Physics::strafeCC( Object& obj, f32 speed )
  {
    static Body& body = *obj.getComponentT<Body>();
    static gfxModel& model = *obj.getComponentT<gfxModel>();

    if(strafeTimer.Get() > 1.5f)
    {
      body.m_current.m_position += model.GetRight() * speed;
      strafeTimer.StartTimer();
    }
      
  }

  void Physics::addConstraint(Constraint* constraint)
  {
    ErrorIf(constraint == 0, "Constraint can't be zero");
    m_constraints.push_back(constraint);
  }

  void Physics::removeConstraint(Constraint* constraint)
  {
    ErrorIf(constraint == 0, "Constraint can't be zero");
    it = std::find(m_constraints.begin(), m_constraints.end(), constraint);
    // if its on the list
    if(*it == constraint)
    {
      delete *it;
      m_constraints.erase(it);
    }
    else // else return
      return;
  }

  void Physics::removeAllCollPtConstraints()
  {
    // loop through constraints
    for(it = getConstraintBegin(); it != getConstraintEnd(); ++it)
    {
      CollisionPoint* c_point = DCAST(CollisionPoint*,*it);

      // if this collision point exists, delete it!
      if(c_point)
      {
        delete (*it);
        it = m_constraints.erase(it);
      }
    }
  }

  void Physics::removeAllConstraints()
  {
    // delete all the constraints
    for(it = getConstraintBegin(); it != getConstraintEnd(); ++it)
      delete *it;

    // clear our container
    m_constraints.clear();
  }

  void Physics::vRegisterComponent( TypeID id, Component* component )
  {
    static const TypeID cBody = gCore->getTypeID<Body>();
      static const TypeID cProj = gCore->getTypeID<ProjectileComponent>();

    if(id == cBody)
    {
      Body* bodyComp = SCAST(Body*, component);
      ErrorIf(m_bodyRegistry.find(bodyComp->getInstanceID()) != m_bodyRegistry.end(), "Duplicate component registration");
      m_bodyRegistry[bodyComp->getInstanceID()] = bodyComp;
      addBody(bodyComp);
    }
//     else if(id == cProj)
//     {
//       ProjectileComponent* projComp = SCAST(ProjectileComponent*, component);
//       ErrorIf(m_projectileRegistry.find(projComp->getInstanceID()) != m_projectileRegistry.end(), "Duplicate component registration");
//       m_projectileRegistry[projComp->getInstanceID()] = projComp;
//     }
    else
      printDebug("Trying to register component Physics could care less about...");
  }

  void Physics::vUnregisterComponent( TypeID id, Component* component )
  {
    static const TypeID cBody = gCore->getTypeID<Body>();
      static const TypeID cProj = gCore->getTypeID<ProjectileComponent>();

    Body* bodyComp = NULL;

    if(id == cBody)
    {
      m_bodyRegistry.erase(component->getInstanceID());
      bodyComp = SCAST(Body*, component);
    }
//     else if(id == cProj)
//     {
//       m_projectileRegistry.erase(component->getInstanceID());
//     }
    else
      printDebug("Trying to unregister component Physics shouldn't know about...");

    //Remove this pointer from every single list in Physics
    if(bodyComp)
    {
      BodyIt it;

      if(m_bodies.size())
      {
        it = std::find(m_bodies.begin(), m_bodies.end(), bodyComp);
        if(it != m_bodies.end())
          m_bodies.erase(it);
      }

      if(m_addedBodies.size())
      {
        it = std::find(m_addedBodies.begin(), m_addedBodies.end(), bodyComp);
        if(it != m_addedBodies.end())
          m_addedBodies.erase(it);
      }

      if(m_removedBodies.size())
      {
        it = std::find(m_removedBodies.begin(), m_removedBodies.end(), bodyComp);
        if(it != m_removedBodies.end())
          m_removedBodies.erase(it);
      }

      if(m_collider)
      {
        if(m_collider->m_contacts.size())
        {
          u32 size = m_collider->m_contacts.size();

          for(u32 i = 0; i < size; ++i)
          {
            Contact* contact = m_collider->m_contacts.at(i);
            Body* b1 = contact->m_obb1->getBodyPointer();
            Body* b2 = contact->m_obb2->getBodyPointer();

            if(bodyComp == b1 || bodyComp == b2)
            {
              m_collider->m_contacts.erase(m_collider->m_contacts.begin() + i);
              --i;
              --size;
            }
          }
        }
      }
    }
  }


    Body* Physics::getFirstOfColType( CollisionType type )
    {
      for(u32 i = 0; i < m_bodies.size(); ++i)
        if(m_bodies[i]->m_collisionType == type)
          return m_bodies[i];

      return NULL;
    }

    Body* Physics::getBodyWithinDistOfType( Body* body, CollisionType type, f32 sqDist )
    {
      vec3 pos = body->getPos();

      for(u32 i = 0; i < m_bodies.size(); ++i)
      {
        if(m_bodies[i]->m_collisionType == type)
        {
          vec3 bodyPos = m_bodies[i]->m_current.m_position;

          f32 distSq = (bodyPos - pos).LengthSq();

          if(distSq < sqDist)
            return m_bodies[i];
        }
      }

      return NULL;
   }

    Body* Physics::getClosestBodyOfType( Body* body, CollisionType type )
    {
      Body* bRet = NULL;
      f32 sqDistMin = FLT_MAX;
      vec3 pos = body->getPos();

      for(u32 i = 0; i < m_bodies.size(); ++i)
      {
        if(m_bodies[i]->m_collisionType == type)
        {
          vec3 bodyPos = m_bodies[i]->m_current.m_position;

          f32 distSq = (bodyPos - pos).LengthSq();

          if(distSq < sqDistMin)
          {
            sqDistMin = distSq;
            bRet = m_bodies[i];
          }
            
        }
      }

      return bRet;
    }


}; // namespace Virtus