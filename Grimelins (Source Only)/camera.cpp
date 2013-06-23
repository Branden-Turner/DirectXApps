/*! ====================================================================================================================
  @file camera.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.16.2011

  @brief
    Simple camera class...

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "camera.h"
#include "syssound.h"

namespace Grimelins {

  SCRIPT_ClassStart(Camera);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(switchScript);
  SCRIPT_Method(disableScript);
  SCRIPT_Method(setPosition);
  SCRIPT_Method(setTarget);
  SCRIPT_Method(setDistance);
  SCRIPT_Method(setEye);
  SCRIPT_Method(setCameraMode);
  SCRIPT_Method(toggleCameraMode);
  SCRIPT_Method(getPosition);
  SCRIPT_Method(getUp);
  SCRIPT_Method(getRight);
  SCRIPT_Method(getDir);
  SCRIPT_Method(getEye);
  SCRIPT_Method(getTarget);
  SCRIPT_Method(moveX);
  SCRIPT_Method(moveY);
  SCRIPT_Method(moveZ);
  SCRIPT_Method(rotate);
  SCRIPT_Method(rotX);
  SCRIPT_Method(rotY);
  SCRIPT_Method(rotZ);
  SCRIPT_Method(addAngleX);
  SCRIPT_Method(addAngleY);
  SCRIPT_Method(addDistance);
  SCRIPT_Method(updateDT);
  SCRIPT_Method(getMatrix);
  SCRIPT_Method(shake);
  SCRIPT_MethodOverload(update, "updateManual", void, (void));
  SCRIPT_ClassEnd();

Camera::Camera( void )
  : mCameraMode(ThirdPerson), mEye(0,0,0), mScript(NULL), mScriptUpdate(NULL), mPrevScript(NULL), mPrevScriptUpdate(NULL)
{
  ResetCamera();
  BuildMatrix();
}

Camera::~Camera( void )
{
}

Camera& Camera::reset( void )
{
  mDirtyCache = true;

  ResetCamera();

  return *this;
}

Camera& Camera::setPosition( ScriptVec3 const& pos )
{
  mDirtyCache = true;
  SOUND->SetListenerPosition(pos, mDir, mUp);

  mPos = pos;

  return *this;
}

Camera& Camera::setTarget( ScriptVec3 const& tar )
{
  mDirtyCache = true;

  mTarget = tar;

  return *this;
}

Camera& Camera::setDistance( f32 dist )
{
  mDirtyCache = true;
  
  mAngDist.z = dist;

  return *this;
}

Camera& Camera::setEye( ScriptVec3 const & eye )
{
  mDirtyCache = true;

  mEye = eye;

  return *this;
}

Camera& Camera::setCameraMode( u32 mode )
{
  mDirtyCache = true;

  mCameraMode = (CameraMode)mode;

  return *this;
}

Camera& Camera::toggleCameraMode( void )
{
  mDirtyCache = true;

  mCameraMode = (mCameraMode == Spherical) ? ThirdPerson : Spherical;

  return *this;
}

Camera& Camera::move( ScriptVec3 const& pos )
{
  mDirtyCache = true;

  mEye += pos * mDT;
  
  return *this;
}

Camera& Camera::move( ScriptVec2 const& pos )
{
  mDirtyCache = true;

  mEye.x += pos.x * mDT;
  mEye.y += pos.y * mDT;

  return *this;
}

Camera& Camera::moveX( f32 dis )
{
  mDirtyCache = true;

  mEye += mRig * (dis * mDT);

  return *this;
}

Camera& Camera::moveY( f32 dis )
{
  mDirtyCache = true;

  mEye += mUp * (dis * mDT);

  return *this;
}

Camera& Camera::moveZ( f32 dis )
{
  mDirtyCache = true;

  mEye += mDir * (dis * mDT);

  return *this;
}

Camera& Camera::rotate( ScriptVec3 const& ang )
{
  return rotX(ang.x * mDT).rotY(ang.y * mDT).rotZ(ang.z * mDT);
}

Camera& Camera::rotX( f32 ang )
{
  mDirtyCache = true;

  Matrix4& mat = Matrix4::RotationAxis( mRig, ang * mDT, mEye );

  mUp  = mat.MulVec(mUp);
  mDir = mat.MulVec(mDir);

  return *this;
}

Camera& Camera::rotY( f32 ang )
{
  mDirtyCache = true;

  Matrix4& mat = Matrix4::RotationAxis( mUp, ang * mDT, mEye  );

  mRig = mat.MulVec(mRig);
  mDir = mat.MulVec(mDir);

  return *this;
}

Camera& Camera::rotZ( f32 ang )
{
  mDirtyCache = true;

  Matrix4& mat = Matrix4::RotationAxis( mDir, ang * mDT, mEye  );

  mUp  = mat.MulVec(mUp);
  mRig = mat.MulVec(mRig);

  return *this;
}

Camera& Camera::addAngleX( f32 ang )
{
  mDirtyCache = true;

  mAngDist.x += ang;

  return *this;
}

Camera& Camera::addAngleY( f32 ang )
{
  if( abs(mAngDist.y + ang) > (halfPI * 0.95f) )
    return *this;

  mDirtyCache = true;  

  mAngDist.y += ang;

  return *this;
}

Camera& Camera::addDistance( f32 dis )
{
  mDirtyCache = true;

  mAngDist.z += dis;

  if( mAngDist.z < 5.0f)
    mAngDist.z = 5.0f;

  return *this;
}

ScriptMatrix4 Camera::getMatrix( void )
{
  if(mDirtyCache)
    BuildMatrix();

  return mCachedMat;
}


Camera::CameraMode Camera::getCameraMode( void )
{
  return mCameraMode;
}

void Camera::BuildMatrix( void )
{
  mDirtyCache = false;

  f32 cosa;
  vec3 spher;

  switch(mCameraMode)
  {
    //This is where you manually set up the camera in game logic
    case ThirdPerson:
      mCachedMat = Matrix4::LookAtMatrix( mPos + mEye, mTarget, mUp );
    break;

    //Camera is on the sphere
    case Spherical:
      cosa = cosf(mAngDist.y);

      spher.x = sinf(mAngDist.x) * cosa * mAngDist.z;
      spher.y = sinf(mAngDist.y) * mAngDist.z;
      spher.z = cosf(mAngDist.x) * cosa * mAngDist.z;

      mCachedMat = Matrix4::LookAtMatrix(spher + mPos + mEye, mTarget, mUp );
    break;

    //Camera is at center of sphere
    case FirstPerson:
      cosa = cosf(mAngDist.y);

      vec3 spher;
      spher.x = sinf(mAngDist.x) * cosa * mAngDist.z;
      spher.y = sinf(mAngDist.y) * mAngDist.z;
      spher.z = cosf(mAngDist.x) * cosa * mAngDist.z;

      mCachedMat = Matrix4::LookAtMatrix(mPos + mEye, spher, mUp );
    break;
  }
}

void Camera::ResetCamera( void )
{
  mCachedMat.SetIdentity();
  mDirtyCache = false;

  mRig     = vec3(1.0f, 0.0f,  0.0f);
  mUp      = vec3(0.0f, 1.0f,  0.0f);
  mDir     = vec3(0.0f, 0.0f, -1.0f);

  mTarget  = vec3(0.0f, 0.0f, -1.0f);
  mEye     = vec3(0.0f, 0.0f,  0.0f);
  mPos     = vec3(0.0f, 0.0f,  0.0f);
  mAngDist = vec3(0.0f, 0.0f, 150.0f);
}

Camera& Camera::updateDT( f32 dt )
{
  mDT = dt;
  return *this;
}

bool Camera::handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp)
{
  return false; // Do not consume event by default
}

Grimelins::ScriptVec3 Camera::getPosition()
{
  return mPos;
}

Grimelins::ScriptVec3 Camera::getUp()
{
  return mUp;
}

Grimelins::ScriptVec3 Camera::getRight()
{
  return mRig;
}

Grimelins::ScriptVec3 Camera::getDir()
{
  return mDir;
}

Grimelins::ScriptVec3 Camera::getEye()
{
  return mEye;
}

Grimelins::ScriptVec3 Camera::getTarget()
{
  return mTarget;
}

Camera& Camera::switchScript( Text const& file )
{
  ResetCamera();
  //save previous script (could be NULL)
  mPrevScript = mScript;
  mPrevScriptUpdate = mScriptUpdate;
  mScript = NULL;
  mScriptUpdate = NULL;
    gCore->Scripter.runString("camera.clear();");


  mScript = new Sc(gCore->Scripter);
  try { mScript->CompileFile(CONFIG_PATH_SCRIPT + file); }
  catch (Sqrat::Exception e)  {
    printDebug("Error compiling file '%s' in squirrel virtual machine for Script object.\n", file.c_str());
    revertToPrev();  
    return *this;
  }

  if(!mScript->IsCompiled())
  {
    ErrorIf(true, "Script Failed to compile '%s'\n\nCheck the filepath and filename.\n If you are sure it's right, check Squirrel syntax in the file.\n", file.c_str());    
    revertToPrev();
    return *this;
  }

  try { mScript->Run(); }
  catch (Sqrat::Exception e)  { 
    printDebug("Error in squirrel virtual machine running script '%s' for Scripted camera.\n", file.c_str()); 
    revertToPrev();
    return *this;
  }

  mScriptUpdate = new ScFn(Sqrat::RootTable(gCore->Scripter).GetSlot("camera"), "update");
  ErrorIf(!mScriptUpdate, "Script Object failed to allocate heap memory for a function");
  ErrorIf(mScriptUpdate->IsNull(), "Camera was given a script that does not implement camera::update(cam, dt).");

  if(!mScriptUpdate || mScriptUpdate->IsNull())
  {
    revertToPrev();
    return *this;
  }

  ScFn* CamSetup = new ScFn(Sqrat::RootTable(gCore->Scripter).GetSlot("camera"), "setup");
  ErrorIf(!CamSetup, "Script Object failed to allocate heap memory for a function");

  if(CamSetup && !CamSetup->IsNull())
    CamSetup->Execute(this);
  delete CamSetup;
  return *this;
}

void Camera::removeScript( void )
{
  delete mScriptUpdate;
  mScriptUpdate = NULL;
  delete mScript;
  mScript = NULL;
}

void Camera::revertToPrev( void )
{
  removeScript();
  mScript = mPrevScript;
  mScriptUpdate = mPrevScriptUpdate;
  mPrevScript = NULL;
  mPrevScriptUpdate = NULL;
  gCore->Scripter.runString("camera.clear();");
}

static f32 bobSpeed = 15.0f;
static f32 bobUpperBound = 5.0f;
static f32 bobLowerBound = -1.0f;
static FrameTimer shakeTimer(false);
void Camera::shake( f32 intensity, f32 time )
{
  camShakeTime = time;
  camShakeIntensity = intensity;
  camShake = true;
  shakeTimer.StartTimer();
}


void Camera::update( void )
{
  if(mScriptUpdate != NULL)
    mScriptUpdate->Execute(this, mDT);

    //Camera bobbing
    if(!camShake)
    {  
      mEye *= 0.98f;
//       if(mEye.y > bobUpperBound)
//       {
//         mEye.y =  bobUpperBound;
//         bobSpeed = -bobSpeed;
//       }
//       else if(mEye.y < bobLowerBound)
//       {
//          mEye.y =  bobLowerBound;
//          bobSpeed = -bobSpeed;
//       }
//     
//       mEye = mEye + mUp * bobSpeed * mDT;
    }
  
    //Otherwise shake the damn thing.
    else if(shakeTimer.Get() < camShakeTime)
    {
      static vec3 shakeDir; 
    
      s32 pickDir = rand() % 4;
    
      switch(pickDir)
      {
        case 0:
          shakeDir = mUp;
        break;
      
          case 1:
          shakeDir = -mUp;
        break;
      
          case 2:
          shakeDir = -mRig;
        break;
      
          case 3:
          shakeDir = mRig;
        break;
      }
    
      mEye = mEye + shakeDir * bobSpeed * camShakeIntensity;
      mEye.y = std::max(0.0f, mEye.y);
    }
    //Or snap it back to it's starting position.
    else
    {
      camShake = false;
      camShakeIntensity = 0.0f;
      camShakeTime = 0.0f;
//       mEye = camShakeSnap;
    }
}

Camera& Camera::disableScript( void )
{
  revertToPrev();
  removeScript();
  return *this;
}

} // namespace Virtus