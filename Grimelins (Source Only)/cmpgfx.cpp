/**********************************************************************/
/*
  
  \file
    cmpgfx.cpp
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Graphics components for our game objects.  These will be managed by graphics
    and they will be attached to objects.  These read the transform for their positions,
    rotations, and scales.
    
*/
/**********************************************************************/
#include "precompiled.h"
#include "cmpgfx.h"
#include "graphics.h"
#include "comptransform.h"
#include "object.h"
#include "dx9device.h"
#include "dx9resourcemanager.h"

namespace Grimelins
{
  /////// SCRIPT BINDS
  //// ShapeComponent
  SCRIPT_ClassStart(gfxShape);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(getTypeName);
  SCRIPT_Method(getTypeID);
  //SCRIPT specialized functions
  //none
  SCRIPT_ClassEnd();

  //// ModelComponent
  SCRIPT_ClassStart(gfxModel);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(getTypeName);
  SCRIPT_Method(getTypeID);
  SCRIPT_Method(ChangeAnim);
  SCRIPT_Method(ChangeAnimNOW);
  SCRIPT_Method(Yaw);
  SCRIPT_Method(Roll);
  SCRIPT_Method(Pitch);
  SCRIPT_Method(YawTo);
  SCRIPT_Method(RollTo);
  SCRIPT_Method(PitchTo);
  SCRIPT_Method(getDirection);
  SCRIPT_Method(getUp);
  SCRIPT_Method(getRight);
  SCRIPT_Method(setDirection);
  SCRIPT_Method(setAnimSpeed);
  SCRIPT_Method(blendToAnimTime);
  SCRIPT_Method(blendToAnim);
  SCRIPT_Method(switchDiffuse);
  SCRIPT_Method(switchEmissive);
  //SCRIPT specialized functions
  //none
  SCRIPT_ClassEnd();



  void gfxShape::serialize( Scope& data, bool read )
  {
    SERIAL_BeginSerializing(gfxShape);
    SERIAL_Var(vec3, "position", shape.pos);
    SERIAL_Var(vec3, "scale", shape.scale);
    SERIAL_Var(ShapeType, "type", shape.type);
    SERIAL_Var(Color, "color", shape.color);
    SERIAL_Var(u1, "solid", shape.solid);
  }

  void gfxShape::serialize( Component const* source )
  {
    gfxShape const* shapeComp = SCAST(gfxShape const*, source);
    shape = shapeComp->shape;
  }

  void gfxShape::ReadTransform( void )
  {
    //Read transform of parent object
    Transform* trans = m_parent->getComponentT<Transform>();
    if(trans)
    {
      shape.pos = trans->position;
      shape.scale = trans->scale;
    }
  }

  void gfxShape::addRegistry( void )
  {
    GRAPHICS->vRegisterComponent(getTypeID(), this);
  }

  void gfxShape::remRegistry( void )
  {
    GRAPHICS->vUnregisterComponent(getTypeID(), this);
  }

  TypeID gfxShape::getTypeID( void ) const
  {
    return gCore->getTypeID<gfxShape>();
  }

  Text gfxShape::getTypeName( void ) const
  {
    return Text("ShapeComponent");
  }

  Component* gfxShape::create( void )
  {
    return SCAST(Component*, new gfxShape());
  }

  gfxShape::~gfxShape()
  {
    remRegistry();
  }

  gfxShape::gfxShape()
  {
  }

  void gfxModel::Update( f32 dt )
  {
    ReadTransform();
    if(model->scrollSpeed)
      model->scrollUVs();
    if(model->controller_)
    {
      model->controller_->Update(dt);
      model->controller_->ProcessAnimationGraph();
    }
  }


