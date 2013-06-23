/*! ====================================================================================================================
  @file dx9quad.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.11.2011

  @brief
    Basic quad class... just holding some basic data.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "dx9quad.h"

namespace Grimelins { 

DX9Quad::DX9Quad( void )
  : mVBuffer(NULL),
    mIBuffer(NULL)
{
}

DX9Quad::~DX9Quad( void )
{
}

} // namespace Virtus