/*! ====================================================================================================================
  @file virtus.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Oct.14.2011

  @brief 
    Inclusion of engine framework.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_VIRTUS_H
#define VIRTUS_VIRTUS_H


#include "fwdefinitions.h"     // Global framework definitions


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// prerequisites.

#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// include segment.

#define SOUNDSYS_V2            // Use v2 of Sound System

#include "utilities.h"         // Project wide settings, like typedefs
#include "diagdebug.h"         // Debug diagnostic utilities
#include "helpers.h"           // Little helper functions and abbreviations
#include "exception.h"         // Exceptions
#include "error.h"             // Error Codes
#include "memman.h"            // Memory management
#include "watchdog.h"          // Watchdog crash handler

#include "shrdptr.h"           // Shared pointer
#include "hstring.h"           // Hashed strings
#include "singleton.h"         // Singleton helper
#include "ScriptBind.h"        // Script Binding
#include "lamath.h"            // Linear Algebra math
#include "scriptvec2.h"
#include "scriptvec3.h"
#include "scriptvec4.h"
#include "scriptmatrix4.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "vector6.h"
#include "matrix4.h"
#include "matrix6.h"
#include "geometry.h"
#include "quaternion.h"
#include "jacobi1x6.h"
#include "primesearch.h"
#include "propertytype.h"      // Class access
#include "ant_tweak.h"         // Ant Tweak Bar settings, includes, defines etc

#include "fileparser.h"        // File parsing
#include "stringutils.h"       // String utilities

#include "resman.h"            // Resource Management
#include "com.h"               // Communication
#include "RawStore.h"           // Command arguments
#include "convar.h"            // Console variables
#include "cfgfile.h"           // User configurations
#include "ilayer.h"            // Layer Management
#include "isystem.h"           // System Management
#include "core.h"              // Core class

#include "appdat.h"            // OS Data
#include "appfiles.h"          // OS file interaction
#include "apputilities.h"      // General OS interaction
#include "appdiagnostics.h"    // OS diagnostics
#include "appmachine.h"        // Hardware info

#include "script.h"            // Script Objects
#include "global.h"            // Core instantiation



#endif // Include Guard