/*! ====================================================================================================================
  @file vector6.h

  Codename Grimelins

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Nov.15.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_VECTOR6_H
#define VIRTUS_VECTOR6_H

#ifndef VEC6INLINE
  #if(VIRTUS_CONFIG_DEBUG_MODE)
    #define VEC6INLINE inline
  #else
    #define VEC6INLINE __forceinline
  #endif
#endif

namespace Grimelins
{
  struct Vector6
  {
    union
    {
      struct { f32 a,b,c,d,e,f; };
      f32 v[6];
    };

    VEC6INLINE Vector6(void);
    VEC6INLINE Vector6(f32 aa, f32 bb, f32 cc, f32 dd, f32 ee, f32 ff);

    VEC6INLINE Vector6& operator=(const Vector6& rhs);

    VEC6INLINE Vector6 operator+(const Vector6& rhs) const;
    VEC6INLINE Vector6 operator-(const Vector6& rhs) const;
    VEC6INLINE Vector6 operator*(f32 rhs) const;
  };

  typedef Vector6 vec6;

}; // namespace Grimelins

#include "vector6.inl"

#endif // VIRTUS_VECTOR6_H