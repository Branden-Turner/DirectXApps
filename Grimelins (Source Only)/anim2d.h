#ifndef ANIM_2D_H
#define ANIM_2D_H

#include "cmpgfx.h"

namespace Grimelins
{
  class Shape;

  struct AnimQuad : public DrawnShape
  {
    AnimQuad( vec3& position, u32 nFrames, f32 fLen, f32 qwidth, f32 qheight, f32 twidth, f32 theight, u1 screen = false, f32 aSpeed = 1.0f );

    void update(f32 dt);
    void nextFrame();

    void resetUVs2D();
    void resetUVs3D();

    void updateUVs2D();
    void updateUVs3D();

    //Current frame we're on and how much time we've spent inside it
    u32 curFrame;
    f32 curTime;
    f32 animSpeed;

    //Number of frames in the animation, and how long each frame takes
    u32 numFrames;
    f32 frameLength;

    //The sprite sheet offsets for u (x) and v (y) (currently we only support a flat (horizontal) sheet
    f32 uOffset;
    f32 vOffset;

    //Is this going to be a screenspace quad or a 3d quad?
    u1 screenSpace;

    f32 texW;
    f32 texH;

    //This will be the shape we have, which will either be a 2d or 3d quad (plane w/ 4 verts)
    Shape* shape;
  };
}



#endif