/*! ====================================================================================================================
  @file component_body.h

  Codename Virtus

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Sep.16.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_COMPONENT_BODY_H
#define VIRTUS_COMPONENT_BODY_H
#include "component.h"
#include "object.h"
#include "boundingtypes.h"

namespace Grimelins
{
    class BoundingVolume;
    class OBB;

    SCRIPT_HeaderDefine(Body);
    struct State {
      State(void)
        :m_size(0.0f), m_mass(0.0f),m_invMass(0.0f),m_inerTensor(0.0f),m_invInerTensor(0.0f) {}
      f32 m_size;									// Size
      f32 m_xSize;                // Size in the x direction
      f32 m_ySize;                // Size in the y direction
      f32 m_zSize;                // Size in the z direction
      f32 m_mass;                 // mass of object
      f32 m_invMass;              // converts momentum to velocity
      f32 m_inerTensor;           // for the inverse
      f32 m_invInerTensor;        // converts angular momentum to angular velocity
      vec3 m_position;						// Position
      vec3 m_velocity;            // Velocity
      vec3 m_momentum;            // Momentum
      vec3 m_deltaPos;            // change in position
      Quaternion m_orientation;   // Orientation
      Quaternion m_spin;          // rate of change in orientation
      vec3 m_angMomentum;         // angular momentum
      vec3 m_angVelocity;         // angular velocity
      f32 m_us;                   // coefficient of static friction
      f32 m_ud;                   // coefficient of drag
    };
    class Body : public Component
    {
     public:
      Body(void);
      virtual ~Body(void);

      //Component interface
      virtual void addRegistry( void );
      virtual void remRegistry( void );
      virtual Text getTypeName(void) const;
      virtual TypeID getTypeID(void) const;
      virtual void serialize( Scope& data, bool read );
      virtual void serialize( Component const* source);
      virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp);

      //Body Methods
      bool IsMotionEnabled() const;               // Return if the rigid body can move
      void setIsMotionEnabled(bool isEnabled);    // Set the value to true, if it can move
      bool IsCollisionEnabled() const;            // Return if the body can collide with other bodies
      void setIsCollisionEnabled(bool isEnabled); // Set the value to true, if it can collide
      void setCollisionDebug(bool collide = true) {m_isColliding = collide;}

      void updateBV();
      const BoundingVolume* getAABB()const { return m_aabb; }    // Return the broad-phase bounding volume
      const BoundingVolume* getOBB()const { return m_obbBV; }    // Return the narrow-phase bounding volume
      const State* getCurrentState()const { return &m_current; } // Return all the pertinent info
      const vec3* getPosition() const { return &(m_current.m_position); } // return the position
      CollisionType getColType() const { return m_collisionType; };
      const vec3& getPos() const { return m_current.m_position; };
      void setPos(vec3& pos) { m_current.m_position = pos; };
      void setXScale(f32 scale) { m_current.m_xSize = scale; };
      void setYScale(f32 scale) { m_current.m_ySize = scale; };
      void setZScale(f32 scale) { m_current.m_zSize = scale; };
      f32 getXScale() { return m_current.m_xSize; };
      f32 getYScale() { return m_current.m_ySize; };
      f32 getZScale() { return m_current.m_zSize; };
      void setOrientation(Quaternion const &setval);
      Quaternion getOrientation();
      void createBoundingVolumes();

      //game logic mandates this functionality...
      void clearCollisions(void) {m_collisionFlags = 0;};
      void addCollision(int flag) { m_collisionFlags |= flag;};
      bool collidingWith(int flag) { return (flag==AnyCollisionType)  ?  (m_collisionFlags != 0)  :  ((m_collisionFlags&flag) == flag) ; };
      bool isColliding(void){return m_isColliding;};

    private://Associates and creator
      friend class Factory;
      friend class Integrator;
      friend class Integrate_RK4;
      friend class Physics;
      friend class Object;
      friend class ProjectileComponent;
      friend class Level;
      static Component* create( void );
      void SetTransform();
    private:
      State m_current;
      State m_previous;

      bool m_isMotionEnabled;     // True if the body can move
      bool m_isCollisionEnabled;  // True if the body can collide with other bodies
      bool m_isColliding;   // for debug purposes, this body is colliding

      BoundingVolume* m_aabb;   // Bounding volume used in broad-phase
      BoundingVolume* m_obbBV;  // Bounding volume used in narrow-phase
      OBB* m_obb;               // This is solely used to get the AABB from the OBB

      NarrowPhaseType m_narrowType;
      CollisionType m_collisionType;

      int m_collisionFlags;

      void setAABB(BoundingVolume* broadBV); // Set the broad-phase bounding volume
      void setOBB(BoundingVolume* narrowBV); // Set the narrow-phase bounding volume
      

    };// class

}; // namespace Virtus

#endif // VIRTUS_COMPONENT_BODY_H