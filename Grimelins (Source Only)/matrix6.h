/*! ====================================================================================================================
  @file matrix6.h

  Codename Grimelins

  @author Jodi Manela <jodi.manela@digipen.edu>
  @date Nov.7.2011

  @brief 
    This matrix class will be used in calculating the LCP, 6 degrees of freedom

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_MATRIX6_H
#define VIRTUS_MATRIX6_H

#ifndef MTX6INLINE
# if(DEBUG_MODE)
#   define MTX6INLINE inline
# else
#   define MTX6INLINE inline
# endif
#endif


namespace Grimelins
{
	struct Vector6;

  struct Matrix6
  {
    union
    {
      struct 
      {
        f32 m00, m01, m02, m03, m04, m05,
            m06, m07, m08, m09, m10, m11,
            m12, m13, m14, m15, m16, m17,
            m18, m19, m20, m21, m22, m23,
            m24, m25, m26, m27, m28, m29,
            m30, m31, m32, m33, m34, m35;
      };

      f32 m[6][6];
      f32 v[36];
    };

    MTX6INLINE Matrix6(void);
    MTX6INLINE Matrix6(const Matrix6& rhs);
    MTX6INLINE Matrix6(f32 mm00, f32 mm01, f32 mm02, f32 mm03, f32 mm04, f32 mm05,
                       f32 mm10, f32 mm11, f32 mm12, f32 mm13, f32 mm14, f32 mm15,
                       f32 mm20, f32 mm21, f32 mm22, f32 mm23, f32 mm24, f32 mm25,
                       f32 mm30, f32 mm31, f32 mm32, f32 mm33, f32 mm34, f32 mm35,
                       f32 mm40, f32 mm41, f32 mm42, f32 mm43, f32 mm44, f32 mm45,
                       f32 mm50, f32 mm51, f32 mm52, f32 mm53, f32 mm54, f32 mm55);

    MTX6INLINE Matrix6& operator=(const Matrix6& rhs);

    MTX6INLINE Vector6 operator*(const Vector6& rhs) const;

    MTX6INLINE Matrix6 operator+(const Matrix6& rhs) const;
    MTX6INLINE Matrix6 operator-(const Matrix6& rhs) const;
    MTX6INLINE Matrix6 operator*(const f32 value) const;

    MTX6INLINE Matrix6 getTranspose();
    MTX6INLINE Matrix6 identity();
  };

  typedef Matrix6 mat6;
  static const Matrix6 mMatrix6Identity(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

}; // namespace Grimelins


#include "matrix6.inl"

#endif // VIRTUS_MATRIX6_H

