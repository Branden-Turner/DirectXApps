/**********************************************************************/
/*
  
  \file
    vqs.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This is going to be our VQS for incremental interpolation.
    
*/
/**********************************************************************/
#ifndef VIRTUS_VQS_H
#define VIRTUS_VQS_H

#include "animquaternion.h"

namespace Grimelins
{
    class VQS
    {
    public:

      VQS();
      VQS(vec3 &trans, AnimQuaternion &rot, f32 s);
      vec3 translation_;
      AnimQuaternion rotation_;
      f32 scale_;
      void ToMatrix(Matrix4& mat);
      void Interpolate(const VQS &v1, const VQS &v2, f32 t);

      VQS operator +(const VQS &rhs) const;
      VQS operator *(const VQS &rhs) const;
      vec3 operator *(const vec3 &rhs) const;
      
      VQS& operator +=(const VQS &rhs);
      VQS& operator *=(const VQS &rhs);

      VQS operator *( f32 scalar );
    };
}

#endif VIRTUS_VQS_H