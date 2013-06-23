/*! ====================================================================================================================
  @file utilities.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.15.2011

  @brief 
    Utility macros, defines and functions

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_UTILITIES_H
#define VIRTUS_UTILITIES_H

namespace Grimelins
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// types.

typedef char               ch;
typedef bool               u1;
typedef unsigned char      u8;
typedef char               s8;
typedef unsigned short     u16;
typedef short              s16;
typedef unsigned int       u32;
typedef int                s32;
typedef unsigned long long u64;
typedef long long          s64;
typedef float              f32;
typedef double             f64;


typedef union _lInteger 
{
  struct 
  {
    unsigned long LowPart;
    long HighPart;
  };
  struct 
  {
    unsigned long LowPart;
    long HighPart;
  } u;

  long long QuadPart;
} lInt;

typedef std::string Text;
typedef const char* CText;
typedef u32 TypeID;

}


#endif // Include Guard
