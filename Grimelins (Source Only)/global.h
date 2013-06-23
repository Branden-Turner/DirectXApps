/*! ====================================================================================================================
  @file global.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.24.2011

  @brief 
    Declaration of global variables

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_GLOBAL_H
#define VIRTUS_GLOBAL_H


namespace Grimelins
{
  Core* CoreInstance(void);   /// Use this when calling before main
  extern Core* gCore;         /// Main System management
}


#endif // Include Guard

