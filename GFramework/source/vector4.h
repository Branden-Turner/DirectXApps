#ifndef VECTOR4_H
#define VECTOR4_H



namespace cs350
{

struct Vector4
{
  union
  {
    struct { float x,y,z,w; };
    float v[4];
  };
  
  inline Vector4();
  inline Vector4(const Vector4& rhs);
  inline Vector4(float xx, float yy, float zz, float ww);

  inline Vector4& operator=(const Vector4& rhs);
  inline Vector4& operator=(const D3DXVECTOR4 rhs);

  inline Vector4 operator+(float rhs) const;
  inline Vector4 operator-(float rhs) const;
  inline Vector4 operator*(float rhs) const;
  inline Vector4 operator/(float rhs) const;
  inline Vector4& operator+=(float rhs);
  inline Vector4& operator-=(float rhs);
  inline Vector4& operator*=(float rhs);
  inline Vector4& operator/=(float rhs);

  inline Vector4 operator+(const Vector4& rhs) const;
  inline Vector4 operator-(const Vector4& rhs) const;
  inline Vector4 operator*(const Vector4& rhs) const;
  inline Vector4 operator/(const Vector4& rhs) const;
  inline Vector4& operator+=(const Vector4& rhs);
  inline Vector4& operator-=(const Vector4& rhs);
  inline Vector4& operator*=(const Vector4& rhs);
  inline Vector4& operator/=(const Vector4& rhs);

  inline float& operator[](unsigned index);
  inline const float& operator[](unsigned index) const;

  inline Vector4 operator-() const;
  inline Vector4 operator+() const;

  inline float Mag() const;
  inline float MagSq() const;
  inline Vector4& Normalize();
  inline Vector4 Normalized() const;
  inline float Dot(const Vector4& rhs) const;
  inline Vector4 Cross(const Vector4& rhs) const;
  inline void Zero();
  inline float PlaneEqn(const Vector3& p) const;
  friend std::ostream& operator<< (std::ostream& out, Vector4 const& v4 );
};

Vector4& Vector4::operator=( const D3DXVECTOR4 rhs )
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = rhs.w;

  return *this;
}



std::ostream& operator<< (std::ostream& out, Vector4 const& v4 );

typedef Vector4 float4;
typedef Vector4 vec4;

} // namespace Virtus

#include "vector4.inl"

#endif // Include Guard