/*! ====================================================================================================================
  @file component_body.cpp

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.16.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "compbody.h"
#include "boundingvolume.h"
#include "obb.h"
#include "comptransform.h"
#include "object.h"
#include "physics.h"

namespace Grimelins
{
  /////// SCRIPT BINDS
  //// Body
  SCRIPT_ClassStart(Body);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(getTypeName);
  SCRIPT_Method(getTypeID);
  SCRIPT_Method(IsMotionEnabled);
  SCRIPT_Method(setIsMotionEnabled);
  SCRIPT_Method(IsCollisionEnabled);
  SCRIPT_Method(setIsCollisionEnabled);
  SCRIPT_Method(setCollisionDebug);
  SCRIPT_Method(isColliding);
  SCRIPT_Method(getOrientation);
  SCRIPT_Method(setOrientation);
  SCRIPT_Method(collidingWith);
  //SCRIPT specialized functions
  //none
  SCRIPT_ClassEnd();

    Component* Body::create( void )
    {
      return SCAST(Component*, new Body());
    }

    Body::Body(void): m_aabb(0), m_obbBV(0), m_obb(0), m_isColliding(false)
    {
    }

    Body::~Body( void )
    {
      remRegistry();
      if(m_aabb) delete m_aabb;
      if(m_obbBV) delete m_obbBV;
    }

    void Body::setOrientation(Quaternion const &setval)
    { 
//      m_current.m_orientation = setval;
//      vec3 cetner = m_obb->getCenter();
//       m_obb->update(cetner, setval);
//       m_aabb->update(cetner, setval);
    }

    Quaternion Body::getOrientation() 
    { 
      return m_current.m_orientation; 
    }

    Text Body::getTypeName( void ) const
    {
      return "Body";
    }

    void Body::addRegistry( void )
    {
      PHYSICS->vRegisterComponent(getTypeID(), this);
    }

    void Body::remRegistry( void )
    {
      PHYSICS->vUnregisterComponent(getTypeID(), this);
    }

    TypeID Body::getTypeID( void ) const
    {
      return TYPE_ID(Body);
    }

    void Body::serialize( Scope& data, bool read )
    {
      SERIAL_BeginSerializing(Body);

      SERIAL_Var(u1, "motionEnabled",     m_isMotionEnabled);
      SERIAL_Var(u1, "collisionEnabled",  m_isCollisionEnabled);
      SERIAL_Var(NarrowPhaseType, "narrowType", m_narrowType);
      SERIAL_Var(CollisionType, "coltype", m_collisionType);

      SERIAL_OpenSubScope("previous");
        SERIAL_Var(f32,   "mass",           m_previous.m_mass);
        if(read)
        {
          if(cScope)
          {
            Scope::vec3It it = cScope->vec3_.begin();
            vec3 scale;
            for(;it!=cScope->vec3_.end(); ++it)
            {
              if(it->name == "scale")
              {
                scale = it->data;
                m_previous.m_xSize = scale.x;
                m_previous.m_ySize = scale.y;
                m_previous.m_zSize = scale.z;
                break;
              }
            }
          }
        }
        else
        {
          cScope->vec3_.push_back(Variable<vec3>(vec3(m_previous.m_xSize, m_previous.m_ySize, m_previous.m_zSize), "scale"));
        }
        SERIAL_Var(f32,   "invMass",        m_previous.m_invMass);
        SERIAL_Var(f32,   "inerTensor",     m_previous.m_inerTensor);
        SERIAL_Var(f32,   "invInerTensor",  m_previous.m_invInerTensor);
        SERIAL_Var(f32,   "static",         m_previous.m_us);
        SERIAL_Var(f32,   "drag",           m_previous.m_ud);
        SERIAL_Var(vec3,  "position",       m_previous.m_position);
        SERIAL_Var(vec3,  "momentum",       m_previous.m_momentum);
        SERIAL_Var(vec3,  "deltaPos",       m_previous.m_deltaPos);
        SERIAL_Var(vec3,  "angMomentum",    m_previous.m_angMomentum);
        SERIAL_Var(vec3,  "angVelocity",    m_previous.m_angVelocity);
        SERIAL_Var(Quat,  "orientation",    m_previous.m_orientation);
        SERIAL_Var(Quat,  "spin",           m_previous.m_spin);
      SERIAL_CloseSubScope();

      SERIAL_OpenSubScope("current");
        SERIAL_Var(f32,   "mass",           m_current.m_mass);
        if(read)
        {
          if(cScope)
          {
            Scope::vec3It it = cScope->vec3_.begin();
            vec3 scale;
            for(;it!=cScope->vec3_.end(); ++it)
            {
              if(it->name == "scale")
              {
                scale = it->data;
                m_current.m_xSize = scale.x;
                m_current.m_ySize = scale.y;
                m_current.m_zSize = scale.z;
                break;
              }
            }
          }
        }
        else
        {
          cScope->vec3_.push_back(Variable<vec3>(vec3(m_current.m_xSize, m_current.m_ySize, m_current.m_zSize), "scale"));
        }        
        SERIAL_Var(f32,   "invMass",        m_current.m_invMass);
        SERIAL_Var(f32,   "inerTensor",     m_current.m_inerTensor);
        SERIAL_Var(f32,   "invInerTensor",  m_current.m_invInerTensor);
        SERIAL_Var(f32,   "static",         m_current.m_us);
        SERIAL_Var(f32,   "drag",           m_current.m_ud);
        SERIAL_Var(vec3,  "position",       m_current.m_position);
        SERIAL_Var(vec3,  "momentum",       m_current.m_momentum);
        SERIAL_Var(vec3,  "deltaPos",       m_current.m_deltaPos);
        SERIAL_Var(vec3,  "angMomentum",    m_current.m_angMomentum);
        SERIAL_Var(vec3,  "angVelocity",    m_current.m_angVelocity);
        SERIAL_Var(Quat,  "orientation",    m_current.m_orientation);
        SERIAL_Var(Quat,  "spin",           m_current.m_spin);
      SERIAL_CloseSubScope();

      createBoundingVolumes();
      clearCollisions();

    }

    void Body::serialize( Component const* source )
    {
      Body const* rhs = SCAST(Body const*, source);

      //Copy over the state data
      m_previous = rhs->m_previous;
      m_current = rhs->m_current;
      m_isMotionEnabled = rhs->m_isMotionEnabled;
      m_isCollisionEnabled = rhs->m_isCollisionEnabled;
      m_narrowType = rhs->m_narrowType;
      m_collisionType = rhs->m_collisionType;

      //Create the bounding volumes
      createBoundingVolumes();
      clearCollisions();
    }

    void Body::createBoundingVolumes()
    {
      if(m_obb)
        delete m_obb;
      if(m_aabb)
        delete m_aabb;

      f32 extSizeX = m_current.m_xSize/2;
      f32 extSizeY = m_current.m_ySize/2;
      f32 extSizeZ = m_current.m_zSize/2;

      //Create the bounding volumes.
      switch(m_narrowType)
      {       
        case Obb:
          m_obb = new OBB(m_current.m_position, vec3(1,0,0), vec3(0,1,0), vec3(0,0,1), 
                               extSizeX, extSizeY, extSizeZ);
          m_obbBV = RCAST(BoundingVolume*, m_obb);
        break;

        case BoundingSphere:
        break;

        default:
        break;
      }

      m_aabb = RCAST(BoundingVolume*, m_obb->computeAABB());

      m_obbBV->setBodyPointer(this);
      m_aabb->setBodyPointer(this);

      m_aabb->update(m_current.m_position, m_current.m_orientation);
      m_obbBV->update(m_current.m_position, m_current.m_orientation); 
    }

    Grimelins::u1 Body::handleEvent( hstring const& message, RawStore const& args, MessageType type, f32 timestamp )
    {
      MSG_StartHandling();
      MSG_DoneHandling();
    }


    bool Body::IsMotionEnabled() const
    {
      return m_isMotionEnabled;
    }

    void Body::setIsMotionEnabled(bool isEnabled)
    {
      m_isMotionEnabled = isEnabled;
    }

    bool Body::IsCollisionEnabled() const
    {
      return m_isCollisionEnabled;
    }

    void Body::setIsCollisionEnabled(bool isEnabled)
    {
      m_isCollisionEnabled = isEnabled;
    }

    // update the bounding volumes
    void Body::updateBV()
    {
      m_aabb->update(m_current.m_position, m_current.m_orientation);
      m_obbBV->update(m_current.m_position, m_current.m_orientation);
    }

    // set the broad-phase bounding volume
    void Body::setAABB(BoundingVolume* broadBV)
    {
      m_aabb = broadBV;
      m_aabb->setBodyPointer(this);
    }

    // set the narrow-phase bounding volume
    void Body::setOBB(BoundingVolume* narrowBV)
    {
      m_obb = RCAST(OBB*, narrowBV);
      m_obbBV = narrowBV;
      m_obbBV->setBodyPointer(this);
    }

    void Body::SetTransform()
    {
      Transform* trans = m_parent->getComponentT<Transform>();
      if(trans)
      {
        trans->position = m_current.m_position;
        trans->rotation = m_current.m_orientation;
        trans->scale = vec3(m_current.m_xSize, m_current.m_ySize, m_current.m_zSize);
      }
    }

}; // namespace Virtus