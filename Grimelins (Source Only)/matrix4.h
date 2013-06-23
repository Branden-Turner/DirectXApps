/*! ====================================================================================================================
  @file matrix4.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.16.2011

  @brief
    Matrix 4x4

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

#ifndef VIRTUS_MATRIX4_H
#define VIRTUS_MATRIX4_H

#ifndef MTX4INLINE
# if(DEBUG_MODE)
#   define MTX4INLINE inline
# else
#   define MTX4INLINE inline
# endif
#endif


namespace Grimelins { 

struct Vector4;

struct Matrix4
{
  union
  {
    struct  
    {
      f32 m00, m01, m02, m03,
          m10, m11, m12, m13,
          m20, m21, m22, m23,
          m30, m31, m32, m33;
    };
  
    f32 m[4][4];
    f32 v[16];
  };

  MTX4INLINE Matrix4(void);
  MTX4INLINE Matrix4(const Matrix4& rhs);
  MTX4INLINE Matrix4(f32 mm00, f32 mm01, f32 mm02, f32 mm03,
                     f32 mm10, f32 mm11, f32 mm12, f32 mm13,
                     f32 mm20, f32 mm21, f32 mm22, f32 mm23,
                     f32 mm30, f32 mm31, f32 mm32, f32 mm33);
  
  MTX4INLINE Matrix4& operator=(const Matrix4& rhs);

  //Copy constructor for scripted matrices
  MTX4INLINE Matrix4(const ScriptMatrix4& rhs);
  
  MTX4INLINE Vector4 operator*(const Vector4& rhs) const;
  MTX4INLINE Vector3 MulVec(const Vector3& rhs) const;
  MTX4INLINE Vector3 MulPnt(const Vector3& rhs) const;

  MTX4INLINE Matrix4 operator+(const Matrix4& rhs) const;
  MTX4INLINE Matrix4 operator-(const Matrix4& rhs) const;
  MTX4INLINE Matrix4 operator*(const Matrix4& rhs) const;

  MTX4INLINE Matrix4& operator+=(const Matrix4& rhs);
  MTX4INLINE Matrix4& operator-=(const Matrix4& rhs);
  MTX4INLINE Matrix4& operator*=(const Matrix4& rhs);

  MTX4INLINE Matrix4& SetTranspose(void);
  MTX4INLINE Matrix4& SetZero(void);
  MTX4INLINE Matrix4& SetIdentity(void);

  MTX4INLINE Matrix4  Transposed(void) const;
  MTX4INLINE Matrix4  Inverse(void) const;

  MTX4INLINE Vector3 GetTranslation() const;

  static const Matrix4& GetIdentity(void);

  
  static Matrix4 Scale(f32 uniformScale);
  static Matrix4 Scale(f32 xx, f32 yy, f32 zz);
  static Matrix4 Scale( Vector3 const& sca );
  static Matrix4 Translation( Vector3 const& pos );
  static Matrix4 Translation(f32 xx, f32 yy, f32 zz);
  static Matrix4 RotationX(f32 angle);
  static Matrix4 RotationY(f32 angle);
  static Matrix4 RotationZ(f32 angle);
  static Matrix4 RotationAxis( Vector3 const& vec, f32 angle, const Vector3& pos = Vector3() );
  static Matrix4 AlignVector( Vector3 const& ri, Vector3 const& up, Vector3 const& di );
  static Matrix4 AlignVectorTranspose( Vector3 const& ri, Vector3 const& up, Vector3 const& di );
  static Matrix4 AlignVectorCamera( const Vector3& ri, const Vector3& up, const Vector3& di, const Vector3& ey );
  static Matrix4 LookAtMatrix( Vector3 const& eye, Vector3 const& pos, Vector3 const& up );
  static Matrix4 PerspectiveFOV( f32 aspectRatio, f32 halfFov, f32 nearP, f32 farP );
  static Matrix4 Orthographic( f32 width, f32 height, f32 nearP, f32 farP );
};

typedef Matrix4 float4x4;
typedef Matrix4 mat4;

static const Matrix4 mMatrix4Identity(1.0f, 0.0f, 0.0f, 0.0f,
                                      0.0f, 1.0f, 0.0f, 0.0f,
                                      0.0f, 0.0f, 1.0f, 0.0f,
                                      0.0f, 0.0f, 0.0f, 1.0f);




} // namespace Virtus

#include "matrix4.inl"

#endif // Include Guard