/**********************************************************************/
/*
  
  \file
    sound.cpp
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    Sound Engine for Grimelins.  Features include:
      *Full Sound support (playing, pausing, stopping, etc.)
      *Sound Analysis     (if we want to use it in game.)
      *Cached/Loaded sounds
      *3D sound support
      *FMOD designer support
     
*/
/**********************************************************************/
#include "precompiled.h"
#ifndef SOUNDSYS_V2
#include "sound.h"

#pragma comment(lib, "fmodex_vc.lib")

namespace Grimelins
{
    static const s32 MAX_CHANNELS = 1024;

    SCRIPT_ClassStart(Sound);
    SCRIPT_CTOR_Disabled();
    SCRIPT_Method(Play);
    SCRIPT_Method(FadeAll);
    SCRIPT_Method(StopAll);
    SCRIPT_Method(FadeAllToTarget);
    SCRIPT_Method(ToggleMute);
    SCRIPT_Method(IsMuted);
    SCRIPT_ClassEnd();

    // A Global Pointer to the sound engine so that levels can play sounds
    Sound* SOUND;

    //Fader stuff
    SS_Fader::SS_Fader(SS_Channel* channel_, f32 targetVol_, s32 duration_, s32 stopOnFinal_) : 
    channel(channel_),  duration(duration_), finalVol(targetVol_), 
      startTime(SOUND->timer_->Get()), stopOnFinal(stopOnFinal_), playing(1)
    {
      //If supplied a bad channel, we don't want to start working with it.
      if(channel)
      {
        //Everything else has been initialized properly at this point.
        channel->channel->getVolume(&originalVol);

        //Decide if we're fading up or down.
        if(originalVol < finalVol)
          direction = 1;
        else 
          direction = -1;

        //Now we can queue the fader
        SOUND->queuedFaders_.push_back(this);
      }
    }

    SoundResult Sound::UpdateFader(SS_Fader* fader)
    {
      //If the fader isn't playing, we're done.
      if(! fader->playing)
        return SOUND_GOOD;

      s32 playing;

      //Also, if the channel is done playing, the fader is now done as well.
      IsSoundPlaying(fader->channel, &playing);
      if(!playing)
      {
        fader->playing = 0;
        return SOUND_GOOD;
      }

      //Find elapsed Time->
      f32 currentTime = timer_->Get();
      f32 elapsedTime = currentTime - fader->startTime;
      f32 percentOfTime = (f32)elapsedTime / ((fader->duration) ? fader->duration : 1);

      //Calculate the volume step for this update
      f32 volumeStep = ((fader->finalVol - fader->originalVol) * percentOfTime) + fader->originalVol;

      SetVolume(fader->channel, volumeStep);

      u1 done = false;

      //Figure out if we're done or not
      //If we're going up and we're above our target volume, or we're going
      //down and we're below our target volume, we're done.
      if((fader->direction > 0  && volumeStep >= fader->finalVol) || 
        (fader->direction < 0 && volumeStep <= fader->finalVol))
        done = true;

      if(done)
      {
        fader->playing = 0;
        if(fader->stopOnFinal)
          StopSound(fader->channel);
      }

      return SOUND_GOOD; 
    }

