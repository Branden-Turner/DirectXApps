/**********************************************************************/
/*
  
  \file
    mesh.cpp
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Mesh operations for animating, skinning, and loading meshes into the engine.
    
*/
/**********************************************************************/
#include "precompiled.h"
#include "mesh.h"
#include "dx9resourcemanager.h"
#include "dx9buffers.h"

namespace Grimelins 
{
    Mesh::Mesh() : vBufferData_(NULL), iBufferData_(NULL), vertexCount_(0), 
                   triCount_(0), indexCount_(0), vertexBuffer_(NULL), indexBuffer_(NULL)
    {}

    Mesh::~Mesh()
    {
      SafeDelete(vBufferData_);
      SafeDelete(iBufferData_);

      if(vertexBuffer_)
        SafeRelease(*vertexBuffer_->mBuffer);
      if(indexBuffer_)
        SafeRelease(*indexBuffer_->mBuffer);
    }

}//Virtus