#ifndef VECTOR2_H
#define VECTOR2_H

namespace cs350 
{

  struct Vector2
  {
    union
    {
      struct { float x,y; };
      float v[2];
    };

    inline Vector2();
    inline Vector2(const Vector2& rhs);
    inline Vector2(float xx, float yy);

    inline Vector2& operator=(const Vector2& rhs);

    inline Vector2 operator+(float rhs) const;
    inline Vector2 operator-(float rhs) const;
    inline Vector2 operator*(float rhs) const;
    inline Vector2 operator/(float rhs) const;
    inline Vector2& operator+=(float rhs);
    inline Vector2& operator-=(float rhs);
    inline Vector2& operator*=(float rhs);
    inline Vector2& operator/=(float rhs);

    inline Vector2 operator+(const Vector2& rhs) const;
    inline Vector2 operator-(const Vector2& rhs) const;
    inline Vector2 operator*(const Vector2& rhs) const;
    inline Vector2 operator/(const Vector2& rhs) const;
    inline Vector2& operator+=(const Vector2& rhs);
    inline Vector2& operator-=(const Vector2& rhs);
    inline Vector2& operator*=(const Vector2& rhs);
    inline Vector2& operator/=(const Vector2& rhs);

    inline float& operator[](unsigned index);
    inline const float& operator[](unsigned index) const;

    inline Vector2 operator-() const;
    inline Vector2 operator+() const;

    inline float Mag() const;
    inline float MagSq() const;
    inline Vector2& Normalize(  );
    inline Vector2 Normalized(  ) const;
    inline float Dot(const Vector2& rhs) const;
    inline void Zero();
    friend std::ostream& operator<< (std::ostream& out, Vector2 const& v2 );

  };
  std::ostream& operator<< (std::ostream& out, Vector2 const& v2 );
  typedef Vector2 vec2;

} 

#include "vector2.inl"


#endif 