/*! ====================================================================================================================
  @file scriptInclude.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.18.2011

  @brief Header which imports all Squirrel libraries.

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_SCRIPTINCLUDE_H
#define VIRTUS_SCRIPTINCLUDE_H

#if VIRTUS_CONFIG_DEBUG_MODE
#pragma comment(lib, "squirreld.lib")    //Squirrel Core Library
#pragma comment(lib, "sqstdlibd.lib")    //Squirrel Standard library
#pragma comment(lib, "sqratimportd.lib") //Squirrel Binding Library
#elif VIRTUS_CONFIG_PROFILE_MODE
#pragma comment(lib, "squirrelp.lib")    //Squirrel Core Library
#pragma comment(lib, "sqstdlibp.lib")    //Squirrel Standard library
#pragma comment(lib, "sqratimportp.lib") //Squirrel Binding Library
#else
#pragma comment(lib, "squirrel.lib")    //Squirrel Core Library
#pragma comment(lib, "sqstdlib.lib")    //Squirrel Standard library
#pragma comment(lib, "sqratimport.lib") //Squirrel Binding Library
#endif

//Squirrel library headers
#include <squirrel.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>	
#include <sqstdstring.h>
#include <sqstdaux.h>

//sqrat library headers
#include <sqmodule.h>
#include <sqrat.h>
#include <sqratimport.h>
#include <sqmodule.h>

#endif //VIRTUS_SCRIPTINCLUDE_H
