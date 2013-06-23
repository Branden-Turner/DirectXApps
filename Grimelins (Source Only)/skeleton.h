/**********************************************************************/
/*
  
  \file
    skeleton.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This will take care of the hierarchical structure to store the bones in
    so that we can cleanly draw and animate our models.
    
*/
/**********************************************************************/
#ifndef VIRTUS_SKELETON_H
#define VIRTUS_SKELETON_H

#include "vqs.h"

namespace Grimelins 
{
    struct Bone
    {
      //Position/rotation data needed for transform
      vec3 localPos_;
      AnimQuaternion localRot_;

      //the bind transform and the model to bone transform are both stored per bone so we don't
      //have to use the inverse all the time to go back to model space.
      Matrix4 invTransform_;
      Matrix4 bindTransform_;

      //Identification
      u32 index_;
      s32 parentIndex_;
      std::vector<Bone*> children_;
      std::string name_;

      vec3 worldPos_;
    };

    //A skeleton is just a bucket of bones
    class Skeleton
    {
      public:
        std::vector<Bone> bones_;
        std::vector<Bone*> rootBones_;
    };
    
}//Virtus

#endif //VIRTUS_SKELETON_H