  void gfxModel::LoadModelTexture()
  {
    static DX9Texture* defDiff = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE"default_d.dds");
    static DX9Texture* defNrm = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE"default_n.dds");
    static DX9Texture* defEm = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE"default_e.dds");
    static DX9Texture* defSpec = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE"default_s.dds");
    static DX9Texture* defA = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE"default_a.dds");
    //HACK BY JONATHAN BECAUSE OF MISSING DDS
    //TO BE REMOVED BY A QUALIFIED DDS PRACTITIONER (B.T.)
    #pragma message(Reminder "\n\nHACKED DDS/PNG LOADING, REMOVE THIS PROPERLY\n\n")
    Text ext = ".png";

    Text arrayOfPNGNames[] = {"a2platformfl", "a2platformfr", "a2platformbl", "a2platformbr", "a2crystals", "a2planks", "a2siren"};
    int numPNGs = 7;
    for(int i=0;i<numPNGs;++i) if(model->name_ == arrayOfPNGNames[i]) ext = ".png";
    

    if(hasDiff)
    {
      Text diffTexture(CONFIG_PATH_TEXTURE);
      diffTexture.append(model->name_ + "_d" + ext);
      model->diff = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(diffTexture.c_str());
    }
    else
      model->diff = defDiff;

    if(hasNrm)
    {
      Text nrmTexture(CONFIG_PATH_TEXTURE);
      nrmTexture.append(model->name_ + "_n" + ext);
      model->nrml = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(nrmTexture.c_str());
    }
    else
      model->nrml = defNrm;

    if(hasEm)
    {
      Text emTexture(CONFIG_PATH_TEXTURE);
      emTexture.append(model->name_ + "_e" + ext);
      model->emmi = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(emTexture.c_str());
    }
    else
      model->emmi = defEm;

    if(hasSpec)
    {
      Text specTexture(CONFIG_PATH_TEXTURE);
      specTexture.append(model->name_ + "_s" + ext);
      model->spec = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(specTexture.c_str());
    }
    else
      model->spec = defSpec;

    if(hasAlpha)
    {
      Text alphaTexture(CONFIG_PATH_TEXTURE);
      alphaTexture.append(model->name_ + "_a" + ext);
      model->alpha = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(alphaTexture.c_str());
    }
    else
      model->alpha = defA;

  }


  void gfxModel::serialize( Scope& data, bool read )
  {
     vec3 diffCol;
     SERIAL_BeginSerializing(gfxModel);
     SERIAL_Var(Text, "modelfile", modelname);
     SERIAL_Var(u1, "hasmultanims", hasMultipleAnims);
     SERIAL_Var(u1, "hasd", hasDiff);
     SERIAL_Var(u1, "hasn", hasNrm);
     SERIAL_Var(u1, "hase", hasEm);
     SERIAL_Var(u1, "hass", hasSpec);
     SERIAL_Var(u1, "hasa", hasAlpha);
     SERIAL_Var(vec3, "startDir", startDir);
     SERIAL_Var(vec3, "diffCol", diffCol);  

     if(hasMultipleAnims)
       model = GRAPHICS->mModelMgr->LoadModelAnimsFromFile(modelname.c_str());
     else
       model = GRAPHICS->mModelMgr->LoadModel(modelname.c_str());

     //If there was a specified starting direction, use that.
     if(startDir.Length())
     {
       model->dir = startDir;
       model->right = model->dir.Cross(model->up);
     }

     SERIAL_Var(vec3, "pos", model->pos);
     SERIAL_Var(vec3, "trans", model->trans);
     SERIAL_Var(vec3, "scale", model->scale);
     SERIAL_Var(f32, "scrollSpeed", model->scrollSpeed);

     if(hasAlpha)
       model->alphaBlended = true;

     model->diffCol = float4(diffCol.x, diffCol.y, diffCol.z, 1.0f);
     blendTime = blendNextTime = 0.0f;
     blendIndex = betweenIndex = 0;

     //After this is done, load the model's textures
     LoadModelTexture();
  }

  void gfxModel::serialize( Component const* source )
  {
    gfxModel const* modelComp = SCAST(gfxModel const*, source);
    modelname = modelComp->modelname;
    hasMultipleAnims = modelComp->hasMultipleAnims;
    model = new Model();
    *model = *(modelComp->model);
    GRAPHICS->mModelMgr->models_.push_back(model);
    hasDiff = modelComp->hasDiff;
    hasEm = modelComp->hasEm;
    hasNrm = modelComp->hasNrm;
    hasSpec = modelComp->hasSpec;
    hasAlpha = modelComp->hasAlpha;
    blendTime = modelComp->blendTime;
    blendNextTime = modelComp->blendNextTime;
    blendIndex = modelComp->blendIndex;
    betweenIndex = modelComp->betweenIndex;
  }

  void gfxModel::ReadTransform( void )
  {
    //Read transform of parent object
    Transform* trans = m_parent->getComponentT<Transform>();
    if(trans)
    {
      //The model will eventually share the orientation of it's body, and its position is a combination
      //of the body's position and a translation from that body.
      model->pos = trans->position + model->trans;
      model->rot = trans->rotation;
    }
  }

  void gfxModel::addRegistry( void )
  {
    GRAPHICS->vRegisterComponent(getTypeID(), this);
  }

  void gfxModel::remRegistry( void )
  {
    GRAPHICS->vUnregisterComponent(getTypeID(), this);
  }

  TypeID gfxModel::getTypeID( void ) const
  {
    return gCore->getTypeID<gfxModel>();
  }

  Text gfxModel::getTypeName( void ) const
  {
    return Text("ModelComponent");
  }

  Component* gfxModel::create( void )
  {
    return SCAST(Component*, new gfxModel());
  }

  gfxModel::~gfxModel()
  {
    if(model)
    {
      for(u32 i = 0; i < GRAPHICS->mModelMgr->models_.size(); ++i)
        if(GRAPHICS->mModelMgr->models_[i] == model)
          GRAPHICS->mModelMgr->models_.erase(GRAPHICS->mModelMgr->models_.begin() + i);

      delete model;
    }
    remRegistry();

  }

  gfxModel::gfxModel()
  {
 
  }

  void gfxModel::ChangeAnim( Text const& animName )
  {
    model->controller_->ChangeAnimation(animName);
  }

  void gfxModel::ChangeAnimNOW( Text const& animName )
  {
    model->controller_->ChangeAnimationNOW(animName);
  }

  void gfxModel::ChangeAnimNOWIndex( u32 animIndex )
  {
    model->controller_->ChangeAnimationNOW( animIndex );
  }

  void gfxModel::Yaw( f32 angle )
  {
    Matrix4& mat = Matrix4::RotationAxis( model->up, angle  );
    model->right = mat.MulVec(model->right);
    model->dir = mat.MulVec(model->dir);
  }

  void gfxModel::Pitch( f32 angle )
  {
    angle *= GRAPHICS->thisFrameDt;
    Matrix4& mat = Matrix4::RotationAxis( model->right, angle  );
    model->up = mat.MulVec(model->up);
    model->dir = mat.MulVec(model->dir);
  }

  void gfxModel::Roll( f32 angle )
  {
    angle *= GRAPHICS->thisFrameDt;
    Matrix4& mat = Matrix4::RotationAxis( model->dir, angle  );
    model->up = mat.MulVec(model->up);
    model->right = mat.MulVec(model->right);
  }

  void gfxModel::YawTo( f32 angle )
  {
    Matrix4& mat = Matrix4::RotationAxis( model->up, angle  );
    static vec3 rightIdentity(1,0,0);
    static vec3 lookIdentity(0,0,-1);
    model->right = mat.MulVec(rightIdentity);
    model->dir = mat.MulVec(lookIdentity);
  }

  void gfxModel::PitchTo( f32 angle )
  {
    angle *= GRAPHICS->thisFrameDt;
    Matrix4& mat = Matrix4::RotationAxis( model->right, angle  );
    static vec3 upIdentity(0,1,0);
    static vec3 lookIdentity(0,0,-1);
    model->up = mat.MulVec(upIdentity);
    model->dir = mat.MulVec(lookIdentity);
  }

  void gfxModel::RollTo( f32 angle )
  {
    angle *= GRAPHICS->thisFrameDt;
    Matrix4& mat = Matrix4::RotationAxis( model->dir, angle  );
    static vec3 upIdentity(0,1,0);
    static vec3 rightIdentity(1,0,0);
    model->up = mat.MulVec(upIdentity);
    model->right = mat.MulVec(rightIdentity);
  }

  void gfxModel::SetDirection( vec3& dir )
  {
    model->dir = dir;
    static vec3 globalUp(0.0f, 1.0f, 0.0f);

    model->right = dir.Cross(globalUp);
    model->up = model->right.Cross(dir);

    model->right.Normalize();
    model->up.Normalize();
  }

  void gfxModel::setDirection( ScriptVec3 dir )
  {
    model->dir = dir;
    static ScriptVec3 globalUp(0.0f, 1.0f, 0.0f);

    model->right = dir.Cross(globalUp);
    model->up = model->right.Cross(dir);

    model->right.Normalize();
    model->up.Normalize();
  }

  void gfxModel::BlendToNextAnim()
  {
    //If we have a second transition, that will be the final transition.
    if(blendNextTime)
    {
      //Make sure we didn't pick a shitty blend index in the viewer.  
      if(blendIndex >= 0 && blendIndex < model->controller_->animations_.size())
        nextAnimName = model->controller_->animations_[blendIndex]->name_;

      model->controller_->transQ.push(Transition(blendNextTime, nextAnimName));

      if(betweenIndex >= 0 && betweenIndex < model->controller_->animations_.size())
        nextAnimName = model->controller_->animations_[betweenIndex]->name_;

      model->controller_->BlendWithAnimation(nextAnimName, blendTime);
    }
    else
    {
      //Make sure we didn't pick a shitty blend index in the viewer.  
      if(blendIndex >= 0 && blendIndex < model->controller_->animations_.size())
        nextAnimName = model->controller_->animations_[blendIndex]->name_;

      model->controller_->BlendWithAnimation(nextAnimName, blendTime);
    }
  }

  void gfxModel::setAnimSpeed( f32 speed )
  {
    if(model->controller_)
      model->controller_->animSpeed_ = speed;
  }

  void gfxModel::blendToAnimTime( Text const& animName, f32 blendTime /*= 0.17f */ )
  {
    if(model->controller_)
      model->controller_->BlendWithAnimation( animName, blendTime );
  }

  void gfxModel::blendToAnim( Text const& animName )
  {
    blendToAnimTime(animName);
  }

  void gfxModel::switchDiffuse( Text textureName )
  {
    Text difName(CONFIG_PATH_TEXTURE);
    difName.append(textureName);
    model->diff = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(difName.c_str());
  }

  void gfxModel::switchEmissive( Text textureName )
  {
    Text emmiName(CONFIG_PATH_TEXTURE);
    emmiName.append(textureName);
    model->emmi = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(emmiName.c_str());
  }

  DrawnShape::DrawnShape( vec3& position, vec3& extents, Matrix4& orient, ShapeType shapetype /*= Cube*/, Color clr /*= Blue*/, u1 sol /*= true*/ ) 
    :pos(position), scale(extents),     rotation(orient), type(shapetype),             color(clr),           solid(sol)
  {
    cylinder = true;
    anim = false;
    id = 0;
  }

  DrawnShape::~DrawnShape( void )
  {
  }


}