namespace cs350 
{

  inline Vector2::Vector2()
    : x(0.0f), y(0.0f)
  {
  }

  inline Vector2::Vector2(const Vector2& rhs)
    : x(rhs.x), y(rhs.y)
  {
  }


  inline Vector2::Vector2(float xx, float yy)
    : x(xx), y(yy)
  {
  }

  inline Vector2& Vector2::operator=(const Vector2& rhs)
  {
    if( this != &rhs )
      std::memcpy( v, rhs.v, sizeof(v) );

    return *this;
  }

  inline Vector2 Vector2::operator+(float rhs) const
  {
    return Vector2(x + rhs, y + rhs);
  }

  inline Vector2 Vector2::operator-(float rhs) const
  {
    return Vector2(x - rhs, y - rhs);
  }

  inline Vector2 Vector2::operator*(float rhs) const
  {
    return Vector2(x * rhs, y * rhs);
  }

  inline Vector2 Vector2::operator/(float rhs) const
  {
    return Vector2(x / rhs, y / rhs);
  }

  inline Vector2& Vector2::operator+=(float rhs)
  {
    x += rhs;
    y += rhs;
    return *this;
  }

  inline Vector2& Vector2::operator-=(float rhs)
  {
    x -= rhs;
    y -= rhs;
    return *this;
  }

  inline Vector2& Vector2::operator*=(float rhs)
  {
    x *= rhs;
    y *= rhs;
    return *this;
  }

  inline Vector2& Vector2::operator/=(float rhs)
  {
    x /= rhs;
    y /= rhs;
    return *this;
  }

  inline Vector2 Vector2::operator+(const Vector2& rhs) const
  {
    return Vector2(x + rhs.x, y + rhs.y);
  }

  inline Vector2 Vector2::operator-(const Vector2& rhs) const
  {
    return Vector2(x - rhs.x, y - rhs.y);
  }

  inline Vector2 Vector2::operator*(const Vector2& rhs) const
  {
    return Vector2(x * rhs.x, y * rhs.y);
  }

  inline Vector2 Vector2::operator/(const Vector2& rhs) const
  {
    return Vector2(x / rhs.x, y / rhs.y);
  }

  inline Vector2& Vector2::operator+=(const Vector2& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  inline Vector2& Vector2::operator-=(const Vector2& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  inline Vector2& Vector2::operator*=(const Vector2& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    return *this;
  }

  inline Vector2& Vector2::operator/=(const Vector2& rhs)
  {
    x /= rhs.x;
    y /= rhs.y;
    return *this;
  }

  inline float& Vector2::operator[](unsigned index)
  {
    if(index < 2)
    return v[index];
  }

  inline const float& Vector2::operator[](unsigned index) const
  {
    if(index < 2)
    return v[index];
  }

  inline Vector2 Vector2::operator-() const
  {
    return Vector2(-x, -y);
  }

  inline Vector2 Vector2::operator+() const
  {
    return *this;
  }

  inline float Vector2::Mag() const
  {
    return sqrtf(x*x + y*y);
  }

  inline float Vector2::MagSq() const
  {
    return (x*x + y*y);
  }

  inline Vector2& Vector2::Normalize(  )
  {
    float len = Mag();

    x /= len;
    y /= len;

    return *this;
  }

  inline Vector2 Vector2::Normalized(  ) const
  {
    float len = Mag();
    return Vector2(x / len, y / len);
  }

  inline float Vector2::Dot(const Vector2& rhs) const
  {
    return (x * rhs.x + y * rhs.x);
  }

  inline void Vector2::Zero()
  {
    std::memset( v, 0, sizeof(v) );
  }

}