/*! ====================================================================================================================
  @file syssound.h

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.17.2012

  @brief 
    Sound System

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once

#ifdef SOUNDSYS_V2

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Segment

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Segment

#if VIRTUS_CONFIG_DEBUG_MODE | VIRTUS_CONFIG_PROFILE_MODE
# define AK_COMMUNICATION 1
#else
# define AK_COMMUNICATION 0
#endif

#include <MMSystem.h>
#include <InitGuid.h>
#include <dsound.h>
#include <dsconf.h>
#include <AK/SoundEngine/Common/AkCallback.h>
#include <AK/SoundEngine/Common/IAkPlugin.h>

#include "akfileio/AkFilePackageLowLevelIOBlocking.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "AkSoundEngine.lib")
#pragma comment(lib, "AkMemoryMgr.lib")
#pragma comment(lib, "AkStreamMgr.lib")
#pragma comment(lib, "AkMusicEngine.lib")

#if AK_COMMUNICATION
# pragma comment(lib, "CommunicationCentral.lib")
#endif
#pragma comment(lib, "AkVorbisDecoder.lib")

#include "object.h"

namespace Grimelins {

SCRIPT_HeaderDefine(Speaker);


class Sound : public ISystem
{
public:
  typedef AkPlayingID PlayingID;
  typedef AkTimeMs TimeMs;
  typedef void (*EVENT_CALLBACK) (AkCallbackType, AkCallbackInfo*);
  typedef AkRtpcValue RTPCValue;
  typedef AkUInt32 SoundFlag;
  typedef AkPluginType PluginType;
  static AkGameObjectID const GLOBAL = 0x00000002;

public: 
  virtual void vLoadSystem(void);
  virtual void vUnloadSystem(void);
  virtual void vUpdateSystem(void);
  virtual hstring vGetSystemID(void) const;

public: // Methods
  void ForceReRender(void);
  bool LoadBank(Text BankName);
  bool UnloadBank(Text BankName);
  PlayingID PostEvent(Text Name, ObjectID object, SoundFlag Flags = 0, EVENT_CALLBACK Hook = NULL, 
                      void* ClientData = NULL);
  u32 ScriptPostEvent(Text Name, ObjectID Object, u32 Flags); // Script Version
  PlayingID PostGlobalEvent(Text Name, SoundFlag Flags = 0, EVENT_CALLBACK Hook = NULL, void* Cookie = NULL);
  u32 ScriptPostGlobalEvent(Text Name, u32 Flags); // Script version
  TimeMs GetPlayPosition(PlayingID id);
  void SetRealTimeParameterControlValue(const char* RTPC, RTPCValue parameter);
  void SetState(const char* StateGroup, const char* state);
  void SetSwitch(const char* SwitchGroup, const char* SwitchState, ObjectID object);
  void PostTrigger(Text TriggerName, ObjectID object);
  void RegisterPlugin(PluginType type, SoundFlag CompanyID, SoundFlag PluginID, AkCreatePluginCallback CBPl = NULL, 
                      AkCreateParamCallback CBPa = NULL);
  void SetListenerPosition(Vector3 Position, Vector3 OrientationDir, Vector3 OrientationUp);
  void ScriptSetListenerPosition(ScriptVec3 Position, ScriptVec3 OrientationDir, ScriptVec3 OrientationUp);

private: // Helpers
  void registerGameObject(ObjectID object, const char* name);
  void unRegisterGameObject(ObjectID object);
  void setPosition(ObjectID object, Vector3 Position, Vector3 Orientation);

private: // Declarations
  friend class Speaker;
  typedef std::list<Speaker*> Speakers;

private: // Accessors
  void addSpeaker(Speaker* speaker);
  void removeSpeaker(Speaker* speaker);

private: // Data
  Speakers mSpeakers;

};

extern Sound* SOUND;

} // namespace Grimelins



#endif