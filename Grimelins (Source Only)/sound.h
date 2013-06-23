/**********************************************************************/
/*
  
  \file
    sound.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Sound Engine for Grimelins.  Features include:
      *Full Sound support (playing, pausing, stopping, etc.)
      *Cached/Loaded sounds
      *Sound Analysis     (if we want to use it in game.)
      *3D sound support
      *FMOD designer support
     
*/
/**********************************************************************/
#ifndef SOUNDSYS_V2

#ifndef VIRTUS_SOUND_H
#define VIRTUS_SOUND_H

#include <fmod.hpp>
#include <kiss_fft.h>
#include "soundNames.h"

namespace Grimelins
{
    //This will be updated when we have more types of sound result possibilities.
    enum SoundResult
    {
      SOUND_GOOD,
      SOUND_BAD,
      SOUND_ALREADY_PLAYING,
    };

    enum SoundMode
    {
      NORMAL  = 0x0000,
      NO_LOOP = 0x0001,
      LOOPING = 0x0002,
      PAUSED  = 0x0004,
      FG      = 0x0008,
      BG      = 0x0010,
      UNIQUE  = 0x0020,
    };

    class SS_Channel
    {
      public:
        SS_Channel() : channel(NULL), ID(0), freeWhenDone(0) {}
        FMOD::Channel *channel;
        u32 ID;
        s32 freeWhenDone;
    };

    class SS_Sound
    {
      public:
        SS_Sound() : sound(NULL), name(NULL), ID(0), myChannel(NULL), forceUnique(0) {}

        //Clean up dynamically allocated memory.
        ~SS_Sound(){ if(name) delete name; }

        void SetName(const ch* name_);
        FMOD::Sound *sound;
        ch* name;
        std::string namestr_;
        u32 ID;
        SS_Channel *myChannel;
        s32 forceUnique;
    };

    class SS_Fader
    {
      public:
        SS_Fader(SS_Channel* channel_, f32 targetVol_, s32 duration_, s32 stopOnFinal_);
        SS_Channel* channel;
        s32 duration;
        f32 finalVol;
        f32 startTime;
        s32 stopOnFinal;
        s32 playing; 
        f32 originalVol;
        s32 direction;        
    };

    SCRIPT_HeaderDefine(Sound);
    class Sound : public ISystem, public IListener
    {
      //User Interface, because the user isn't trusted.
      public:
        Sound();
        ~Sound();

        friend class SS_Fader;

      //ISystem Interface
      public:
        virtual void vLoadSystem(void);
        virtual void vUpdateSystem(void);
        virtual void vUnloadSystem(void);
        virtual hstring vGetSystemID(void) const { return mIDName_; }
        u32 Play(const Text& filePath, f32 volume = 1.0f, u1 streaming = false);
        u32 StopAll();
        u32 FadeAll();
        u32 FadeAllToTarget(f32 target, s32 duration = 250);
        void ToggleMute();
        u1 IsMuted() { return muted_; };

      //ISubscriber Interface
      private:
        virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp);

      //Internal member functions
      private:
        void Initialize();
        void Uninitialize();
        void Update();
        void UnloadChannels();
        void UnloadSounds();
        SoundResult CacheSoundsInPath(const ch* path); 

        SoundResult IsSoundPlaying(SS_Channel* channel, s32* result);
        SoundResult UpdateFader(SS_Fader* fader);
        SoundResult UpdateFaders();

        SoundResult LoadSound(const ch* soundName, SS_Sound** output, SoundMode mode, u1 streaming = false);
        SoundResult PlaySound(SS_Sound* sound, SS_Channel** output, SoundMode mode = NORMAL);
        
        SoundResult PauseSound(SS_Channel* channel, u1 paused);
        SoundResult GetPaused(SS_Channel* channel, u1* pausedOut);
        SoundResult PauseAll(u1 paused = true);

        SoundResult StopSound(SS_Channel* channel);
        

        SoundResult SetVolume(SS_Channel* channel, f32 vol);
        SoundResult GetVolume(SS_Channel* channel, f32* volOut);
        SoundResult SetMasterVolume(f32 vol);

        SoundResult FadeSoundOut(SS_Channel* channel, s32 duration);
        SoundResult FadeSound(SS_Channel* channel, f32 target, s32 duration);
        
        

        SoundResult Mute();
        SoundResult Unmute();
        

        u32 soundID_;
        u32 channelID_;
        FMOD::System* system_;
        FMOD_RESULT result_;
        f32 startTime;
        FrameTimer* timer_;
        std::map<u32, SS_Sound*> soundTable_;
        std::map<u32, SS_Channel*> channelTable_;
        std::map<std::string, SS_Sound*> soundCache_;
        std::map<SS_Channel*, SS_Fader*> activeFaders_;
        std::deque<SS_Fader*> removalQueue_;
        std::list<SS_Fader*> queuedFaders_;    
        hstring mIDName_;
        u1 muted_;
    };

    //Just an error check wrapper to keep me sane.
    void ErrorCheck(FMOD_RESULT result) throw( Exception );

    typedef std::pair<u32, SS_Sound*> SoundPair;
    typedef std::pair<u32, SS_Channel*> ChannelPair;
    typedef std::pair<std::string, SS_Sound*> CachePair;
    typedef std::pair<SS_Channel*, SS_Fader*> FaderPair;

}//namespace Virtus

#endif // Include Guard

#endif