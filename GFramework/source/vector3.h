
#ifndef VECTOR3_H
#define VECTOR3_H

namespace cs350 
{
  struct Quaternion;

  struct Vector3
  {
    union
    {
      struct 
      { 
        float x, y, z; 
      };
      float v[3];
    };

    inline Vector3();
    inline Vector3(const Vector3& rhs);
    inline Vector3(float x, float y, float z);

    inline Vector3& operator=(const Vector3& rhs);

    inline Vector3 operator+(float rhs) const;
    inline Vector3 operator-(float rhs) const;
    inline Vector3 operator*(float rhs) const;
    inline Vector3 operator/(float rhs) const;
    inline Vector3& operator+=(float rhs);
    inline Vector3& operator-=(float rhs);
    inline Vector3& operator*=(float rhs);
    inline Vector3& operator/=(float rhs);

    inline Vector3 operator+(const Vector3& rhs) const;
    inline Vector3 operator-(const Vector3& rhs) const;
    inline Vector3 operator*(const Vector3& rhs) const;
    inline Vector3 operator/(const Vector3& rhs) const;
    inline Vector3& operator+=(const Vector3& rhs);
    inline Vector3& operator-=(const Vector3& rhs);
    inline Vector3& operator*=(const Vector3& rhs);
    inline Vector3& operator/=(const Vector3& rhs);

    inline float& operator[](unsigned index);
    inline const float& operator[](unsigned index) const;

    inline Vector3 operator-() const;
    inline Vector3 operator+() const;

    inline float Mag() const;
    inline float SqMag() const;
    inline Vector3& Normalize();
    inline Vector3 Normalized() const;
    inline float Dot(const Vector3& rhs) const;
    inline Vector3 Cross(const Vector3& rhs) const;
    inline void Zero();
    inline bool isParallelWith(const Vector3& vec) const;
    friend std::ostream& operator<< (std::ostream& out, Vector3 const& v3 );
  };

  typedef Vector3 vec3;
  std::ostream& operator<< (std::ostream& out, Vector3 const& v3 );

  struct Orientation
  {
    inline Orientation( const vec3& rig, const vec3& u, const vec3& dir, bool transposed = false );
    vec3 right, up, direction;
    bool transpose;
  };



}

#include "vector3.inl"
#endif


