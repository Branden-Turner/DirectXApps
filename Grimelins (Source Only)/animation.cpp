/**********************************************************************/
/*
  
  \file
    animation.cpp
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Basic keyframe animation system used in the Grimelins Project
    
*/
/**********************************************************************/
#include "precompiled.h"
#include "animation.h"

namespace Grimelins 
{
    static const f32 FPSAnim = 1.0f;
    static const f32 fixedDt = 0.016f;

    Animation::Animation()
    {}

    Animation::~Animation()
    {}

    AnimationController::AnimationController() : animLength_(0.0f), animSpeed_(FPSAnim), skeleton_(NULL), activeAnim_(NULL), 
                                                 blendedAnim_(NULL), nextAnim_(NULL), paused(false), newAnim(false), blending(false),
                                                 blendFactor(0.0f), blendTime(0.0f), blendTimer(false)
    {}

    AnimationController::~AnimationController()
    {}

    void AnimationController::Update( f32 dt )
    {
      //Step forward
      animLength_ += animSpeed_ * fixedDt;

      //Advance the blend factor if we're blending for some amount of time.
      if(blending)
      {
        if(blendFactor < 1.0f)
        {
          f32 t = blendTimer.Get();
          t = std::min( std::max(0.0f, t), blendTime);
          if(blendTime)
            blendFactor = 1.0f - ( ( blendTime - t ) / blendTime );
        }
        //If we're done blending, change the animation to the proper animation and go on about our business.
        else
        {
          ChangeAnimationNOW(blendedAnim_->name_);
          blending = false;
          blendFactor = 0.0f;

          //If we have more transitions that we're supposed to do, do them.
          if(transQ.size())
          {
            Transition t = transQ.top();
            transQ.pop();
            BlendWithAnimation(t.transitionTo_, t.length_);
          }
        }
      }

      //Reset the timer and tracks if necessary. (loop animation)
      if(animLength_ > activeAnim_->length_)
      {
        if(newAnim)
        {
          newAnim = false;
          activeAnim_ = nextAnim_;
        }

        //We're going to start searching for keyframes from the beginning again.
        for(u32 i = 0; i < animTrackData_.size(); ++i)
          animTrackData_[i].lastKey_ = 0;

        animLength_ = 0.0f;
      }
    }

    void AnimationController::ProcessAnimationGraph()
    {
      //Process the animation graph in breadth first order
      for( u32 boneIndex = 0; boneIndex < skeleton_->bones_.size(); ++boneIndex)
      {
        //Grab the parent index
        s32 pIndex = skeleton_->bones_[boneIndex].parentIndex_;

        //Get the interpolation transformation in terms of a vqs using the current bone's index
        VQS trans;
        GetBoneVQS(boneIndex, trans);

        //Grab the parent transformation, if this is a parent, do nothing
        Matrix4 parentTransform = pIndex != -1 ?  boneMatrixBuffer_[ pIndex ] :  Matrix4::GetIdentity();

        //Grab the local transform and concatenate it with this bone's parent transform
        Matrix4 localTransform;
        trans.ToMatrix(localTransform);
        Matrix4 modelTransform = localTransform  * parentTransform;

        //Store this for the children's concatenations.
        boneMatrixBuffer_[boneIndex] = modelTransform;
      }
    }

    void AnimationController::RecursiveProcess(Bone &bone, Matrix4 &transform)
    {
      VQS trans;
      
      GetBoneVQS( bone.index_, trans );

      //Grab the model to bone transform.
      Matrix4 lTrans;
      trans.ToMatrix(lTrans);

      //Concatenate it to put it in the proper place on the model.
      Matrix4 modelTrans =  lTrans * transform;

      //Save this bone's model to bone transform
      boneMatrixBuffer_[bone.index_] = modelTrans;

      //Proceed down the hierarchy of the skeleton, concatenating transformations.
      for(u32 i = 0; i < bone.children_.size(); ++i)
        RecursiveProcess(*bone.children_[i], modelTrans);
    }

    void AnimationController::ProcessBindPose()
    {
      for(u32 i = 0; i < skeleton_->bones_.size(); ++i)
        boneMatrixBuffer_[i] = skeleton_->bones_[i].bindTransform_;
    }

