/*! ====================================================================================================================
  @file jacobi1x6.h

  Codename Grimelins

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Nov.18.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_JACOBI1X6_H
#define VIRTUS_JACOBI1X6_H

#ifndef JAC6INLINE
  #if(VIRTUS_CONFIG_DEBUG_MODE)
    #define JAC6INLINE inline
  #else
    #define JAC6INLINE __forceinline
  #endif
#endif

namespace Grimelins
{
  // This matrix is mainly for row operations within the 6x6 matrix
	struct Jacobi1x6 
	{
    union
    {
      struct { f32 a,b,c,d,e,f; };
      f32 v[6];
    };

    JAC6INLINE Jacobi1x6();
    JAC6INLINE Jacobi1x6(f32 aa, f32 bb, f32 cc, f32 dd, f32 ee, f32 ff);
    JAC6INLINE Jacobi1x6& operator=(const Jacobi1x6& mat);
    JAC6INLINE Jacobi1x6 operator+(const Jacobi1x6& mat) const;
    JAC6INLINE Jacobi1x6 operator-(const Jacobi1x6& mat) const;
    JAC6INLINE Jacobi1x6 operator*(f32 value) const;
    JAC6INLINE Jacobi1x6 operator*(const Matrix6& mat) const;
    JAC6INLINE f32 operator*(const Vector6& vec) const;

    JAC6INLINE Vector6 transpose() const;
	};

  JAC6INLINE Jacobi1x6 operator*(f32 value, const Jacobi1x6& mat);
}; // namespace Grimelins

#include "jacobi1x6.inl"

#endif // VIRTUS_JACOBI1X6_H