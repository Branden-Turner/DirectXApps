/**********************************************************************/
/*
  
  \file
    cmpgfx.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Graphics components for our game objects.  These will be managed by graphics
    and they will be attached to objects.  These read the transform for their positions,
    rotations, and scales.
    
*/
/**********************************************************************/
#ifndef GRIME_CMPGFX_H
#define GRIME_CMPGFX_H
#include "component.h"
#include "shapes.h"
#include "color.h"
#include "model.h"

namespace Grimelins
{

  //Purely an interface for working with the graphics engine
  class GraphicsComponent
  {
    public:
      virtual ~GraphicsComponent(void){};
      virtual void Update(f32 dt) = 0;

    private:
      virtual void ReadTransform(void) = 0;
  };

  class DrawnShape
  {
  public:
    DrawnShape(vec3& position = vec3(0,0,0), vec3& extents = vec3(0,0,0), Matrix4& orient = Matrix4(), ShapeType shapetype = ShapeCube, Color clr = Blue, u1 sol = true);
    ~DrawnShape(void);

    //pos, scale (extents), and rotation will all be read from the transform.
    vec3 pos;
    vec3 scale;
    Matrix4 rotation;
    ShapeType type;
    Color color;
    u1 solid;
    f32 dt; 
    u1 cylinder;
    u1 anim;
    u32 id;
  };


  SCRIPT_HeaderDefine(gfxShape);
  class gfxShape : public Component, public GraphicsComponent
  {
    public:
      virtual void Update(f32 dt_){};
      virtual ~gfxShape();
      gfxShape();

    //Component interface
    public:
      virtual void addRegistry( void );
      virtual void remRegistry( void );
      virtual TypeID getTypeID(void) const;
      virtual Text getTypeName(void) const;
      virtual void serialize( Scope& data, bool read );
      virtual void serialize( Component const* source);
      virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp) {return false;};

    //Graphics interface
    friend class Graphics;
    friend class Factory;
    private:
       static Component* create(void);

    private:
      DrawnShape shape;
      virtual void ReadTransform(void);
  };


  SCRIPT_HeaderDefine(gfxModel);
  class gfxModel : public Component, public GraphicsComponent
  {
    public:
      virtual ~gfxModel();
      gfxModel();
      virtual void Update(f32 dt);
      void ChangeAnim(Text const& animName);
      void ChangeAnimNOW(Text const& animName);
      void ChangeAnimNOWIndex( u32 animIndex );
      void Yaw(f32 angle);
      void Pitch(f32 angle);
      void Roll(f32 angle);
      void YawTo(f32 angle);
      void PitchTo(f32 angle);
      void RollTo(f32 angle);
      vec3& GetDirection(){ return model->dir; };
      vec3& GetRight(){ return model->right; };
      vec3& GetUp(){ return model->up; };
      vec3& GetPos(){ return model->pos; };
      ScriptVec3 getDirection() { return model->dir; };
      ScriptVec3 getUp() { return model->up; };
      ScriptVec3 getRight() { return model->right; };

      void setAnimSpeed( f32 speed );
      void blendToAnimTime( Text const& animName, f32 blendTime = 0.17f );
      void blendToAnim( Text const& animName );

      void setDirection( ScriptVec3 dir );
      void SetDirection( vec3& dir );
      void BlendToNextAnim();
      void switchDiffuse(Text textureName);
      void switchEmissive(Text textureName);
      Text nextAnimName;
      f32 blendTime;
      f32 blendNextTime;
      u32 blendIndex;
      u32 betweenIndex;
      vec3 vsPos;

    //Component interface
    public:
      virtual void addRegistry( void );
      virtual void remRegistry( void );
      virtual TypeID getTypeID(void) const;
      virtual Text getTypeName(void) const;
      virtual void serialize( Scope& data, bool read );
      virtual void serialize( Component const* source);
      virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp) {return false;};

    //Graphics interface
    friend class Graphics;
    friend class Factory;
    friend class Object;
    private:
      static Component* create(void);

    friend class ModelManager;
    private:
      Model* model;
      Text  modelname;
      u1 hasMultipleAnims;
      u1 hasNrm;
      u1 hasEm;
      u1 hasDiff;
      u1 hasSpec;
      u1 hasAlpha;
      vec3 pos;
      vec3 startDir;
      
      virtual void ReadTransform(void);
      void LoadModelTexture();
      
  };


}
#endif GRIME_CMPGFX_H