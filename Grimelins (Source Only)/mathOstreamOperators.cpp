#include "precompiled.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"

namespace Grimelins
{
  std::ostream& operator<< (std::ostream& out, Vector2 const& v2 )
  {
    out << v2.x << "," << v2.y;
    return out;
  }

  std::ostream& operator<< (std::ostream& out, Vector3 const& v3 )
  {
    out << v3.x << "," << v3.y << ","  << v3.z;
    return out;
  }

  std::ostream& operator<< (std::ostream& out, Vector4 const& v4 )
  {
    out << v4.x << "," << v4.y << ","  << v4.z << "," << v4.w;
    return out;
  }

  std::ostream& operator<< (std::ostream& out, Quaternion const& q )
  {
    out << q.w << "," << q.x << "," << q.y << ","  << q.z;
    return out;
  }

}
