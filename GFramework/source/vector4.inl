namespace cs350
{

inline Vector4::Vector4()
  : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

inline Vector4::Vector4(const Vector4& rhs)
  : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
{
}


inline Vector4::Vector4(float xx, float yy, float zz, float ww)
  : x(xx), y(yy), z(zz), w(ww)
{
}

inline Vector4& Vector4::operator=(const Vector4& rhs)
{
  if( this != &rhs )
    std::memcpy( v, rhs.v, sizeof(v) );

  return *this;
}

inline Vector4 Vector4::operator+(float rhs) const
{
  return Vector4(x + rhs, y + rhs, z + rhs, w + rhs);
}

inline Vector4 Vector4::operator-(float rhs) const
{
  return Vector4(x - rhs, y - rhs, z - rhs, w - rhs);
}

inline Vector4 Vector4::operator*(float rhs) const
{
  return Vector4(x * rhs, y * rhs, z * rhs, w * rhs);
}

inline Vector4 Vector4::operator/(float rhs) const
{
  return Vector4(x / rhs, y / rhs, z / rhs, w / rhs);
}

inline Vector4& Vector4::operator+=(float rhs)
{
  x += rhs;
  y += rhs;
  z += rhs;
  w += rhs;
  return *this;
}

inline Vector4& Vector4::operator-=(float rhs)
{
  x -= rhs;
  y -= rhs;
  z -= rhs;
  w -= rhs;
  return *this;
}

inline Vector4& Vector4::operator*=(float rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

inline Vector4& Vector4::operator/=(float rhs)
{
  x /= rhs;
  y /= rhs;
  z /= rhs;
  w /= rhs;
  return *this;
}

inline Vector4 Vector4::operator+(const Vector4& rhs) const
{
  return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

inline Vector4 Vector4::operator-(const Vector4& rhs) const
{
  return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

inline Vector4 Vector4::operator*(const Vector4& rhs) const
{
  return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
}

inline Vector4 Vector4::operator/(const Vector4& rhs) const
{
  return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
}

inline Vector4& Vector4::operator+=(const Vector4& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

inline Vector4& Vector4::operator-=(const Vector4& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

inline Vector4& Vector4::operator*=(const Vector4& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  w *= rhs.w;
  return *this;
}

inline Vector4& Vector4::operator/=(const Vector4& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  w /= rhs.w;
  return *this;
}

inline float& Vector4::operator[](unsigned index)
{
  if(index < 4)
  return v[index];
}

inline const float& Vector4::operator[](unsigned index) const
{
  if(index < 4)
  return v[index];
}

inline Vector4 Vector4::operator-() const
{
  return Vector4(-x, -y, -z, -w);
}

inline Vector4 Vector4::operator+() const
{
  return *this;
}

inline float Vector4::Mag() const
{
  return sqrtf(x*x + y*y + z*z + w*w);
}

inline float Vector4::MagSq() const
{
  return (x*x + y*y + z*z + w*w);
}

inline float Vector4::Dot(const Vector4& rhs) const
{
  return (x * rhs.x + y * rhs.x + z * rhs.z + w * rhs.w);
}

inline Vector4 Vector4::Cross(const Vector4& rhs) const
{
  return Vector4(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x, 0.0f);
}

inline Vector4& Vector4::Normalize(  )
{
  float len = Mag();

  x /= len;
  y /= len;
  z /= len;

  return *this;
}

inline Vector4 Vector4::Normalized(  ) const
{
  float len = Mag();
  return Vector4(x / len, y / len, z / len, w);
}

inline void Vector4::Zero()
{
  std::memset( v, 0, sizeof(v) );
}

inline float Vector4::PlaneEqn( const Vector3& p ) const
{
  return (x * p.x) + (y * p.y) + (z * p.z) + (w);
}

} /// namespace Virtus