    void AnimationController::GetBoneVQS( u32 boneIndex, VQS &v )
    {
      //Grab the last key so we don't search through the entirety of our track data.
      u32 curKey = animTrackData_[boneIndex].lastKey_;
      Track &curPath = activeAnim_->tracks_[boneIndex];

      //Search forward for the keyframe, this won't be many steps on average, so a linear search is fine.
      while(!blending && curKey < curPath.size() - 1 && curPath[curKey + 1].time_ < animLength_)
        ++curKey;

      //Used for transitive animation blending (We always go to the first frame of the next animation)
      u32 blendKey = 0;
      Track blankTrack;
      Track &blendPath = (blending) ? blendedAnim_->tracks_[boneIndex] : blankTrack;

      //Clamp the animation at the last key frame.
      if( curKey == curPath.size() - 1 )
      {
        //If we're blending use the blend factor to mix the current animation with the specified "blending" animation
        if(blending)
        {
          v.rotation_ = (curPath[curKey].localRot_ * blendFactor) + (blendPath[blendKey].localRot_ * ( 1.0f - blendFactor ));
          v.translation_ = (curPath[curKey].localPos_ * blendFactor) + (blendPath[blendKey].localPos_ * ( 1.0f - blendFactor ));

          //Clamp blended animation
          if( blendKey == blendPath.size() - 1 )
            blendKey = 0;
        }

        //Otherwise just use the last frame's data.
        else
        {
          v.rotation_ = curPath[curKey].localRot_;
          v.translation_ = curPath[curKey].localPos_;
        }

        //Start searching again at index zero.
        curKey = 0;
      }
      else
      {
        //Interpolate between the current key frame and the next key frame using a VQS
        KeyFrame &key1 = curPath[curKey];
        KeyFrame &key2 = curPath[curKey + 1];

        //Grab the times from both keyframes for the normalized t value.
        f32 t1 = key1.time_, t2 = key2.time_;

        //Gotta use a normalized t value for interpolation of the vqs
        f32 segmentTVal = (animLength_ - t1) / (t2 - t1);

        VQS v1(key1.localPos_, key1.localRot_, 1.0f);
        VQS v2(key2.localPos_, key2.localRot_, 1.0f);

        v.Interpolate(v1, v2, segmentTVal);

        //If we're blending, show the interpolation between whatever place we're at and the first frame of the next animation
        if(blending)
        {
          VQS blend;

          //Interpolate between the current key frame and the next key frame using a VQS
          KeyFrame &b1 = blendPath[blendKey];

          VQS v1(b1.localPos_, b1.localRot_, 1.0f);

          blend.Interpolate(v, v1, blendFactor);

          v = blend;
        }
      }

      //Store last key frame so we don't have to search the entire list every time
      animTrackData_[boneIndex].lastKey_ = curKey;
    }

    void AnimationController::ChangeAnimation( Text const& newAnimName )
    {
      for(u32 i = 0; i < animations_.size(); ++i)
      {
        if(newAnimName == animations_[i]->name_)
        {
          nextAnim_ = animations_[i];
          newAnim = true;
          break;
        }
      }
    }

    void AnimationController::ChangeAnimationNOW( Text const& newAnimName )
    {
      for(u32 i = 0; i < animations_.size(); ++i)
      {
        if(newAnimName == animations_[i]->name_)
        {
          nextAnim_ = activeAnim_ = animations_[i];
          animLength_ = 0.0f;
          //We're going to start searching for keyframes from the beginning again.
          for(u32 i = 0; i < animTrackData_.size(); ++i)
            animTrackData_[i].lastKey_ = 0;

          break;
        }
      }
    }

    void AnimationController::ChangeAnimationNOW( u32 animIndex )
    {
      if(animIndex >= 0 && animIndex < animations_.size())
      {
          activeAnim_ = animations_[animIndex];
          animLength_ = 0.0f;

          //We're going to start searching for keyframes from the beginning again.
          for(u32 i = 0; i < animTrackData_.size(); ++i)
            animTrackData_[i].lastKey_ = 0;
      }
    }

    void AnimationController::BlendWithAnimation( Text const& blendName, f32 blendLength )
    {
      //We're now blending
      blending = true;

      //Set up how long we're blending for.
      blendTime = blendLength;

      //Find and set up the blending animation
      for(u32 i = 0; i < animations_.size(); ++i)
        if(blendName == animations_[i]->name_)
        {
          blendedAnim_ = animations_[i];
          break;
        }

      //Go ahead and start the timer of how long we've been blending.
      blendTimer.StartTimer();
    }


    Transition::Transition( f32 length, Text& name ) : length_(length), transitionTo_(name)
    {
    }

    //         This is for blending two synchronized animations
    //         if(blending)
    //         {
    //           VQS blend;
    // 
    //           //Interpolate between the current key frame and the next key frame using a VQS
    //           KeyFrame &b1 = blendPath[blendKey];
    //           KeyFrame &b2 = blendPath[blendKey + 1];
    // 
    //           //Grab the times from both keyframes for the normalized t value.
    //           f32 t1 = b1.time_, t2 = b2.time_;
    // 
    //           //Gotta use a normalized t value for interpolation of the vqs
    //           f32 segmentTVal = (animLength_ - t1) / (t2 - t1);
    // 
    //           VQS v1(b1.localPos_, b1.localRot_, 1.0f);
    //           VQS v2(b2.localPos_, b2.localRot_, 1.0f);
    // 
    //           blend.Interpolate(v1, v2, segmentTVal);
    // 
    //           v = v * blendFactor + blend * ( 1.0f - blendFactor );
    //         }

}//Virtus