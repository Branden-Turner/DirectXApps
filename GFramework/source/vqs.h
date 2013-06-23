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
#ifndef VQS_H
#define VQS_H

namespace cs350
{
    class VQS
    {
    public:

      VQS();
      VQS(vec3 &trans, quat &rot, float s);
      vec3 translation_;
      quat rotation_;
      float scale_;
      void ToMatrix(mat4& mat);
      void Interpolate(const VQS &v1, const VQS &v2, float t);

      VQS operator +(const VQS &rhs) const;
      VQS operator *(const VQS &rhs) const;
      vec3 operator *(const vec3 &rhs) const;
      
      VQS& operator +=(const VQS &rhs);
      VQS& operator *=(const VQS &rhs);

      VQS operator *( float scalar );
    };
}

#endif