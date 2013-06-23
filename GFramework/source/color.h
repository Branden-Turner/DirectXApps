#ifndef COLOR_H
#define COLOR_H

namespace cs350
{
    enum Color
    {
      Red,
      Green,
      Blue,
      Cyan,
      Yellow,
      Magenta,
      Black,
      White,
      Gray,
      LightGray,
      DarkGray,
      MaxColors
    };

    // These are RGBA format
    static const vec4 VecArray[] = 
    {
      vec4(1.0f,  0.0f,  0.0f,  1.0f),
      vec4(0.0f,  1.0f,  0.0f,  1.0f),
      vec4(0.0f,  0.0f,  1.0f,  1.0f),
      vec4(0.0f,  1.0f,  1.0f,  1.0f),
      vec4(1.0f,  1.0f,  0.0f,  1.0f),
      vec4(1.0f,  0.0f,  1.0f,  1.0f),
      vec4(0.0f,  0.0f,  0.0f,  1.0f),
      vec4(1.0f,  1.0f,  1.0f,  1.0f),
      vec4(0.5f,  0.5f,  0.5f,  1.0f),
      vec4(0.75f, 0.75f, 0.75f, 1.0f),
      vec4(0.25f, 0.25f, 0.25f, 1.0f)
    };

    // These are ARGB format
    static const unsigned Array[] = 
    {
      0xFFFF0000,
      0xFF00FF00,
      0xFF0000FF,
      0xFF00FFFF,
      0xFFFFFF00,
      0xFFFF00FF,
      0xFF000000,
      0xFFFFFFFF,
      0xFF808080,
      0xFFC0C0C0,
      0xFF404040
    };

    static vec4 const& GetColorV( Color col ) { return VecArray[col]; }
    static unsigned         GetColor(  Color col ) { return Array[col];    }
    static vec4 const& GetColorRandV( void  ) { return VecArray[rand() % MaxColors]; }
    static unsigned         GetColorRand(  void  ) { return Array[rand() % MaxColors];    }
} 

#endif 