/*! ====================================================================================================================
  @file core.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.03.2011

  @brief 
    Core engine. Manages system and message dispatchin.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_CORE_H
#define VIRTUS_CORE_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include segment.

#include "layermanager.h"
#include "com.h"
#include "taskmanager.h"
#include "timer.h"
#include "scriptVM.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Definition Segment.
#define TYPE_ID(t) gCore->getTypeID<t>()

#define ENTER_CORE_SECURE()                                     \
  bool thisFnEnteredGlobally;                                   \
                                                                \
  do                                                            \
  {                                                             \
    if (!gCore->Safe.callInsideCore)                            \
    {                                                           \
      gCore->Safe.callInsideCore = true;                        \
      thisFnEnteredGlobally = true;                             \
      gCore->Safe.lock.Acquire();                               \
    }                                                           \
  } while(0) // Macro end

#define LEAVE_CORE_SECURE() do {                                \
    if (thisFnEnteredGlobally)                                  \
    {                                                           \
      gCore->Safe.lock.Release();                               \
      gCore->Safe.callInsideCore = false;                       \
      thisFnEnteredGlobally = false;                            \
    }                                                           \
  } while(0) // Macro end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration segment.

namespace Grimelins
{

    class EngineSafe
    {
      friend class Core;
      friend class EventHandler;
      friend class TaskManager;
      friend class ResourceMaster;
      friend class Timer;

    private:
      EngineSafe(void) : callInsideCore(false) {}
      EngineSafe& operator=(EngineSafe& rhs);

    private:
      Mutex lock;
      bool callInsideCore;
    };

    class Core
    {
    public: // aggregates.
      EngineSafe Safe;
      EventHandler Events;
      LayerManager Layers;
      TaskManager Tasks;
      ResourceMaster Resources;
      Timer Time;
      ScriptVM Scripter;
      typedef std::map<hstring, Text> EventMap;
      EventMap GameLogicEvents;
      ANT__Bar(bar);
      u1 GamePaused;
      u1 GameRunning;//defines whether or not a level is underway. Main menu would be false.

    public:
      Core(void);
      ~Core(void);

    private: // Denied
      Core(Core const& rhs);
      Core& operator=(Core const& rhs);

    public:
      void Start(RawStore const& startupinfo);
      void NextFrame(void);
      void Stop(void);
      u1 IsRunning(void) const;
      u32 GetFrameCount(void) const;
      u64 GetFrameCountLong(void) const;

    public:
      template <typename T> TypeID getTypeID(void) { static const u32 id = ++m_typeIDs; return id; };

    private:
      u1  m_Running;
      u1  m_StopSignaled;
      f32 m_Hz;
      f32 m_LastFrame;
      u32 m_FrameCount;
      u64 m_FrameCountLong;
      TypeID m_typeIDs;
    };
} // namespace Virtus


#endif