#include "precompiled.h"
#include "dx9shapes.h"
#include "anim2d.h"
#include "graphics.h"
#include "dx9device.h"

namespace Grimelins
{

  AnimQuad::AnimQuad( vec3& position, u32 nFrames, f32 fLen, f32 qwidth, f32 qheight, f32 twidth, f32 theight, u1 screen, f32 aSpeed ) 
    : animSpeed(aSpeed),
      numFrames(nFrames),
      frameLength(fLen),
      screenSpace(screen),
      texW(twidth),
      texH(theight)
                                                                                              
  {
    //Set up the initial frame and shape
    uOffset = 1.0f / numFrames;

    //Verts are laid out like this 
    // * 0 ---------- * 2
    //
    //
    // * 1 ---------- * 3

    //Vertex 0 remains unchanged

    //Vertex 1 stays the same as well

    //Vertex 2 goes to uOffset, 0

    //Vertex 3 goes to uOffset, 1

    if(screenSpace)
    {
      shape = GRAPHICS->mDevice->GetShapes()->GetShape(ShapeQuad2D);
      resetUVs2D();
    }
    else
    {
      shape = GRAPHICS->mDevice->GetShapes()->GetShape(ShapePlane);
      resetUVs3D();
    }

    //Set up scale
    scale.x = qwidth;
    scale.y = qheight;
    scale.z = 0.0f;

    pos = position;
  }

  void AnimQuad::update( f32 dt )
  {
    //Keep incrementing how much time we've spent in this frame
    curTime += animSpeed * dt;

    //loop the animation
    if(curTime > frameLength)
    {
      nextFrame();
      curTime = 0.0f;
    }
  }

  void AnimQuad::nextFrame()
  {
    //advance the frame
    curFrame++;

    //loop the frames ( 0 to frames - 1)
    if(curFrame == numFrames)
    {
      curFrame = 0;
      if(screenSpace)
        resetUVs2D();
      else
        resetUVs3D();
    }

    //update the uv of whatever shape you have
    if(shape)
    {
      if(screenSpace)
        updateUVs2D();
      else
        updateUVs3D();
    }

  }

  void AnimQuad::resetUVs2D()
  {
    shape->vtx2D[0].tex = vec2( 0.0f,  0.0f);
    shape->vtx2D[1].tex = vec2( 0.0f,  1.0f);
    shape->vtx2D[2].tex = vec2( uOffset,  0.0f);
    shape->vtx2D[3].tex = vec2( uOffset,  1.0f);

    Vertex_XY_UV* vert = NULL;
    shape->mVBuffer->Lock(vert);

    std::memcpy(vert, &shape->vtx2D[0], sizeof(Vertex_XY_UV) * 4);

    shape->mVBuffer->Unlock();
  }

  void AnimQuad::resetUVs3D()
  {
    shape->vtx3D[0].tex = vec2( 0.0f,  0.0f);
    shape->vtx3D[1].tex = vec2( 0.0f,  1.0f);
    shape->vtx3D[2].tex = vec2( uOffset,  0.0f);
    shape->vtx3D[3].tex = vec2( uOffset,  1.0f);

    Vertex_XYZ_NRM_UV* vert = NULL;
    shape->mVBuffer->Lock(vert);

    std::memcpy(vert, &shape->vtx3D[0], sizeof(Vertex_XYZ_NRM_UV) * 4);

    shape->mVBuffer->Unlock();
  }

  void AnimQuad::updateUVs2D()
  {
    shape->vtx2D[0].tex.x += uOffset * curFrame;
    shape->vtx2D[1].tex.x += uOffset * curFrame;
    shape->vtx2D[2].tex.x += uOffset * curFrame;
    shape->vtx2D[3].tex.x += uOffset * curFrame;

    Vertex_XY_UV* vert = NULL;
    shape->mVBuffer->Lock(vert);

    std::memcpy(vert, &shape->vtx2D[0], sizeof(Vertex_XY_UV) * 4);

    shape->mVBuffer->Unlock();
  }

  void AnimQuad::updateUVs3D()
  {
    shape->vtx3D[0].tex.x += uOffset * curFrame;
    shape->vtx3D[1].tex.x += uOffset * curFrame;
    shape->vtx3D[2].tex.x += uOffset * curFrame;
    shape->vtx3D[3].tex.x += uOffset * curFrame;

    Vertex_XY_UV* vert = NULL;
    shape->mVBuffer->Lock(vert);

    std::memcpy(vert, &shape->vtx3D[0], sizeof(Vertex_XYZ_NRM_UV) * 4);

    shape->mVBuffer->Unlock();
  }

}