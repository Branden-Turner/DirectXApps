/*! ====================================================================================================================
  @file camera.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.16.2011

  @brief
    Simple camera class...

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_CAMERA_H
#define VIRTUS_CAMERA_H

namespace Grimelins {

    SCRIPT_HeaderDefine(Camera);
    class Camera : public IListener
    {
    public:

      enum CameraMode
      {
        FirstPerson = 0,
        ThirdPerson,
        TargetActive,
        Spherical,
        TotalCameraModes
      };

      Camera( void );
      ~Camera( void );

        // Returns references so you can chain these together
      Camera& setPosition( ScriptVec3 const& pos );
      Camera& setTarget( ScriptVec3 const& tar );
      Camera& setDistance( f32 dist );
      Camera& setEye( ScriptVec3 const& eye );
      Camera& setCameraMode( u32 mode );
      Camera& toggleCameraMode( void );
      ScriptVec3 getPosition();
      ScriptVec3 getUp();
      ScriptVec3 getRight();
      ScriptVec3 getDir();
      ScriptVec3 getEye();
      ScriptVec3 getTarget();

        // Adds to the position, vec2 version assumes only x and y change
      Camera& move( ScriptVec3 const& pos );
      Camera& move( ScriptVec2 const& pos );
      Camera& moveX( f32 dis );
      Camera& moveY( f32 dis );
      Camera& moveZ( f32 dis );

        // This is equivalent to Pitch, Yaw, Roll
      Camera& rotate( ScriptVec3 const& ang );
      Camera& rotX( f32 ang );
      Camera& rotY( f32 ang );
      Camera& rotZ( f32 ang );

        // For spherical coordinates (or ThirdPerson Mode)
      Camera& addAngleX( f32 ang );
      Camera& addAngleY( f32 ang );
      Camera& addDistance( f32 dis );

      Camera& reset( void );
      Camera& updateDT( f32 dt );

      ScriptMatrix4 getMatrix( void );
      CameraMode getCameraMode( void );

      Camera& switchScript(Text const& file);
      Camera& disableScript(void);

      void shake( f32 intensity, f32 time );

      void update(void);


      // ISubscriber interface
    private:
      virtual bool handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp);

    private:

      void BuildMatrix( void );
      void ResetCamera( void );

      typedef Sqrat::Script Sc;
      typedef Sqrat::Function ScFn;

      void removeScript(void);
      void revertToPrev(void);

      Sc* mScript;
      ScFn* mScriptUpdate;

      Sc* mPrevScript;
      ScFn* mPrevScriptUpdate;

      Matrix4 mCachedMat;
      CameraMode mCameraMode;
      u1 mDirtyCache;
      f32 mDT;

      vec3 mDir;
      vec3 mRig;
      vec3 mUp;

      //Eye represents the translation from the position that the lens is at.
      vec3 mEye;
      vec3 mTarget;
      vec3 mAngDist;
      vec3 mPos;
      u1 camShake;
      f32 camShakeTime;
      f32 camShakeIntensity;
      vec3 camShakeSnap;
    };

} // namespace Virtus

#endif // Include Guard