    SoundResult Sound::UpdateFaders()
    {
      //Clear out any left over data.
      removalQueue_.clear();

      //Update the faders, and mark any for deletion that need to be.
      std::map<SS_Channel*, SS_Fader*>::iterator it;
      for(it = activeFaders_.begin(); it != activeFaders_.end(); it++)
      {
        //Update the fader
        UpdateFader(it->second);

        //If the fader isn't playing, add it to the deletion queue
        if(!it->second->playing)
          removalQueue_.push_back(it->second);
      }

      //If I have things to kill, I should go ahead and do that now.
      if(!removalQueue_.empty())
      {
        for(u32 i = 0; i < removalQueue_.size(); ++i)
        {
          //Kill it from active faders, then delete it.
          std::map<SS_Channel*, SS_Fader*>::iterator it = activeFaders_.lower_bound(removalQueue_[i]->channel);

          //If we found it, kill it.
          if(it != activeFaders_.end() && !(activeFaders_.key_comp()(removalQueue_[i]->channel, it->first)))
            activeFaders_.erase(it);

          delete removalQueue_[i];
        }
      }

      //Check to see if we need to move from the queued list to the active list.
      if(!queuedFaders_.empty())
      {
        for(std::list<SS_Fader*>::iterator it = queuedFaders_.begin(); it != queuedFaders_.end();)
        {
          std::map<SS_Channel*, SS_Fader*>::iterator it1 = activeFaders_.lower_bound((*it)->channel);

          //If we didn't find it in our active list, we should add it.
          if(it1 == activeFaders_.end() || (activeFaders_.key_comp()((*it)->channel, it1->first)))
          {
            //add it
            activeFaders_.insert(FaderPair((*it)->channel,*it));

            //and remove it from our queue
            queuedFaders_.erase(it++);
          }
          else 
            it++;
        }
      }

      return SOUND_GOOD;
    }

    void SS_Sound::SetName(const ch* name_)
    {
      s32 name_len;

      if (name)
      {
        delete name;
        name = NULL;
      }

      //Make sure we aren't being passed garbage
      if (! name_) return;

      //Copy over the name for us to use.
      name_len = strlen(name_);
      name = new ch [name_len + 1];
      strcpy(name, name_);
      namestr_.assign(name);
    }

    Sound::Sound() : soundID_(0), channelID_(0), 
                     system_(NULL), result_(FMOD_OK), 
                     mIDName_(H("Sound")), muted_(false)
    {
      
    }


    Sound::~Sound()
    {
      SOUND = NULL;
    }

    void ErrorCheck(FMOD_RESULT result) throw( Exception )
    {
      if (result != FMOD_OK)
        if(Application::YesNoQuestion("Well, this is embarrassing. \n" 
           "You're not gonna be able to play with sound, "
           "but do you still want to play?"))
           throw Exception(E_SOUND_NOT_INITIALIZED, "Sound failed to initialize, and the user hates us.");
    }

    void Sound::Initialize()
    {
      timer_ = new FrameTimer;
      startTime = timer_->Get();

      result_ = FMOD::System_Create(&system_);
      ErrorCheck(result_);

      result_ = system_->setOutput(FMOD_OUTPUTTYPE_DSOUND);
      ErrorCheck(result_);

      result_ = system_->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
      ErrorCheck(result_);

      CacheSoundsInPath(CONFIG_PATH_SOUND "sounds.txt");
      SOUND = this;
    }

    void Sound::Uninitialize()
    {
      UnloadSounds();
      delete timer_;

      result_ = FMOD_System_Release((FMOD_SYSTEM*)system_);
      if(result_ != FMOD_OK)
      {
        std::stringstream error;
        error << "System failed to release." << std::endl;
      }
    }

    void Sound::UnloadChannels()
    {
      //Release queued faders
      for(std::list<SS_Fader*>::iterator it = queuedFaders_.begin(); it != queuedFaders_.end(); ++it)
        delete *it;

      //Release active faders
      for(std::map<SS_Channel*, SS_Fader*>::iterator it = activeFaders_.begin(); it != activeFaders_.end(); ++it)
        delete it->second;

      //Release channels
      for(std::map<u32, SS_Channel*>::iterator it = channelTable_.begin(); it != channelTable_.end(); ++it)
        delete it->second;
    }

    void Sound::UnloadSounds()
    {
      std::map<u32, SS_Sound*>::iterator it;

      for(it = soundTable_.begin(); it != soundTable_.end(); ++it)
        delete it->second;

      UnloadChannels();
    }

