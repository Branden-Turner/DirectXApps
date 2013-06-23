/*! ====================================================================================================================
  @file scriptmatrix4.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.16.2011

  @brief
    Matrix 4x4

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIME_SCRIPTMATRIX4_H
#define GRIME_SCRIPTMATRIX4_H

namespace Grimelins { 

  struct Matrix4;

  SCRIPT_HeaderDefine(ScriptMatrix4);
  class ScriptMatrix4
  {
    public:
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

    ScriptMatrix4(void);
    ScriptMatrix4(const ScriptMatrix4& rhs);
    ScriptMatrix4(const Matrix4& rhs);
    ScriptMatrix4(f32 mm00, f32 mm01, f32 mm02, f32 mm03,
                  f32 mm10, f32 mm11, f32 mm12, f32 mm13,
                  f32 mm20, f32 mm21, f32 mm22, f32 mm23,
                  f32 mm30, f32 mm31, f32 mm32, f32 mm33);

    ScriptMatrix4& SetMatrix(f32 mm00, f32 mm01, f32 mm02, f32 mm03,
                             f32 mm10, f32 mm11, f32 mm12, f32 mm13,
                             f32 mm20, f32 mm21, f32 mm22, f32 mm23,
                             f32 mm30, f32 mm31, f32 mm32, f32 mm33);

    ScriptMatrix4& operator=(const ScriptMatrix4& rhs);

    ScriptVec4 operator*(const ScriptVec4& rhs) const;
    ScriptVec3 MulVec(const ScriptVec3& rhs) const;
    ScriptVec3 MulPnt(const ScriptVec3& rhs) const;

    ScriptMatrix4 operator+(const ScriptMatrix4& rhs) const;
    ScriptMatrix4 operator-(const ScriptMatrix4& rhs) const;
    ScriptMatrix4 operator*(const ScriptMatrix4& rhs) const;

    ScriptMatrix4& operator+=(const ScriptMatrix4& rhs);
    ScriptMatrix4& operator-=(const ScriptMatrix4& rhs);
    ScriptMatrix4& operator*=(const ScriptMatrix4& rhs);

    ScriptMatrix4& SetTranspose(void);
    ScriptMatrix4& SetZero(void);
    ScriptMatrix4& SetIdentity(void);

    ScriptMatrix4  Transposed(void) const;
    ScriptMatrix4  Inverse(void) const;

    ScriptVec3 GetTranslation() const;

    const ScriptMatrix4& GetIdentity(void);

    ScriptMatrix4 Scale(f32 uniformScale);
    ScriptMatrix4 Scale(f32 xx, f32 yy, f32 zz);
    ScriptMatrix4 Scale( ScriptVec3 const& sca );
    ScriptMatrix4 Translation( ScriptVec3 const& pos );
    ScriptMatrix4 Translation(f32 xx, f32 yy, f32 zz);
    ScriptMatrix4 RotationX(f32 angle);
    ScriptMatrix4 RotationY(f32 angle);
    ScriptMatrix4 RotationZ(f32 angle);
    ScriptMatrix4 RotationAxis( ScriptVec3 const& vec, f32 angle, const ScriptVec3& pos = ScriptVec3() );
    ScriptMatrix4 AlignVector( ScriptVec3 const& ri, ScriptVec3 const& up, ScriptVec3 const& di );
    ScriptMatrix4 AlignVectorTranspose( ScriptVec3 const& ri, ScriptVec3 const& up, ScriptVec3 const& di );
    ScriptMatrix4 AlignVectorCamera( const ScriptVec3& ri, const ScriptVec3& up, const ScriptVec3& di, const ScriptVec3& ey );
    ScriptMatrix4 LookAtMatrix( ScriptVec3 const& eye, ScriptVec3 const& pos, ScriptVec3 const& up );
    ScriptMatrix4 PerspectiveFOV( f32 aspectRatio, f32 halfFov, f32 nearP, f32 farP );
    ScriptMatrix4 Orthographic( f32 width, f32 height, f32 nearP, f32 farP );
  };

  static const ScriptMatrix4 mScriptMatrix4Identity(1.0f, 0.0f, 0.0f, 0.0f,
                                                    0.0f, 1.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 1.0f, 0.0f,
                                                    0.0f, 0.0f, 0.0f, 1.0f);



  typedef ScriptMatrix4 smat4;

} // namespace Virtus


#endif // Include Guard