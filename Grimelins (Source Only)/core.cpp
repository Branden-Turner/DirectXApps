/*! ====================================================================================================================
  @file core.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.03.2011

  @brief 
    Core engine.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "apputilities.h"
#include "core.h"

// #include "SQVM_Bind.h"
namespace Grimelins
{


    cvar print_messages("print_messages", "Prints all messages to console", false);
    cvar print_core_events("print_core_events", "Displays events in the core", false);
    cvar max_fps("max_fps", "Caps the framerate at given value. 0 = no cap", 0);

    namespace
    {

      ANT__RETVAL ANTgetFPS(void* destination, void* lastFrametime)
      {
        f32 dt = gCore->Time.Get() - *(f32*)lastFrametime;
        if(dt!=0)
          *(f32*)destination = 1.0f / dt;
        else
          *(f32*)destination = 1337.1337f;

      }

      ANT__RETVAL ANTPauseGame(void* vp)
      {
//         if(gCore->GamePaused)
//           ANT__Define(bar, "/pause label='Pause Game'");
//         else
//           ANT__Define(bar, "/pause label='Unpause Game'");
// 
//         gCore->GamePaused = !gCore->GamePaused;
      }

      class PrintListener : public IListener
      {
        u1 handleEvent(hstring const& msg, RawStore const& args, MessageType type, f32 timestamp)
        {
          if (!print_messages || msg == H("con_print"))
            return false;

          std::stringstream str;
          str << "Message " << msg.str() << " sent [";

          if (args.HasData())
            str << "arguments provided";
          else
            str << "no arguments";

          str << "] (at " << timestamp << ")" << std::endl;


          printDebugT("%s", str.str().c_str());

          return false; // We don't want to consume the messages, we're just listening silently
        }
      } ThePress;


    } // namespace


    Core::Core(void) :  m_Running(false), 
                        m_StopSignaled(false), 
                        m_Hz(0.0f),
                        m_FrameCount(0), 
                        m_FrameCountLong(0),
                        m_typeIDs(0)
    {
      Time.StartTimer();
      GamePaused = false;
      GameRunning = false;
    }

    Core::~Core(void)
    {
    }

//     ANT__CVAR_DECL();

    void Core::Start(RawStore const& startupinfo)
    {

      Events.initialize();

      RunCoreBinding();

      // Load systems
      Layers.loadLayers();


      m_Running = true;
      m_StopSignaled = false;

      // Output all events if configured to do so
      Events.Subscribe( AllMessages, &ThePress );

      ANT__Bar_Init(bar, "Gimelins", true);
      ANT__Bar_AddVarCB(bar, "fps", NULL, ANTgetFPS, &m_LastFrame, f32);
      ANT__Bar_AddButton_P(bar, "pause", ANTPauseGame, NULL, " label='Pause Game'");

      if (print_core_events)
        printDebug("CORE: Starting (%f Hz)\n", m_Hz);
      

      Events.PostNow(H("startup_info"), startupinfo, MSG_PRIORITY_HIGH | MSG_NOT_CONSUMABLE);
      Events.PostNow(H("core_startup"), MSG_PRIORITY_HIGH | MSG_NOT_CONSUMABLE);

    }

    void Core::NextFrame(void)
    {
      ErrorIf(!m_Running, "Attempt to update unstarted engine. Did you call update after shutdown?");

      if (!m_Running)
        return;


      if (m_StopSignaled)
      {
        // Unload systems
        Events.clearQueue();
        Layers.unloadLayers();
        m_Running = false;
        Events.uninitialize();
        return;
      }

      m_FrameCount = (u32)++m_FrameCountLong;

      if (m_Hz != 0) // 0 means uncapped frame rate
      {
        // Figure out how long to wait before next frame
        f32 dt = gCore->Time.Get() - m_LastFrame;
        Application::Sleep(m_Hz - dt);
      }

      if (print_core_events)
        printDebug("CORE: Sending messages\n");

      // Send all messages
      Events.processQueue();


      if (print_core_events)
        printDebug("CORE: Reassigning tasks to threadpools\n");

      // Process tasks
      Tasks.process();


      if (print_core_events)
        printDebug("CORE: Updating Layers\n");

      // Update systems
      Layers.updateLayers();

      m_LastFrame = gCore->Time.Get();
    }

    void Core::Stop(void)
    {
      gCore->Events.PostNow(H("core_shutdown"), MSG_PRIORITY_HIGH | MSG_NOT_CONSUMABLE);
      m_StopSignaled = true;
    }

    u1 Core::IsRunning(void) const
    {
      return m_Running;
    }

    u32 Core::GetFrameCount(void) const
    {
      return m_FrameCount;
    }

    u64 Core::GetFrameCountLong(void) const
    {
      return m_FrameCountLong;
    }

} // namespace Virtus