    SoundResult Sound::LoadSound( const ch* soundName, SS_Sound** output, SoundMode mode, u1 streaming /*= false*/ )
    {
      startTime = timer_->Get();
      FMOD::Sound* sound;
      SS_Sound* hSound;
      FMOD_MODE fmode = FMOD_2D;

      std::string name(soundName);

      //Check the cache
      std::map<std::string, SS_Sound*>::iterator it = soundCache_.lower_bound(name);
      if(it != soundCache_.end() && !(soundCache_.key_comp()(soundName, it->first)))
      {
        hSound = it->second;
        *output = hSound;
        return SOUND_GOOD;
      }

      //Check to see if I should be looping the sound or not.
      if (mode & LOOPING)
        fmode |= FMOD_LOOP_NORMAL;
      else
        fmode |= FMOD_ACCURATETIME | FMOD_LOOP_OFF;

      if(streaming)
        result_ = system_->createStream(soundName, fmode & ~FMOD_UNIQUE, NULL, &sound);
      else
        result_ = system_->createSound(soundName, fmode & ~FMOD_UNIQUE, NULL, &sound);

      if(result_ != FMOD_OK)
        return SOUND_BAD;

      //Now we actually care about making a new, managed sound.
      hSound = new SS_Sound;
      hSound->sound = sound;
      hSound->forceUnique = (mode & UNIQUE) ? 1 : 0;
      hSound->SetName(soundName);
      hSound->ID = soundID_;
      *output = hSound;

      //Add the sound to both the management table and cache for future use.
      soundTable_.insert(SoundPair(soundID_++, hSound));
      soundCache_.insert(CachePair(std::string(hSound->name), hSound));

      return SOUND_GOOD;
    }


    SoundResult Sound::PlaySound(SS_Sound *sound, SS_Channel **output, SoundMode mode)
    {
      if(muted_)
      {
        *output = NULL;
        return SOUND_GOOD;
      }

      FMOD::Channel* channel;
      SS_Channel* hChan;
      u1 paused;

      if(!sound)
        return SOUND_BAD;

      //Check to see if the sound is already playing.
      if (sound->forceUnique && sound->myChannel)
      {
        s32 playing;
        IsSoundPlaying(sound->myChannel, &playing);
        if (playing)
        {
          *output = NULL;
          return SOUND_ALREADY_PLAYING;
        }
        else
          sound->myChannel = NULL;
      }

      //Now we care if we're paused or not. 
      // ( != 0 is to suppress warning CS4800, which does not predict bit masking)
      paused = ((mode & PAUSED) != 0);
      
      result_ = system_->playSound(FMOD_CHANNEL_FREE, sound->sound, paused, &channel);
      if(result_ != FMOD_OK)
        return SOUND_BAD;

      if(muted_)
        channel->setVolume(0.0f);

      //Now we actually care about tracking the channel.
      hChan = new SS_Channel;
      hChan->channel = channel;
      hChan->ID = channelID_;
      sound->myChannel = hChan;
      *output = hChan;

      channelTable_.insert(ChannelPair(channelID_++, hChan));
      return SOUND_GOOD;
    }


    u32 Sound::Play(const Text& file, f32 volume, u1 streaming)
    {
      const char* filePath = file.c_str();

      if(muted_)
        return 0;

      SS_Sound* sound = 0;
      SS_Channel* channel = 0;

      if(streaming)
        LoadSound(filePath, &sound, LOOPING, streaming);
      else
        LoadSound(filePath, &sound, NO_LOOP);

      PlaySound(sound, &channel, PAUSED);
      if (channel)
      {
        SetVolume(channel, volume);
        PauseSound(channel, false);
        if(streaming)
          channel->freeWhenDone = 0;
        else 
          channel->freeWhenDone = 1;
      }

      return SOUND_GOOD;
    }

    SoundResult Sound::PauseSound(SS_Channel* channel, u1 paused)
    {
      if(!channel)
        return SOUND_GOOD;

      FMOD_RESULT result = channel->channel->setPaused(paused);
      if(result != FMOD_OK)
        return SOUND_BAD;

      return SOUND_GOOD;
    }

