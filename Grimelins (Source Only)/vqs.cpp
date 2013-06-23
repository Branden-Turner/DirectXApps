/**********************************************************************/
/*
  
  \file
    vqs.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This is going to be our VQS for incremental interpolation.
    
*/
/**********************************************************************/
#include "precompiled.h"
#include "vqs.h"


namespace Grimelins
{
    VQS::VQS( vec3 &trans, AnimQuaternion &rot, f32 s ) : translation_(trans), rotation_(rot), scale_(s)
    {}

    VQS::VQS()
    {}

    void VQS::ToMatrix( Matrix4& mat )
    {
      rotation_.ToMatrix(mat);
      mat.m30 = translation_.x;
      mat.m31 = translation_.y;
      mat.m32 = translation_.z;
    }

    void VQS::Interpolate( const VQS &v1, const VQS &v2, f32 t )
    {
      //Slerp or Nlerp for rotation (depends on how "good" we want our animation to look)
      rotation_.Nlerp(v1.rotation_, v2.rotation_, t);

      //Lerp for the translation
      translation_ = v1.translation_ * (1.0f - t) + v2.translation_ * t;

      //Explerp for scale
      scale_ = std::pow(v1.scale_ / v2.scale_, t) * v1.scale_;
    }

    VQS VQS::operator+( const VQS &rhs ) const
    {
      return VQS(translation_ + rhs.translation_, rotation_ + rhs.rotation_, scale_ + rhs.scale_);
    }

    VQS VQS::operator*( const VQS &rhs ) const
    {
      return VQS((*this) * rhs.translation_, rotation_ * rhs.rotation_, scale_ * rhs.scale_);
    }

    vec3 VQS::operator*( const vec3 &rhs ) const
    {
      return (rotation_ * (rhs * scale_) * rotation_.Inverse()) + translation_;
    }

    Grimelins::VQS VQS::operator*( f32 scalar )
    {
      return VQS( translation_ * scalar, scalar * rotation_, scalar * scale_ );
    }

    VQS& VQS::operator+=( const VQS &rhs )
    {
      translation_ += rhs.translation_;
      rotation_ += rhs.rotation_;
      scale_ += rhs.scale_;

      return *this;
    }

    VQS& VQS::operator*=( const VQS &rhs )
    {
      translation_ = (*this) * rhs.translation_;
      rotation_ *= rhs.rotation_;
      scale_ *= rhs.scale_;

      return *this;
    }
}