/**********************************************************************/
/*
  
  \file
    matrix4.h
  
  \author
    branden.turner
    All content © 2012 DigiPen (USA) Corporation, all rights reserved.

  \brief
    4x4 Matrix class for use in this graphics framework
    
*/
/**********************************************************************/

#ifndef MATRIX4_H
#define MATRIX4_H

namespace cs350
{
  struct Vector4;
  struct Vector3;
  struct Orientation;

  //For rotation matrices.
  enum RotationAxis
  {
    X,
    Y,
    Z
  };



  struct Matrix4x4
  {
    //Always construct a matrix as the identity matrix. 
    inline Matrix4x4();
    inline Matrix4x4(const Matrix4x4& rhs);

    //For the different matrix functions that need columns/rows constructed
    inline Matrix4x4(float mm00, float mm01, float mm02, float mm03,
                     float mm10, float mm11, float mm12, float mm13,
                     float mm20, float mm21, float mm22, float mm23,
                     float mm30, float mm31, float mm32, float mm33);

    inline Matrix4x4( const D3DXMATRIX& rhs );
    //Basic operations
    inline void Zero();
    inline void Identity();

    //Orientation and scale matrices
    static Matrix4x4 Rotation(float angle, RotationAxis axis);
    static Matrix4x4 Rotation(float angle, const Vector3& vec);
    static Matrix4x4 Scale(float scale);
    static Matrix4x4 Scale(float x, float y, float z);
    static Matrix4x4 Scale( const Vector3& sc );
    static Matrix4x4 Translation(float x, float y, float z);
    static Matrix4x4 Translation( const Vector3& trans );

    //Fills in a vec3 with the translation component.
    void GetTranslation( Vector3& trans );

    //When I need matrix representations of cross and tensor products
    Matrix4x4 CrossProdMtx(const Vector3& represent) const;
    Matrix4x4 TensorProdMtx(const Vector3& lhs, const Vector3& rhs) const;
    inline Matrix4x4 InverseMtx() const;


    inline Matrix4x4 Transposed() const;
    inline void Transpose();

    //Get the identity matrix
    static const Matrix4x4& I();

    inline Matrix4x4& operator*=(const Matrix4x4& rhs);
    inline Matrix4x4  operator*  (const Matrix4x4& rhs) const;
    inline Matrix4x4& operator+=(const Matrix4x4& rhs);
    inline Matrix4x4 operator+(const Matrix4x4& rhs) const;
    inline Matrix4x4& operator= (const Matrix4x4& rhs);

    //Handles 4d vectors and treats 3d vectors 
    inline Vector4 operator*(const Vector4& rhs) const;
    inline Vector3   MultVec(const Vector3& rhs) const;
    inline Vector3    MultPt(const Vector3& rhs) const;

    static Matrix4x4 OrientationMtx( const Orientation& orient );
    static Matrix4x4 AlignVectorCamera( const Vector3& ri, const Vector3& up, const Vector3& di, const Vector3& ey );
    static Matrix4x4 LookAtMatrix( Vector3 const& eye, Vector3 const& pos, Vector3 const& up );
    static Matrix4x4 LookAtMatrixDir( Vector3 const& eye, Vector3 const& dir, Vector3 const& up );
    static Matrix4x4 PerspectiveProjFOV( float aspectRatio, float halfFov, float nearP, float farP );
    static Matrix4x4 OrthographicProj( float width, float height, float nearP, float farP );

    //Need a flat, 4x4, and index representation for use around the engine.
    union
    {
      struct 
      {
        float m00, m01, m02, m03,
              m10, m11, m12, m13,
              m20, m21, m22, m23,
              m30, m31, m32, m33; 
      };
      float mtx[4][4];
      float v[16];
    };   
  };

  typedef Matrix4x4 float4x4;
  typedef Matrix4x4 mat4;

  static const Matrix4x4    IdentityMtx(1.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 1.0f);
}

#include "matrix4.inl"
#endif 