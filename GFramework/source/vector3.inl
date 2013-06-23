namespace cs350
{

  inline Vector3::Vector3()
    : x(0.0f), y(0.0f), z(0.0f)
  {
  }

  inline Vector3::Vector3(const Vector3& rhs)
    : x(rhs.x), y(rhs.y), z(rhs.z)
  {
  }


  inline Vector3::Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
  {
  }

  inline Vector3& Vector3::operator=(const Vector3& rhs)
  {
    if( this != &rhs )
      std::memcpy( v, rhs.v, sizeof(v) );

    return *this;
  }

  inline Vector3 Vector3::operator+(float rhs) const
  {
    return Vector3(x + rhs, y + rhs, z + rhs);
  }

  inline Vector3 Vector3::operator-(float rhs) const
  {
    return Vector3(x - rhs, y - rhs, z - rhs);
  }

  inline Vector3 Vector3::operator*(float rhs) const
  {
    return Vector3(x * rhs, y * rhs, z * rhs);
  }

  inline Vector3 Vector3::operator/(float rhs) const
  {
    return Vector3(x / rhs, y / rhs, z / rhs);
  }

  inline Vector3& Vector3::operator+=(float rhs)
  {
    x += rhs;
    y += rhs;
    z += rhs;
    return *this;
  }

  inline Vector3& Vector3::operator-=(float rhs)
  {
    x -= rhs;
    y -= rhs;
    z -= rhs;
    return *this;
  }

  inline Vector3& Vector3::operator*=(float rhs)
  {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
  }

  inline Vector3& Vector3::operator/=(float rhs)
  {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
  }

  inline Vector3 Vector3::operator+(const Vector3& rhs) const
  {
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  inline Vector3 Vector3::operator-(const Vector3& rhs) const
  {
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  inline Vector3 Vector3::operator*(const Vector3& rhs) const
  {
    return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
  }

  inline Vector3 Vector3::operator/(const Vector3& rhs) const
  {
    return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
  }

  inline Vector3& Vector3::operator+=(const Vector3& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  inline Vector3& Vector3::operator-=(const Vector3& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }

  inline Vector3& Vector3::operator*=(const Vector3& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
  }

  inline Vector3& Vector3::operator/=(const Vector3& rhs)
  {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
  }

  inline float& Vector3::operator[](unsigned index)
  { 
    if(index < 3)
      return v[index];
  }

  inline const float& Vector3::operator[](unsigned index) const
  {
    if(index < 3)
      return v[index];
  }

  inline Vector3 Vector3::operator-() const
  {
    return Vector3(-x, -y, -z);
  }

  inline Vector3 Vector3::operator+() const
  {
    return *this;
  }

  inline float Vector3::Mag() const
  {
    return sqrtf(x*x + y*y + z*z);
  }

  inline float Vector3::SqMag() const
  {
    return (x*x + y*y + z*z);
  }

  inline float Vector3::Dot(const Vector3& rhs) const
  {
    return (x * rhs.x + y * rhs.y + z * rhs.z);
  }

  inline Vector3 Vector3::Cross(const Vector3& rhs) const
  {
    return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
  }

  inline void Vector3::Zero()
  {
    x = y = z = 0.0f;
  }

  inline Vector3& Vector3::Normalize(  )
  {
    float mag = Mag();

    if(mag)
    {
      x /= mag;
      y /= mag;
      z /= mag;
    }
    else
    {
      x = 0;
      y = 0;
      z = 0;
    }

    return *this;

  }

  inline Vector3 Vector3::Normalized(  ) const
  {
    float mag = Mag();

    return Vector3(x / mag, y / mag, z / mag);
  }

  inline bool Vector3::isParallelWith(const Vector3& vec) const
  {
    float scalProd = Dot(vec);
    float a = std::abs(scalProd);
    float b = Mag() * vec.Mag();
    return approxEqual(a,b);
  }


  inline Orientation::Orientation( const vec3& rig, const vec3& u, const vec3& dir, bool transposed )
    : right(rig), up(u), direction(dir), transpose(transposed)
  {
  }

} 