    SoundResult Sound::PauseAll( u1 paused )
    {
      std::map<u32, SS_Channel*>::iterator it;

      for(it = channelTable_.begin(); it != channelTable_.end(); it++)
        PauseSound(it->second, paused);

      return SOUND_GOOD;
    }

    SoundResult Sound::GetPaused(SS_Channel* channel, u1* pausedOut)
    {
      if(!channel)
        return SOUND_GOOD;

      FMOD_RESULT result = channel->channel->getPaused(pausedOut);
      if(result != FMOD_OK)
        return SOUND_BAD;

      return SOUND_GOOD;
    }

    void Sound::Update()
    {
      system_->update();
      UpdateFaders();
    }

    SoundResult Sound::IsSoundPlaying(SS_Channel* channel, s32* result)
    {
      if(result)
        *result = 1;

      result_ = FMOD_Channel_IsPlaying((FMOD_CHANNEL*)(channel->channel), result);  
      if(result_ != FMOD_OK && result_ != FMOD_ERR_INVALID_HANDLE)
        return SOUND_BAD;

      return SOUND_GOOD;
    }

    SoundResult Sound::StopSound(SS_Channel* channel)
    {
      if(!channel) 
        return SOUND_GOOD;

      FMOD_RESULT result = channel->channel->stop();    
      if(result != FMOD_OK)
        return SOUND_BAD;

      return SOUND_GOOD;
    }

    u32 Sound::StopAll()
    {
      std::map<u32, SS_Channel*>::iterator it;

      for(it = channelTable_.begin(); it != channelTable_.end(); it++)
        StopSound(it->second);

      return SOUND_GOOD;
    }

    SoundResult Sound::SetMasterVolume( f32 vol )
    {
      for(std::map<u32, SS_Channel*>::iterator it = channelTable_.begin(); it != channelTable_.end(); ++it)
        SetVolume(it->second, vol);

      return SOUND_GOOD;
    }

    SoundResult Sound::SetVolume(SS_Channel* channel, f32 vol)
    {
      if(!channel) 
        return SOUND_GOOD;

      FMOD_RESULT result = channel->channel->setVolume(vol);    
      if(result != FMOD_OK)
        return SOUND_BAD;

      return SOUND_GOOD;
    }

    SoundResult Sound::GetVolume(SS_Channel* channel, f32* volOut)
    {
      if(!channel) 
        return SOUND_GOOD;

      FMOD_RESULT result = channel->channel->getVolume(volOut);    
      if(result != FMOD_OK)
        return SOUND_BAD;

      return SOUND_GOOD;
    }

    SoundResult Sound::CacheSoundsInPath(const ch* path)
    {
      FILE* file;
      ch buffer[256];
      SS_Sound* sound;
      SoundMode mode;
      u1 streaming = false;

      //Open the file, and make sure it's open
      file = fopen(path, "rt");
      if (!file) 
        return SOUND_GOOD;

      //Read in the files in this folder to be loaded, and load them.
      while (fscanf(file, "%s", buffer) != EOF)
      {
        s32 loop = 0;
        mode = NO_LOOP;

        //Parse for looping or not
        if (strcmp(buffer, "Loop:") == 0)
        {
          loop = 1;
          mode = LOOPING;
        }
        else if (strcmp(buffer, "Unique:") == 0)
          mode = UNIQUE;
        else if(strcmp(buffer, "Stream:") == 0)
        {
          loop = 1;
          mode = LOOPING;
          streaming = true;
        }

        //The next thing is the actual filepath
        if (fscanf(file, "%s", buffer) == EOF) break;

        std::string relPath(CONFIG_PATH_SOUND);
        relPath.append(buffer);

        //Cache the sound
        LoadSound(relPath.c_str(), &sound, mode, streaming);
      }

      //Close up shop, because we're done.
      fclose(file);

      return SOUND_GOOD;
    }

