/**********************************************************************/
/*
  
  \file
    animation.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Basic keyframe animation system used in the Grimelins Project 
    
*/
/**********************************************************************/
#ifndef GRIME_ANIMATION_H
#define GRIME_ANIMATION_H

#include "skeleton.h"
#include <stack>

namespace Grimelins 
{
    //Used for transition queueing 
    struct Transition
    {
      Transition( f32 length, Text& name );
      f32 length_;
      Text transitionTo_;
    };

    //A frame of animation to be interpolated between (using VQS interpolation)
    struct KeyFrame
    {
      f32 time_;
      vec3 localPos_;
      AnimQuaternion localRot_;
    };

    typedef std::vector<KeyFrame> Track;

    struct TrackData
    {
      u32 lastKey_;
    };

    typedef std::vector<TrackData> TrackBuffer;
    typedef std::vector<Matrix4> MatrixBuffer;

    //Animation data to be used by the animation controller of an object
    struct Animation
    {
      Animation();
      ~Animation();
      std::string name_;
      f32 length_;
      std::vector<Track> tracks_;
    };

    //Controls the animation of a mesh
    struct AnimationController
    {
      //Init
      AnimationController();
      ~AnimationController();

      //Internal functions for engine use
      void Update(f32 dt);
      void ProcessAnimationGraph();
      void RecursiveProcess(Bone &bone, Matrix4 &transform);
      void ProcessBindPose();
      void GetBoneVQS( u32 boneIndex, VQS &v );
      void ChangeAnimation( Text const& newAnimName );
      void ChangeAnimationNOW( Text const& newAnimName );
      void ChangeAnimationNOW( u32 animIndex );
      void BlendWithAnimation( Text const& blendName, f32 blendLength );

      //To be manipulated by internals of engine
      f32 animLength_;
      f32 animSpeed_;
      shrdptr<Skeleton> skeleton_;
      shrdptr<Animation> activeAnim_;
      shrdptr<Animation> blendedAnim_;
      shrdptr<Animation> nextAnim_;
      TrackBuffer animTrackData_;
      MatrixBuffer boneMatrixBuffer_;
      std::vector<shrdptr<Animation>> animations_;

      //just a queue for transitive animation blending
      std::stack<Transition> transQ;

      //to be used by game logic/animation system
      u1 paused;
      u1 newAnim;
      u1 blending;
      f32 blendFactor;
      f32 blendTime;
      FrameTimer blendTimer;
    };

}

#endif //GRIME_ANIMATION_H