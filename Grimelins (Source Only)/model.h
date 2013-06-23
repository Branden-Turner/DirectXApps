/**********************************************************************/
/*
  
  \file
    model.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This is how we store models in engine after we've loaded them.
    
*/
/**********************************************************************/
#ifndef VIRTUS_MODEL_H
#define VIRTUS_MODEL_H

#include "mesh.h"
#include "animation.h"
#include "dx9texturetype.h"

namespace Grimelins 
{
    class DX9Texture;
    class Model
    {
    public:
      Model();
      ~Model();
      Model(const Model& rhs);
      Model& operator=(const Model& rhs);
      AnimationController* controller_;;
      shrdptr<Mesh> mesh_;

      //These will be read from the transform.
      vec3 pos, trans, scale;
      Quat rot;

      //These are going to be used for moving the model around (for now)
      vec3 dir, up, right;

      //This is for scripts to be able to pass a modelname out and then manipulate it.
      Text name_;

      //Textures applied to this model
      shrdptr<DX9Texture, NoDelete> diff;
      shrdptr<DX9Texture, NoDelete> nrml;
      shrdptr<DX9Texture, NoDelete> emmi;
      shrdptr<DX9Texture, NoDelete> spec;
      shrdptr<DX9Texture, NoDelete> alpha;

      u1 alphaBlended;
      float4 diffCol;
      f32    shinyCoeff;
      f32    bloomFactor;
      f32    scrollSpeed;

      void scrollUVs();
    };
}

#endif //VIRTUS_MODEL_H