    SoundResult Sound::FadeSoundOut(SS_Channel* channel, s32 duration)
    {
      new SS_Fader(channel, 0.0f, duration, 1);
      return SOUND_GOOD;
    }

    SoundResult Sound::FadeSound(SS_Channel* channel, f32 target, s32 duration)
    {
      new SS_Fader(channel, target, duration, 0);
      return SOUND_GOOD;
    }

    u32 Sound::FadeAll()
    {
      for(std::map<u32, SS_Channel*>::iterator it = channelTable_.begin(); it != channelTable_.end(); ++it)
      {
        s32 playing;
        u1 paused;

        IsSoundPlaying(it->second, &playing);
        GetPaused(it->second, &paused);
        if(playing && !paused)
          FadeSoundOut(it->second, 250);
        else 
          StopSound(it->second);
      }

      return SOUND_GOOD;
    }

    u32 Sound::FadeAllToTarget(f32 target, s32 duration)
    {
      for(std::map<u32, SS_Channel*>::iterator it = channelTable_.begin(); it != channelTable_.end(); ++it)
      {
        s32 playing;
        u1 paused;

        IsSoundPlaying(it->second, &playing);
        GetPaused(it->second, &paused);
        if(playing && !paused)
          FadeSound(it->second, target, duration);
        else 
          StopSound(it->second);
      }

      return SOUND_GOOD;
    }

    //For muting/unmuting sounds in the options menu.
    SoundResult Sound::Mute()
    {
      SetMasterVolume(0.0f);
      muted_ = true;
      return SOUND_GOOD;
    }

    SoundResult Sound::Unmute()
    {
      SetMasterVolume(1.0f);
      muted_ = false;
      return SOUND_GOOD;
    }

    void Sound::vLoadSystem( void )
    {
      //Init the system
      Initialize();

      //Subscribe to any messages that may be used with the system
      gCore->Events.Subscribe(H("sound_play"), this);
      gCore->Events.Subscribe(H("sound_stream"), this);
      gCore->Events.Subscribe(H("sound_mute"), this);
      gCore->Events.Subscribe(H("sound_unmute"), this);
      gCore->Events.Subscribe(H("sound_set_master_volume"), this);
      gCore->Events.Subscribe(H("sound_fade_out"), this);
      gCore->Events.Subscribe(H("sound_fade"), this);
      gCore->Events.Subscribe(H("sound_pause"), this);

      SCRIPT_RegisterGlobal(Sound);
    }

    void Sound::vUpdateSystem( void )
    {
      Update();
    }

    void Sound::vUnloadSystem( void )
    {
      Uninitialize();
    }

    u1 Sound::handleEvent( hstring const& message, RawStore const& args, MessageType type, f32 timestamp )
    {
      if(message == H("sound_play"))
      {
        std::string name = args.GetString("soundname");
        f32 volume = args.GetF32("volume");

        Play(name.c_str(), volume);
        return true;
      }
      else if(message == H("sound_stream"))
      {
        std::string name = args.GetString("soundname");
        f32 volume = args.GetF32("volume");

        Play(name.c_str(), volume, true);
        return true;
      }
      else if(message == H("sound_mute"))
      {
        Mute();
        return true;
      }
      else if(message == H("sound_unmute"))
      {
        Unmute();
        return true;
      }
      else if(message == H("sound_set_master_volume"))
      {
        f32 volume = args.GetF32("volume");

        SetMasterVolume(volume);
        return true;
      }
      else if(message == H("sound_fade_out"))
      {
        FadeAll();
        return true;
      }
      else if(message == H("sound_fade"))
      {
        f32 volume = args.GetF32("volume");

        FadeAllToTarget(volume);
        return true;
      }
      else if(message == H("sound_pause"))
      {
        PauseAll();
        return true;
      }

      return false;
    }

    void Sound::ToggleMute()
    {
      if(muted_)
        Unmute();
      else
        Mute();
    }

}//namespace Virtus

#endif