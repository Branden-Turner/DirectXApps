/*! ====================================================================================================================
  @file ant_tweak.cpp

  Codename Grimelins 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Nov.4.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "ant_tweak.h"

//define any ant functions you want to make that have more than one application.
// JUST REMEMBER! You need to have the prototype in the header, and you need to define it in both sections of this file!

namespace Grimelins{
#ifndef ANT_TWEAK_ENABLED
  //SECTION 1. Release Mode Implementations (empty ones)

  ANT__RETVAL ANT__Hide(void* bar){}
  ANT__RETVAL ANT__Show(void* bar){}
  ANT__RETVAL ANT__Toggle(void* bar){}
  ANT__RETVAL ANT__CopyStdStringToClient(std::string&, const std::string&){}


#else
  //SECTION 2 Real Implementations

  ANT__RETVAL ANT__Hide(void* bar)
  {
    ANT_Bar* b = static_cast<ANT_Bar*>(bar);
    b->hide();
  }
  ANT__RETVAL ANT__Show(void* bar)
  {
    ANT_Bar* b = static_cast<ANT_Bar*>(bar);
    b->show();
    b->setTop();
  }

  ANT__RETVAL ANT__Toggle(void* bar)
  {
    ANT_Bar* b = static_cast<ANT_Bar*>(bar);
    b->toggle();
    b->setTop();
  }

  ANT__RETVAL ANT__CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
  {
    destinationClientString = sourceLibraryString;
  }

  unsigned ANT_Bar::uniqueNameID = 0;

#endif

}//namespace
