/*! ====================================================================================================================
  @file syssound.cpp

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.17.2012

  @brief 
    Sound System bound to Audiokinext Wwise

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Segment

#include "precompiled.h"

// AK
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/Plugin/AkVorbisFactory.h>

#if !VIRTUS_CONFIG_RELEASE_MODE
# include <AK/Comm/AkCommunication.h> // This enables network communication for sound designer
#pragma comment(lib, "wsock32.lib")
#endif

#include "object.h"
#include "compspeaker.h"
#include "factory.h"
#include "syssound.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Definition Segment

#define SOUND_MAX_MEM_POOLS 20
#define SOUND_BASE_PATH L"assets\\sound\\"
#define SOUND_LANG_DIR L"English(US)"

#if VIRTUS_CONFIG_DEBUG_MODE || VIRTUS_CONFIG_PROFILE_MODE
#define CHECK_AUDIO_OBJECT(object) do { \
  Object* obj = gFactory->getObjectByID(object); \
  ErrorIf(!obj, "Can not set switch, object is undefined!"); \
  ErrorIf(!obj->getComponentT<Speaker>(), "Can not set switch: object does not have speaker"); \
  } while(0)
#else
#define CHECK_AUDIO_OBJECT(object) ((void)(0))
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Code Segment

namespace Grimelins {

  SCRIPT_ClassStart(Sound);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(ForceReRender);
  SCRIPT_Method(LoadBank);
  SCRIPT_Method(UnloadBank);
  SCRIPT_MethodOverload(ScriptPostEvent, "PostEvent", u32, (Text Name, u32 Object, u32 Flags));
  SCRIPT_MethodOverload(ScriptPostGlobalEvent, "PostGlobalEvent", u32, (Text Name, u32 Flags));
  SCRIPT_Method(GetPlayPosition);
  SCRIPT_Method(SetState);
  SCRIPT_Method(SetSwitch);
  SCRIPT_Method(PostTrigger);
  SCRIPT_MethodOverload(ScriptSetListenerPosition, "SetListenerPosition", void, (ScriptVec3, ScriptVec3, ScriptVec3));
  //SCRIPT specialized functions
  //none
  SCRIPT_ClassEnd();


  Sound* SOUND = NULL;
  static CAkFilePackageLowLevelIOBlocking sFilePackage;


  //////////////////////////////////////////////////////////////////////////
  // Virtual Interface

  void Sound::vLoadSystem( void )
  {
    assert(SOUND == nullptr);
    SOUND = this;
    SCRIPT_RegisterGlobal(Sound);

    // Initialize memory manager
    AkMemSettings memSettings;
    memSettings.uMaxNumPools = SOUND_MAX_MEM_POOLS;

    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
      Error("Couldn't launch sound system: Error while applying memory settings.");

    // Initialize low level File I/O Settings
    AkStreamMgrSettings strmSettings;
    AK::StreamMgr::GetDefaultSettings(strmSettings);

    if (!AK::StreamMgr::Create(strmSettings))
      Error("Couldn't launch sound system: Error while appplying stream settings.");

    AkDeviceSettings devSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(devSettings);

    if (sFilePackage.Init(devSettings) != AK_Success)
      Error("Couldn't launch sound system: Error while applying dev settings.");

    // Initialize sound engine
    AkInitSettings initSettings;
    AkPlatformInitSettings platInitiSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platInitiSettings);

    if (AK::SoundEngine::Init(&initSettings, &platInitiSettings) != AK_Success)
      Error("Couldn't launch sound system: Error during WWISE initialization.");


    // Initialize music engine
    AkMusicSettings musicSetings;
    AK::MusicEngine::GetDefaultInitSettings(musicSetings);

    if (AK::MusicEngine::Init(&musicSetings) != AK_Success)
      Error("Couldn't launch sound system: Error during music engine initialization.");

#if AK_COMMUNICATION
    // Initialize communications
    AkCommSettings commSettings;
    AK::Comm::GetDefaultInitSettings(commSettings);
    commSettings.ports.uDiscoveryBroadcast = 42042;

    if (AK::Comm::Init(commSettings) != AK_Success)
      Error("Couldn't launch sound system: Error while initializing development communication.");
#endif

    // Initialize low level reading from appropriate locations
    sFilePackage.SetBasePath(SOUND_BASE_PATH);
    sFilePackage.SetLangSpecificDirName(SOUND_LANG_DIR);


    // Register Vorbis

    AK::SoundEngine::RegisterCodec(AKCOMPANYID_AUDIOKINETIC, AKCODECID_VORBIS, CreateVorbisFilePlugin, 
      CreateVorbisBankPlugin);

    if (!LoadBank("Init.bnk"))
      Application::FullStopError("Missing Sound Bank Initialization file, can not proceed loading audio engine!");


    AK::SoundEngine::RegisterGameObj(GLOBAL, "GLOBAL_SOUND");
  }

  void Sound::vUnloadSystem( void )
  {
    SOUND = NULL;

#if AK_COMMUNICATION
    AK::Comm::Term();
#endif

    AK::MusicEngine::Term();
    AK::SoundEngine::Term();

    if (AK::IAkStreamMgr::Get())
      AK::IAkStreamMgr::Get()->Destroy();

    AK::MemoryMgr::Term();
  }

  void Sound::vUpdateSystem( void )
  {
    // Update Speaker Positions

    Speakers::iterator speaker = mSpeakers.begin();
    Speakers::iterator speakerE = mSpeakers.end();

    for (; speaker != speakerE; ++speaker)
    {
      Object* obj = (*speaker)->parent();
      setPosition(obj->getID(), obj->getPosition(), (*speaker)->getDirection());
    }

    AK::SoundEngine::RenderAudio();
  }

  hstring Sound::vGetSystemID( void ) const
  {
    return "Sound";
  }

  //////////////////////////////////////////////////////////////////////////
  // Public Methods

  void Sound::ForceReRender(void)
  {
    AK::SoundEngine::RenderAudio();
  }

  bool Sound::LoadBank(Text BankName)
  {
    AkBankID BankID;
    AKRESULT wwRes = AK::SoundEngine::LoadBank(BankName.c_str() , 
      AK_DEFAULT_POOL_ID , BankID );

    if (wwRes != AK_Success)
    {
      ErrorIf(true, "Unable to load sound bank '%s'!", BankName.c_str());
      return false;
    }
    else
      return true;

  }

  bool Sound::UnloadBank(Text BankName)
  {
    AKRESULT wwRes = AK::SoundEngine::UnloadBank(BankName.c_str());

    if (wwRes != AK_Success)
    {
      ErrorIf(true, "Unable to unload sound bank '%s'!", BankName.c_str());
      return false;
    }
    else
      return true;
  }


  Sound::PlayingID Sound::PostEvent(Text Name, ObjectID object, SoundFlag Flags, EVENT_CALLBACK Hook, 
    void* ClientData)
  {
    CHECK_AUDIO_OBJECT(object);

    return AK::SoundEngine::PostEvent(Name.c_str(), object + GLOBAL, Flags, Hook, 
      ClientData);
  }

  u32 Sound::ScriptPostEvent(Text Name, ObjectID Object, u32 Flags)
  {
    return PostEvent(Name, Object, Flags, NULL, NULL);
  }


  Sound::PlayingID Sound::PostGlobalEvent(Text Name, SoundFlag Flags, EVENT_CALLBACK Hook, void* Cookie)
  {
    return AK::SoundEngine::PostEvent(Name.c_str(), GLOBAL, Flags, Hook, Hook);
  }

  u32 Sound::ScriptPostGlobalEvent(Text Name, u32 Flags)
  {
    return PostGlobalEvent(Name, Flags, NULL, NULL);
  }

  Sound::TimeMs Sound::GetPlayPosition(PlayingID id)
  {
    Sound::TimeMs ret;
    AK::SoundEngine::GetSourcePlayPosition(id, &ret);
    return ret;
  }


  void Sound::SetRealTimeParameterControlValue(const char* RTPC, RTPCValue parameter)
  {
    AKRESULT wwRes = AK::SoundEngine::SetRTPCValue(RTPC, parameter);
    ErrorIf(wwRes != AK_Success, "Unable to change real time parameter control value");
  }

  void Sound::SetState(const char* StateGroup, const char* state)
  {
    AK::SoundEngine::SetState(StateGroup, state);
  }

  void Sound::SetSwitch(const char* SwitchGroup, const char* SwitchState, ObjectID object)
  {
    CHECK_AUDIO_OBJECT(object);

    AKRESULT wwRes = AK::SoundEngine::SetSwitch(SwitchGroup, SwitchState, object + GLOBAL);
    ErrorIf(wwRes != AK_Success, "Unable to set switch in group '%s' to state '%s'", SwitchGroup, SwitchState);
  }

  void Sound::RegisterPlugin(PluginType type, SoundFlag CompanyID, SoundFlag PluginID, AkCreatePluginCallback CBPl, 
    AkCreateParamCallback CBPa)
  {
    AKRESULT wwRes = AK::SoundEngine::RegisterPlugin(type, CompanyID, PluginID, CBPl, CBPa);
    ErrorIf(wwRes != AK_Success, "Unable to register plugin.");
  }

  void Sound::PostTrigger(Text TriggerName, ObjectID object)
  {
    AKRESULT wwRes;

    if (!gFactory->getObjectByID(object))
      wwRes = AK::SoundEngine::PostTrigger(TriggerName.c_str(), AK_INVALID_GAME_OBJECT);
    else
    {
      CHECK_AUDIO_OBJECT(object);
      wwRes = AK::SoundEngine::PostTrigger(TriggerName.c_str(), object + GLOBAL);
    }

    ErrorIf(wwRes != AK_Success, "Could not post trigger.");
  }

  void Sound::SetListenerPosition(Vector3 Position, Vector3 OrientationDir, Vector3 OrientationUp)
  {
    AkListenerPosition pos;
    pos.Position.X = Position.x;
    pos.Position.Y = Position.y;
    pos.Position.Z = Position.z;
    pos.OrientationFront.X = OrientationDir.x;
    pos.OrientationFront.Y = OrientationDir.y;
    pos.OrientationFront.Z = OrientationDir.z;
    pos.OrientationTop.X = OrientationUp.x;
    pos.OrientationTop.Y = OrientationUp.y;
    pos.OrientationTop.Z = OrientationUp.z;

    AK::SoundEngine::SetListenerPosition(pos);
  }


  void Sound::ScriptSetListenerPosition(ScriptVec3 Position, ScriptVec3 OrientationDir, ScriptVec3 OrientationUp)
  {
    SetListenerPosition(Vector3(Position), Vector3(OrientationDir), Vector3(OrientationUp));
  }

  //////////////////////////////////////////////////////////////////////////
  // Private Helpers


  void Sound::registerGameObject(ObjectID object, const char* name)
  {
    AKRESULT wwRes = AK::SoundEngine::RegisterGameObj(object + GLOBAL, name);
    ErrorIf (wwRes != AK_Success, "Unable to register object with sound engine.");
  }

  void Sound::unRegisterGameObject(ObjectID object)
  {
//     CHECK_AUDIO_OBJECT(object);

    AKRESULT wwRes = AK::SoundEngine::UnregisterGameObj(object + GLOBAL);
    ErrorIf (wwRes != AK_Success, "Unable to unregister object with sound engine.");
  }

  void Sound::setPosition(ObjectID object, Vector3 Position, Vector3 Orientation)
  {
    CHECK_AUDIO_OBJECT(object);

    AkSoundPosition pos;
    pos.Orientation.X = Orientation.x;
    pos.Orientation.Y = Orientation.y;
    pos.Orientation.Z = Orientation.z;
    pos.Position.X = Position.x;
    pos.Position.Y = Position.y;
    pos.Position.Z = Position.z;

    AK::SoundEngine::SetPosition(object + GLOBAL, pos);
  }


//////////////////////////////////////////////////////////////////////////
// Private Methods

void Sound::addSpeaker(Speaker* speaker)
{
  mSpeakers.push_back(speaker);
  registerGameObject(speaker->parent()->getID(), speaker->parent()->getArchetype().c_str());
}

void Sound::removeSpeaker(Speaker* speaker)
{
  unRegisterGameObject(speaker->parent()->getID());
  mSpeakers.erase(std::remove(mSpeakers.begin(), mSpeakers.end(), speaker), mSpeakers.end());
}


} // namespace Grimelins


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// wWise (De/)Allocation Hooks

namespace AK
{
  void* AllocHook(size_t size)
  {
    return operator new(size);
  }

  void FreeHook(void* data)
  {
    delete(data);
  }

  void* VirtualAllocHook(void* addr, size_t size, DWORD type, DWORD protect)
  {
    return VirtualAlloc(addr, size, type, protect);
  }

  void VirtualFreeHook(void* addr, size_t size, DWORD type)
  {
    VirtualFree(addr, size, type);
  }

} // namespace AK