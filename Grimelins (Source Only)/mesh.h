/**********************************************************************/
/*
  
  \file
    mesh.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Mesh operations for animating, skinning, and loading meshes into the engine.
    
*/
/**********************************************************************/
#ifndef VIRTUS_MESH_H
#define VIRTUS_MESH_H

#include "dx9.h"

namespace Grimelins 
{

    enum VertexType
    {
      Static,
      Skinned, 
      Simple
    };

    class DX9VertexBuffer;
    class DX9IndexBuffer;

    class Mesh
    {
      public:
        Mesh();
        ~Mesh();

        void *vBufferData_;
        s32 *iBufferData_;

        u32 vertexCount_;
        u32 triCount_;
        u32 indexCount_;
        VertexType vertexType_;
        u32 vertSize_;
        DXVertexDeclPtr decl_;
        
        DX9VertexBuffer *vertexBuffer_;
        DX9IndexBuffer *indexBuffer_;
    };
}//Virtus

#endif //VIRTUS_MESH_H