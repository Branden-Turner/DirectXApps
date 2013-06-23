#ifndef GRIMELIN_BOUNDTYPE_H
#define GRIMELIN_BOUNDTYPE_H

namespace Grimelins 
{
  enum NarrowPhaseType
  {
    Obb,
    BoundingSphere,
    numNarrowTypes
  };

  enum CollisionType
  {
    AnyCollisionType,
    Player1             = 1<<1,
    Boss                = 1<<2,
    DamageSource        = 1<<3,
    Environment         = 1<<4,
    CollisionTypeCount  = 4,
    AllCollisionTypes   = Player1 | Boss | DamageSource | Environment
  };
}

#endif