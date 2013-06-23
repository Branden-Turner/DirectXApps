/**********************************************************************/
/*
  
  \file
    precompiled.h
  
  \author
    branden.turner
    All content © 2012 DigiPen (USA) Corporation, all rights reserved.

  \brief
    makes the precompiled header
    
*/
/**********************************************************************/
#ifndef PRECOMPILED_H
#define PRECOMPILED_H

# pragma warning( disable : 4201 )
# pragma warning( disable : 4239 )
# pragma warning( disable : 4127 )
# pragma warning( disable : 4706 )
# pragma warning( disable : 4505 )
# pragma warning( disable : 4100 )

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include <WindowsX.h>

#include <iostream>

#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <sstream>

// Standard C Lib
#include <cstdlib>
#include <climits>
#include <cctype>
#include <cstdio>
#include <cmath>

#include "dx10.h"
#include "globalmath.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix4.h"
#include "color.h"
#include "timer.h"
#include "quaternion.h"
#include "vqs.h"

#include "vertexType.h"
#include "geometry.h"
#include "shapes.h"
#include "dxapp.h"
#include "kdtree.h"
#include "dbvh.h"
#include "debugdraw.h"


#endif
