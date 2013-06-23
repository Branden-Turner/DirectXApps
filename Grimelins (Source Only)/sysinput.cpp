/*! ====================================================================================================================
  @file appinput.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 09.Jul.2011

  @brief 
    Input system.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"

#include <XInput.h>
#pragma comment(lib, "XInput.lib")

#include "sysinput.h"
#include "sysinpcontrols.h"
#include "syswindow.h"

//ick sorry had to do this for the vibration directional input
#include "scriptGlobal.h"
#include "object.h"

#define CTRL_STICK_RESOLUTION 32767.f
#define CTRL_VIBRATION_MAX 65535
#define CTRL_STICK_LEFT_DEADZONE XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / CTRL_STICK_RESOLUTION
#define MOUSE_MOVEMENT_DEADZONE 0
#define MOUSE_WHEEL_DELTA 0

namespace Grimelins
{

    static  FrameTimer vibrationTimer(false);

    SCRIPT_ClassStart(Input);
    SCRIPT_CTOR_Disabled();
    SCRIPT_Method(vibrateReset);
    SCRIPT_Method(vibratePause);
    SCRIPT_Method(vibrateUnpause);
    SCRIPT_ClassEnd();

    cvar print_input_events("print_input_events", "Prints all input events", false);

    StateBuffer Input::mStateBuffer;
    Input::ActionBindings Input::mKeyActionBindings;

//     ResourceManager<Controller> Input::mControllerComponents;

    Input* INPUT;


    Input::Input(void)
    {
    }

    Input::~Input(void)
    {
      INPUT = NULL;
    }


    void Input::vLoadSystem(void)
    {
      Window::TrackMouse();

      gCore->Events.Subscribe(H("bind"), this);
      gCore->Events.Subscribe(H("focus_lost"), this);
      gCore->Events.Subscribe(H("focus_gained"), this);

//       gCore->Resources.InitializeManager(mControllerComponents);

      XInputEnable(true);
      mVibrateLock = false;
      ErrorIf(INPUT, "multiple instances of input");
      INPUT = this;

      SCRIPT_RegisterGlobal(Input);
    }

    void Input::vUnloadSystem(void)
    {
      gCore->Events.UnsubscribeFromAll(this);

//       gCore->Resources.UninitializeManager(mControllerComponents);

      XInputEnable(false); // should disable vibration too
    }

    void Input::vUpdateSystem(void)
    {
      
      if (GetKeyState(VK_MENU) >> 15)
      {
        if (mStateBuffer.mButtonStates[INPUT_KB_ALT].state == UP)
        {
          mStateBuffer.mButtonStates[INPUT_KB_ALT].state = TRIGGER;
          mStateBuffer.mButtonStates[INPUT_KB_ALT].lastChange = gCore->GetFrameCount();
        }
        else
        {
          mStateBuffer.mButtonStates[INPUT_KB_ALT].state = DOWN;
          mStateBuffer.mButtonStates[INPUT_KB_ALT].lastChange = gCore->GetFrameCount();
        }
      }
      else if (mStateBuffer.mButtonStates[INPUT_KB_ALT].state != UP)
      {
        mStateBuffer.mButtonStates[INPUT_KB_ALT].state = RELEASE;
        mStateBuffer.mButtonStates[INPUT_KB_ALT].lastChange = gCore->GetFrameCount();
      }

      updateMouse();
      updateControllers();

      StateBuffer::ButtonStates::iterator it = mStateBuffer.mButtonStates.begin();
      StateBuffer::ButtonStates::iterator endIt = mStateBuffer.mButtonStates.end();

      for (; it != endIt; ++it)
      {
        // Dispatch logical events
        fireKeyActionBindings(it->first);

        // Debugging Code
        if (print_input_events)
        {
          if (it->second.state == KeyState(RELEASE))
            printDebug("%s released\n", it->first.str());

          if (it->second.state == KeyState(TRIGGER))
            printDebug("%s trigger\n", it->first.str());

          if (it->second.state == KeyState(DOWN))
            printDebug("%s down\n", it->first.str());
        }

        // Reset key states
        switch (it->second.state)
        {
        case KeyState(RELEASE):
          it->second.state = KeyState(UP);
          break;
        case KeyState(TRIGGER):
          it->second.state = KeyState(DOWN);
          break;
        }
      }

      mStateBuffer.mMouseWheelDelta = 0;
//       updateControllerComponents();
    }

    hstring Input::vGetSystemID(void) const
    {
      return H("Input");
    }

    u1 Input::handleEvent(hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp)
    {      
      if (message == H("bind"))
      {
        if (arguments.HasData())
        {
          hstring key = arguments.GetString("key");
          hstring action = arguments.GetString("action");
          u8 state = arguments.GetU8("state");

          KeyState st;
          
          if      (state == 't') st = TRIGGER;
          else if (state == 'd') st = DOWN;
          else if (state == 'r') st = RELEASE;
          else    Error("Incorrectly formatted bind message received");

          ErrorIf(key.empty() || action.empty(), "Error: Wrongly formatted bind command sent");
          bindAction(key, action, st);
        }
        else Error("Error: Wrongly formatted bind command sent");

        return true;
      }

      if (message == H("focus_lost"))
      {
        XInputEnable(false);
        mStateBuffer.Reset();
        mStateBuffer.mHasFocus = false;
        return false;
      }

      if (message == H("focus_gained"))
      {
        XInputEnable(true);
        mStateBuffer.mHasFocus = true;
        return false;
      }


      return false;
    }

    bool Input::parseCommand(hstring const& command, std::vector<std::string> const& arguments)
    {
      if (command == H("bind") || command == H("unbind"))
      {
        u32 size = arguments.size();

        if (size < 3)
        {
          printConsole("Invalid format for bind command");
          return true;
        }

          std::vector<hstring> combination;
          std::string action;
          u32 lastAdd = true;
          KeyState state = KeyState(TRIGGER);

          for (u32 i = 1; i < size; ++i)
          {
            if (lastAdd)
            {
              combination.push_back(H(arguments[i].c_str()));
              lastAdd = false;
            }
            else if (arguments[i] == "+")
              lastAdd = true;
            else if (i == size - 1)
            {
              action = arguments[i];

              if (action[0] == '+')
              {
                state = DOWN;
                action.erase(0, 1);
              }
              else if (action[0] == '-')
              {
                state = RELEASE;
                action.erase(0, 1);
              }

            }
            else 
            {
              printConsole("Invalid format for command");
              return true;
            }
          }

          if (command == H("bind"))
          {
            if (combination.size() == 1)
              bindAction(combination[0], H(action.c_str()), state);
            else
              bindAction(combination, H(action.c_str()), state);

            gCore->GameLogicEvents[H(action.c_str())] = action;

            printConsole("Bound key-action relationship sucessfully");
          }
          else
          {
            if (combination.size() == 1)
              unbindAction(combination[0], H(action.c_str()), state);
            else
              unbindAction(combination, H(action.c_str()), state);

            printConsole("Unbound key-action relationship sucessfully");
          }
          return true;
      }

      if(command == H("event"))
      {
        if(arguments.size() < 2)
        {
          printConsole("Invalid format for event registration");
          return true;
        }
        //assumes startupComplete is the last event defined in userconfig.cfg
        //once this EventsReady message is dispatched, the factory will begin
        //serializing objects and behaviors. Because behaviors depend on 
        //GameLogicEvents, they must be serialized afterward, this addresses
        //that dependency
        if(arguments[1] == "startupComplete")
          gCore->Events.Post(H("EventsReady"), MSG_DEFAULT);
        else
          gCore->GameLogicEvents[H(arguments[1].c_str())] = arguments[1];
        return true;
      }

      if (command == H("unbindall"))
      {
        if (arguments.size() < 2)
        {
          printConsole("Not enough parameters given to unbindall");
          return true;
        }

        unbindActionFromAll(arguments[1]);
        return true;
      }


      return false;
    }

    u1 Input::handleRequest(hstring const& request, RawStore& data)
    {
      if (request == H("input_claim_focus"))
      {
        
        Handle listener = Handle::Construct(data.GetU32("subscriber"));

        if (gCore->Events.IsListener(listener))
          mFocusSubscriber = listener;

        return true;
      }

      if (request == H("input_release_focus"))
      {
        mFocusSubscriber = Handle();
        return true;
      }

//       if( request == H("createController") )
//       {
//         Handle newHandle = createControllerComponent(data);
//         data.SetHandle("newCmpController", newHandle);
//         return true;
//       }

      return false;
    }

    void Input::bindAction(hstring const& key, hstring const& action, KeyState state)
    {
      BoundAction bndact;
      bndact.action = action;
      bndact.state = state;

      ErrorIf(std::find(mKeyActionBindings[key].begin(), mKeyActionBindings[key].end(), bndact) != mKeyActionBindings[key].end(),
        "Warning: Attempt to bind identical action (%s) to same key (%s) multiple times", action.str(), key.str());

      mKeyActionBindings[key].push_back(bndact);
    }

    void Input::bindAction(std::vector<hstring> const& combination, hstring const& action, KeyState state)
    {
      u32 size = combination.size();

      if (size == 0)
        return;
      else if (size == 1)
      {
        bindAction(combination[0], action, state);
        return;
      }

      hstring key = combination[0];
      BoundAction bndact;
      bndact.action = action;
      bndact.state = state;

      std::vector<hstring>::const_iterator it = ++combination.begin();
      std::vector<hstring>::const_iterator endIt = combination.end();
      for (; it != endIt; ++it) bndact.additonalKeys.push_back(*it);


      ErrorIf(std::find(mKeyActionBindings[key].begin(), mKeyActionBindings[key].end(), bndact) != mKeyActionBindings[key].end(),
        "Warning: Attempting to bind identical action (%s) to same key combination (%s, %s, ...) multiple times.", action.str(), key.str(), combination[1].str());

      mKeyActionBindings[key].push_back(bndact);
    }

    void Input::unbindAction(hstring const& key, hstring const& action, KeyState state)
    {
      BoundAction bndact;
      bndact.action = action;
      bndact.state = state;

      ErrorIf(std::find(mKeyActionBindings[key].begin(), mKeyActionBindings[key].end(), bndact) == mKeyActionBindings[key].end(),
        "Warning: Attempt to unbind action %s that is not bound (key: %s)", action.str(), key.str());

      std::remove(mKeyActionBindings[key].begin(), mKeyActionBindings[key].end(), bndact);
    }

    void Input::unbindAction(std::vector<hstring> const& combination, hstring const& action, KeyState state)
    {
      u32 size = combination.size();

      if (size == 0)
        return;
      else if (size == 1)
      {
        unbindAction(combination[0], action, state);
        return;
      }

      BoundAction bndact;
      hstring key = combination[0];
      bndact.action = action;
      bndact.state = state;
      std::vector<hstring>::const_iterator it = ++combination.begin();
      std::vector<hstring>::const_iterator endIt = combination.end();
      for (; it != endIt; ++it) bndact.additonalKeys.push_back(*it);

      ErrorIf(std::find(mKeyActionBindings[key].begin(), mKeyActionBindings[key].end(), bndact) == mKeyActionBindings[key].end(),
        "Warning: Attempt to unbind action %s that is not bound ( to key combination: %s, %s, ...)", action.str(), combination[0].str(), combination[1].str());

      std::remove(mKeyActionBindings[key].begin(), mKeyActionBindings[key].end(), bndact);
    }

    void Input::unbindActionFromAll(hstring const& action)
    {
      ActionBindings::iterator it = mKeyActionBindings.begin();
      ActionBindings::iterator endIt = mKeyActionBindings.end();

      for (; it != endIt; ++it)
        std::remove(it->second.begin(), it->second.end(), action);
    }


    void Input::unbindAllActionsFromKey(hstring const& key)
    {
      std::remove_if(mKeyActionBindings[key].begin(), mKeyActionBindings[key].end(), &IsNotCombination);
    }

    void Input::unbindAllActionsFromKey(std::vector<hstring> combination)
    {
      std::remove_if(mKeyActionBindings[combination[0]].begin(), mKeyActionBindings[combination[0]].end(), &IsCombination);
    }


    void Input::fireKeyActionBindings(hstring const& key)
    {
      if (mKeyActionBindings[key].empty())
        return;

      vec4 sticks(
        mStateBuffer.mCtrlLeftStick.x, 
        mStateBuffer.mCtrlLeftStick.y, 
        mStateBuffer.mCtrlRightStick.x, 
        mStateBuffer.mCtrlRightStick.y
        );

      vec4 stickdeltas(
        mStateBuffer.mCtrlLeftStickDelta.x,
        mStateBuffer.mCtrlLeftStickDelta.y,
        mStateBuffer.mCtrlRightStickDelta.x,
        mStateBuffer.mCtrlRightStickDelta.y        
        );


      RawStore arguments;
      //Fitz:  changed all types to primitive Int(s32), float(f32) because script
      //       doesn't understand the others (unsigned etc, 8,16 etc)

      //Mouse position
      arguments.AddS32("mouse_x", (s32)mStateBuffer.mMousePosition.x);
      arguments.AddS32("mouse_y", (s32)mStateBuffer.mMousePosition.y);

      //Mouse delta
      arguments.AddS32("mouse_dx", (s32)mStateBuffer.mMouseDelta.x);
      arguments.AddS32("mouse_dy", (s32)mStateBuffer.mMouseDelta.y);
      arguments.AddS32("wheel_d", (s32)mStateBuffer.mMouseWheelDelta);

      //LR Trigger position
      arguments.AddF32("ltrigger", mStateBuffer.mCtrlLeftTrigger);
      arguments.AddF32("rtrigger", mStateBuffer.mCtrlRightTrigger);

      //LR Trigger delta
      arguments.AddF32("ltrigger_d", mStateBuffer.mCtrlLeftTriggerDelta);
      arguments.AddF32("rtrigger_d", mStateBuffer.mCtrlRightTriggerDelta);

      //LR Stick position
      arguments.AddF32("lstick_x", mStateBuffer.mCtrlLeftStick.x);
      arguments.AddF32("lstick_y", mStateBuffer.mCtrlLeftStick.y);
      arguments.AddF32("rstick_x", mStateBuffer.mCtrlRightStick.x);
      arguments.AddF32("rstick_y", mStateBuffer.mCtrlRightStick.y);

      //LR Stick delta
      arguments.AddF32("lstick_dx", mStateBuffer.mCtrlLeftStickDelta.x);
      arguments.AddF32("lstick_dy", mStateBuffer.mCtrlLeftStickDelta.y);
      arguments.AddF32("rstick_dx", mStateBuffer.mCtrlRightStickDelta.x);
      arguments.AddF32("rstick_dy", mStateBuffer.mCtrlRightStickDelta.y);

      std::vector<BoundAction>::iterator it = mKeyActionBindings[key].begin();
      std::vector<BoundAction>::iterator endIt = mKeyActionBindings[key].end();

      for (; it != endIt; ++it)
      {
        if (!it->additonalKeys.empty()) // Keyboard combination subscription
        {
          u32 numUp = 0;
          u32 numTrigger = 0;
          u32 numDown = 0;
          u32 numRelease = 0;


          KeyState firstState = mStateBuffer.mButtonStates[key].state;


          if (firstState == UP)           ++numUp;
          else if (firstState == TRIGGER) ++numTrigger;
          else if (firstState == DOWN)    ++numDown;
          else if (firstState == RELEASE) ++numRelease;


          std::vector<hstring>::const_iterator keysIt = it->additonalKeys.begin();
          std::vector<hstring>::const_iterator keysEndIt = it->additonalKeys.end();

          for (; keysIt != keysEndIt; ++keysIt)
          {
            KeyState currState = mStateBuffer.mButtonStates[*keysIt].state;

            if (currState == UP)            { ++numUp; break; }
            else if (currState == TRIGGER)  { ++numTrigger;   }
            else if (currState == DOWN)     { ++numDown;      }
            else if (currState == RELEASE)  { ++numRelease;   }
          }

          if (numUp) 
            continue;
          
          KeyState combinationState;

          if (numRelease && numTrigger == 0)            combinationState = RELEASE;
          else if (numTrigger && numRelease == 0)       combinationState = TRIGGER;
          else if (numTrigger == 0 && numRelease == 0)  combinationState = DOWN;
          else combinationState = UP;

          if (combinationState != it->state)
            continue;

          if(it->action == H("move_right"))
          {
            int i = 5;
          }

          if (mFocusSubscriber)
            gCore->Events.PostTo(mFocusSubscriber, it->action, arguments, MSG_TYPE_NETWORK | MSG_PRIORITY_HIGH);
          else
            gCore->Events.Post(it->action, arguments, MSG_TYPE_NETWORK | MSG_PRIORITY_HIGH);

        }
        else // Single key subscription
        {
          if (mStateBuffer.mButtonStates[key].state != it->state)
            continue;

          if (mFocusSubscriber)
            gCore->Events.PostTo(mFocusSubscriber, it->action, arguments, MSG_TYPE_NETWORK | MSG_PRIORITY_HIGH);
          else
            gCore->Events.Post(it->action, arguments, MSG_TYPE_NETWORK | MSG_PRIORITY_HIGH);
        }
      }
    }

    //////////////////////////////////////////////////////////////////////////
    // Controller Related

    void Input::translateAndUpdateControllerButton(u16 button, u32 XName, hstring const& VName)
    {
      KeyState currentState = mStateBuffer.mButtonStates[VName].state;

      
      if (button & XName)
      {
        if (currentState == UP)
        {
          mStateBuffer.mButtonStates[VName].state = TRIGGER;
          mStateBuffer.mButtonStates[VName].lastChange = gCore->GetFrameCount();
        }
      }
      else if (currentState == DOWN)
      {
        mStateBuffer.mButtonStates[VName].state = RELEASE;
        mStateBuffer.mButtonStates[VName].lastChange = gCore->GetFrameCount();
      }
    }

    void Input::updateMouse(void)
    {
      // Update mouse position
      POINT pos;

      static s32 mDesktopWidth = GetSystemMetrics(SM_CXSCREEN);
      static s32 mDesktopHeight = GetSystemMetrics(SM_CYSCREEN);

      if (mStateBuffer.mHasFocus)
      {
        GetCursorPos(&pos);
        //MOUSE WRAP CODE
//         const int mov = 3;
//         const int chk = 1;
// 
//         if( pos.x <= chk )
//         {
//           SetCursorPos(mDesktopWidth - mov, pos.y);
//           mStateBuffer.mMousePosition.x = mDesktopWidth - mov;
//         }
//         else if( mDesktopWidth - chk <= pos.x )
//         {
//           SetCursorPos(mov, pos.y);
//           mStateBuffer.mMousePosition.x = mov;
//         }
//         if( pos.y <= chk )
//         {
//           SetCursorPos(pos.x, mDesktopHeight - mov);
//           mStateBuffer.mMousePosition.y = mDesktopHeight - mov;
//         }
//         else if( mDesktopHeight - chk <= pos.y )
//         {
//           SetCursorPos(pos.x, mov);
//           mStateBuffer.mMousePosition.y = mov;
//         }
// 
//         GetCursorPos(&pos);

        mStateBuffer.mMouseDelta.x = pos.x - mStateBuffer.mMousePosition.x;
        mStateBuffer.mMouseDelta.y = pos.y - mStateBuffer.mMousePosition.y;
        mStateBuffer.mMousePosition.x = pos.x;
        mStateBuffer.mMousePosition.y = pos.y;
      }
      else
      {
        mStateBuffer.mMouseDelta.x = 0;
        mStateBuffer.mMouseDelta.y = 0;
      }


      u1 mouseChange = false;

      // Update x-axis mouse movement states
      if (mStateBuffer.mMouseDelta.x > MOUSE_MOVEMENT_DEADZONE || mStateBuffer.mMouseDelta.x < -MOUSE_MOVEMENT_DEADZONE)
      {
        mouseChange = true;
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_X].state = KeyState(TRIGGER);
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_X].lastChange = gCore->GetFrameCount();
      }
      else if (mStateBuffer.mButtonStates[INPUT_MS_MOVE_X].state != KeyState(UP))
      {
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_X].state = KeyState(RELEASE);
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_X].lastChange = gCore->GetFrameCount();
      }

      // Update y-axis mouse movement states
      if (mStateBuffer.mMouseDelta.y > MOUSE_MOVEMENT_DEADZONE || mStateBuffer.mMouseDelta.y < -MOUSE_MOVEMENT_DEADZONE)
      {
        mouseChange = true;
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_Y].state = KeyState(TRIGGER);
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_Y].lastChange = gCore->GetFrameCount();
      }
      else if (mStateBuffer.mButtonStates[INPUT_MS_MOVE_Y].state != KeyState(UP))
      {
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_Y].state = KeyState(RELEASE);
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_Y].lastChange = gCore->GetFrameCount();
      }

      // Update xy-axis mouse movement states
      if (mouseChange)
      {
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_XY].state = KeyState(TRIGGER);
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_XY].lastChange = gCore->GetFrameCount();
      }
      else if (mStateBuffer.mButtonStates[INPUT_MS_MOVE_XY].state != KeyState(UP))
      {
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_XY].state = KeyState(RELEASE);
        mStateBuffer.mButtonStates[INPUT_MS_MOVE_XY].lastChange = gCore->GetFrameCount();
      }

      // Update mouse wheel state
      if (mStateBuffer.mMouseWheelDelta > MOUSE_WHEEL_DELTA || mStateBuffer.mMouseWheelDelta < -MOUSE_WHEEL_DELTA)
      {
        mStateBuffer.mButtonStates[INPUT_MS_WHEEL].state = KeyState(TRIGGER);
        mStateBuffer.mButtonStates[INPUT_MS_WHEEL].lastChange = gCore->GetFrameCount();
      }
      else if (mStateBuffer.mButtonStates[INPUT_MS_WHEEL].state != KeyState(UP))
      {
        mStateBuffer.mButtonStates[INPUT_MS_WHEEL].state = KeyState(RELEASE);
        mStateBuffer.mButtonStates[INPUT_MS_WHEEL].lastChange = gCore->GetFrameCount();
      }
    }

    void Input::updateVibration( void )
    {
      Vibrations::iterator it = mVibrations.begin();
      Vibrations::iterator eit = mVibrations.end();
      f32 currentTime = gCore->Time.Get();

      f32 lfFinal=0;//low frequency
      f32 hfFinal=0;//high frequency

      //find which vibrations are able to operate
      while(it!=eit && it->timestart < currentTime)
      {
        if(it->timeend < currentTime)
        {
          Vibrations::iterator rem = it;
          ++it;
          mVibrations.erase(rem);
          mNewVibration = true;
          continue;
        }
        /// STEP 1 : Left / Right Distance bias
        //rumble setup:

        // -------d1 --------- d2 ---------------------- d3 -------- dMax
        //       lf(100%)    hf(100%)                   lf(0%)       hf(0%)

        static const f32 d1 = 450;
        static const f32 d2 = 550;
        static const f32 d3 = 1500;
        static const f32 dMax = 2600;
        static const f32 hrange = dMax-d2;
        static const f32 lrange = d3-d1;

        Object* p = SCRIPTGLOBALS->getPlayer1();
        vec3 v = (p!=NULL) ? p->position : vec3();
        v.y=0;

        f32 dist = (it->source.x==0&&it->source.y==0&&it->source.z==0) ? 0.0f : (v - it->source).Magnitude();
        f32 lf = (dist<d1) ? 1.0f : (dist>d3) ? 0.0f : 1.0f - (dist-d1)/lrange;
        f32 hf = (dist<d2) ? 1.0f : (dist>dMax) ? 0.0f : 1.0f - (dist-d2)/hrange;
        //// STEP 2 : Fade-in / Fade-out bias
        f32 fade;
        if(currentTime < it->inTime)
          fade = (it->fadein==0) ? 1.0f : (currentTime - it->timestart) / it->fadein;
        else if(currentTime > it->outTime)
          fade = (it->fadeout==0) ? 1.0f : (it->timeend - currentTime) / it->fadeout;
        else
          fade = 1.0f;
        lfFinal += (1.0f - lfFinal) * lf * fade *  it->magnitude;
        hfFinal += (1.0f - hfFinal) * hf * fade * it->magnitude;


        ++it;
        mNewVibration = true;
      }
      if(mNewVibration)
      {
        XINPUT_VIBRATION vibSet;
        vibSet.wLeftMotorSpeed = (u16)(lfFinal * CTRL_VIBRATION_MAX);
        vibSet.wRightMotorSpeed = (u16)(hfFinal * CTRL_VIBRATION_MAX);
        // Will return error code when no (capable) device is connected, we don't care
        XInputSetState(0, &vibSet);
        RawStore rs;
        rs.AddF32("avg", (lfFinal+hfFinal)/2.0f);
        rs.AddF32("hf", hfFinal);
        rs.AddF32("lf", lfFinal);
        gCore->Events.PostNow(H("Vibration"), rs);
        mNewVibration = false;
      }
    }

    void Input::updateControllers(void)
    {
      updateVibration();
      XINPUT_STATE State;

      u32 result = XInputGetState(0, &State);

      if (result == ERROR_SUCCESS)
      {
        // This controller is connected, update the key state buffer
        if (State.dwPacketNumber == mLastControllerPackage) // Indicates that nothing changed since last check
          return;

        mLastControllerPackage = State.dwPacketNumber;


        // Update buttons
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, INPUT_CTRL_DPAD_UP);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, INPUT_CTRL_DPAD_DOWN);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, INPUT_CTRL_DPAD_LEFT);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, INPUT_CTRL_DPAD_RIGHT);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_START, INPUT_CTRL_START);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, INPUT_CTRL_BACK);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, INPUT_CTRL_THUMB_LEFT);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, INPUT_CTRL_THUMB_RIGHT);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, INPUT_CTRL_SHOULDER_LEFT);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, INPUT_CTRL_SHOULDER_RIGHT);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_A, INPUT_CTRL_A);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_B, INPUT_CTRL_B);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_X, INPUT_CTRL_X);
        translateAndUpdateControllerButton(State.Gamepad.wButtons, XINPUT_GAMEPAD_Y, INPUT_CTRL_Y);


        // Update left and right triggers
        if (State.Gamepad.bLeftTrigger > 0)
        {
          if (mStateBuffer.mCtrlLeftTrigger == 0.0f)
          {
            mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_LEFT].state = TRIGGER;
            mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_LEFT].lastChange = gCore->GetFrameCount();
          }
        }
        else if (mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_LEFT].state == DOWN)
        {
          mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_LEFT].state = RELEASE;
          mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_LEFT].lastChange = gCore->GetFrameCount();
        }

        if (State.Gamepad.bRightTrigger > 0)
        {
          if (mStateBuffer.mCtrlRightTrigger == 0.0f)
          {
            mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_RIGHT].state = TRIGGER;
            mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_RIGHT].lastChange = gCore->GetFrameCount();
          }
        }
        else if (mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_RIGHT].state == DOWN)
        {
          mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_RIGHT].state = RELEASE;
          mStateBuffer.mButtonStates[INPUT_CTRL_TRIGGER_RIGHT].lastChange = gCore->GetFrameCount();
        }


        mStateBuffer.mCtrlLeftTriggerDelta = mStateBuffer.mCtrlLeftTrigger - (State.Gamepad.bLeftTrigger / 255.0f);
        mStateBuffer.mCtrlLeftTrigger = State.Gamepad.bLeftTrigger / 255.0f;
        mStateBuffer.mCtrlLeftStickDelta = mStateBuffer.mCtrlRightStickDelta - (State.Gamepad.bRightTrigger / 255.0f);
        mStateBuffer.mCtrlRightTrigger = State.Gamepad.bRightTrigger / 255.0f;


        // Update Thumbstick information

        vec2 leftStick, rightStick;
        leftStick.x = State.Gamepad.sThumbLX / CTRL_STICK_RESOLUTION;
        leftStick.y = State.Gamepad.sThumbLY / CTRL_STICK_RESOLUTION;
        rightStick.x = State.Gamepad.sThumbRX / CTRL_STICK_RESOLUTION;
        rightStick.y = State.Gamepad.sThumbRY / CTRL_STICK_RESOLUTION;

        f32 deadzoneLeft = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / CTRL_STICK_RESOLUTION;

        // Clamp values for deadzone
        if (leftStick.x > -CTRL_STICK_LEFT_DEADZONE && leftStick.x  < CTRL_STICK_LEFT_DEADZONE)
          leftStick.x  = 0.f;
        if (leftStick.y > -CTRL_STICK_LEFT_DEADZONE && leftStick.y < CTRL_STICK_LEFT_DEADZONE)
          leftStick.y = 0.f;
        if (rightStick.x > -CTRL_STICK_LEFT_DEADZONE && rightStick.x < CTRL_STICK_LEFT_DEADZONE)
          rightStick.x = 0.f;
        if (rightStick.y > -CTRL_STICK_LEFT_DEADZONE && rightStick.y < CTRL_STICK_LEFT_DEADZONE)
          rightStick.y = 0.f;



        if (mStateBuffer.mCtrlLeftStick.LengthSq() == 0 && leftStick.LengthSq())
        {
          mStateBuffer.mButtonStates[INPUT_CTRL_STICK_LEFT].state = TRIGGER;
          mStateBuffer.mButtonStates[INPUT_CTRL_STICK_LEFT].lastChange = gCore->GetFrameCount();

          if(leftStick.x > 0.5f)
          {
            mStateBuffer.mButtonStates[INPUT_STICK_LEFT_RIGHT].state = TRIGGER;
            mStateBuffer.mButtonStates[INPUT_STICK_LEFT_RIGHT].lastChange = gCore->GetFrameCount();
          }
          if(leftStick.x < -0.5f)
          {
            mStateBuffer.mButtonStates[INPUT_STICK_LEFT_LEFT].state = TRIGGER;
            mStateBuffer.mButtonStates[INPUT_STICK_LEFT_LEFT].lastChange = gCore->GetFrameCount();
          }
          if(leftStick.y > 0.5f)
          {
            mStateBuffer.mButtonStates[INPUT_STICK_LEFT_FORWARD].state = TRIGGER;
            mStateBuffer.mButtonStates[INPUT_STICK_LEFT_FORWARD].lastChange = gCore->GetFrameCount();
          }
          if(leftStick.y < -0.5f)
          {
            mStateBuffer.mButtonStates[INPUT_STICK_LEFT_BACKWARD].state = TRIGGER;
            mStateBuffer.mButtonStates[INPUT_STICK_LEFT_BACKWARD].lastChange = gCore->GetFrameCount();
          }
        }
        else if (mStateBuffer.mButtonStates[INPUT_CTRL_STICK_LEFT].state == DOWN && leftStick.LengthSq() == 0)
        {
          mStateBuffer.mButtonStates[INPUT_CTRL_STICK_LEFT].state = RELEASE;
          mStateBuffer.mButtonStates[INPUT_CTRL_STICK_LEFT].lastChange = gCore->GetFrameCount();

          mStateBuffer.mButtonStates[INPUT_STICK_LEFT_BACKWARD].state = RELEASE;
          mStateBuffer.mButtonStates[INPUT_STICK_LEFT_BACKWARD].lastChange = gCore->GetFrameCount();
          mStateBuffer.mButtonStates[INPUT_STICK_LEFT_RIGHT].state = RELEASE;
          mStateBuffer.mButtonStates[INPUT_STICK_LEFT_RIGHT].lastChange = gCore->GetFrameCount();
          mStateBuffer.mButtonStates[INPUT_STICK_LEFT_LEFT].state = RELEASE;
          mStateBuffer.mButtonStates[INPUT_STICK_LEFT_LEFT].lastChange = gCore->GetFrameCount();
          mStateBuffer.mButtonStates[INPUT_STICK_LEFT_FORWARD].state = RELEASE;
          mStateBuffer.mButtonStates[INPUT_STICK_LEFT_FORWARD].lastChange = gCore->GetFrameCount();
        }

        if (mStateBuffer.mCtrlRightStick.LengthSq() == 0 && rightStick.LengthSq())
        {
          mStateBuffer.mButtonStates[INPUT_CTRL_STICK_RIGHT].state = TRIGGER;
          mStateBuffer.mButtonStates[INPUT_CTRL_STICK_RIGHT].lastChange = gCore->GetFrameCount();
        }
        else if (mStateBuffer.mButtonStates[INPUT_CTRL_STICK_RIGHT].state == DOWN && rightStick.LengthSq() == 0)
        {
          mStateBuffer.mButtonStates[INPUT_CTRL_STICK_RIGHT].state = RELEASE;
          mStateBuffer.mButtonStates[INPUT_CTRL_STICK_RIGHT].lastChange = gCore->GetFrameCount();
        }

        mStateBuffer.mCtrlLeftStickDelta = mStateBuffer.mCtrlLeftStick - leftStick;
        mStateBuffer.mCtrlRightStickDelta = mStateBuffer.mCtrlRightStick - rightStick;
        mStateBuffer.mCtrlLeftStick = leftStick;
        mStateBuffer.mCtrlRightStick = rightStick;
         
      }
    }

    void Input::vibrateAdd(f32 mag, f32 dur, f32 delay, f32 fadeIn, f32 fadeOut, vec3 const& source)
    {
      ErrorIf(mag>1||mag<0||dur<0||delay<0||fadeIn<0||fadeOut<0||fadeIn+fadeOut>1, \
        "Illigal vibration parameters.\n\
        Magnitude: %2.2f [0.0 - 1.0]\n\
        Duration: %2.2f [0.0 +]\n\
        Delay: %2.2f [0.0 +]\n\
        fadeIn: %2.2f [0.0 - 1.0]**\n\
        fadeOut: %2.2f [0.0 - 1.0]**\n\
        In+Out: %2.2f [0.0 - 1.0]\n\
        ** fadeIn + fadeOut must be less than 1.0\n\n\
        Addtional info:\n\
        source: %2.2f, %2.2f, %2.2f\n\
        distance: %2.2f\n\
        ", mag, dur, delay, fadeIn, fadeOut, fadeIn+fadeOut, source.x, source.y, source.z, (SCRIPTGLOBALS->getPlayer1()->getPosition()-source).Magnitude() );

      if(mVibrateLock)
        return;

      Vibration v;
      v.magnitude = mag;
      v.timestart = gCore->Time.Get() + delay;
      v.timeend = v.timestart + dur;
      v.fadein = fadeIn*dur;
      v.fadeout = fadeOut*dur;
      v.inTime = v.fadein + v.timestart;
      v.outTime = v.timeend - v.fadeout;
      v.source = source;
      mVibrations.insert(v);
    }

    void Input::vibrateReset( void )
    {
      mVibrations.clear();
      mVibrateLock = false;
      mNewVibration = true;
    }
    void Input::vibratePause( void )
    {
      mVibratePauseTime = gCore->Time.Get();
    }
    void Input::vibrateUnpause( void )
    {
      f32 newTime = gCore->Time.Get() - mVibratePauseTime;
      mVibratePauseTime = 0;
      Vibrations::iterator it = mVibrations.begin();
      Vibrations::iterator eit = mVibrations.end();
      std::vector<Vibration> newSet;
      while(it!=eit)
      {
        newSet.push_back( Vibration (*it) );
        newSet.back().inTime += newTime;
        newSet.back().outTime += newTime;
        newSet.back().timeend += newTime;
        newSet.back().timestart += newTime;
        ++it;
      }
      mVibrations.clear();
      for(size_t i = 0; i < newSet.size(); ++i)
        mVibrations.insert(newSet[i]);
      mVibrateLock = false;
    }

    void Input::vibrateSet(f32 mag, f32 dur, f32 delay, f32 fadeIn, f32 fadeOut, vec3 const& source)
    {
      vibrateReset();
      vibrateAdd(mag, dur, delay, fadeIn, fadeOut, source);
      mVibrateLock = true;
    }


    //////////////////////////////////////////////////////////////////////////
    // Keyboard Related

    // Helper macros for input messages
#define CONTEXTKEYCODE(lparam) (lparam & 0x20000000)
#define PREVKEYSTATE(lparam) (lParam & 0x40000000)
#define TRANSKEYSTATE(lparam) (lparam & 0x80000000)
#define KBUPDATEKEYSTATEBUFFER(key) do { \
      if (MSG == WM_KEYUP || MSG == WM_SYSKEYUP) mStateBuffer.mButtonStates[key].state = KeyState(RELEASE); \
      else if (mStateBuffer.mButtonStates[key].state != KeyState(DOWN)) mStateBuffer.mButtonStates[key].state = KeyState(TRIGGER); \
      mStateBuffer.mButtonStates[key].lastChange = gCore->GetFrameCount(); \
    } while(0)

#define MSUPDATEKEYSTATEBUFFER(key, keyState) do { \
      mStateBuffer.mButtonStates[key].state = keyState; \
      mStateBuffer.mButtonStates[key].lastChange = gCore->GetFrameCount(); \
    } while(0)

    
    void Input::HandleInputMessage(UINT MSG, WPARAM wParam, LPARAM lParam)
    {
      switch (MSG)
      {

        // Mouse.


      case WM_MOUSEWHEEL:
        mStateBuffer.mMouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        break;


      case WM_MOUSEMOVE:
        if (!mStateBuffer.mMouseOverWindow)
        {
          mStateBuffer.mMouseOverWindow = true;
          Window::TrackMouse();
        }
        break;


      case WM_LBUTTONUP:
        if (mStateBuffer.mButtonStates[INPUT_MS_LBUTTON].state != KeyState(UP))
          MSUPDATEKEYSTATEBUFFER(INPUT_MS_LBUTTON, KeyState(RELEASE));
        break;

      case WM_LBUTTONDOWN:
        if (mStateBuffer.mButtonStates[INPUT_MS_LBUTTON].state == KeyState(TRIGGER))
          MSUPDATEKEYSTATEBUFFER(INPUT_MS_LBUTTON, KeyState(DOWN));
        else
          MSUPDATEKEYSTATEBUFFER(INPUT_MS_LBUTTON, KeyState(TRIGGER));
        break;

      case WM_RBUTTONUP:
        if (mStateBuffer.mButtonStates[INPUT_MS_RBUTTON].state != KeyState(UP))
          MSUPDATEKEYSTATEBUFFER(INPUT_MS_RBUTTON, KeyState(RELEASE));
        break;

      case WM_RBUTTONDOWN:
        if (mStateBuffer.mButtonStates[INPUT_MS_RBUTTON].state == KeyState(TRIGGER))
          MSUPDATEKEYSTATEBUFFER(INPUT_MS_RBUTTON, KeyState(DOWN));
        else
          MSUPDATEKEYSTATEBUFFER(INPUT_MS_RBUTTON, KeyState(TRIGGER));
        break;

      case WM_MBUTTONUP:
        if (mStateBuffer.mButtonStates[INPUT_MS_MBUTTON].state != KeyState(UP))
          MSUPDATEKEYSTATEBUFFER(INPUT_MS_MBUTTON, KeyState(RELEASE));
        break;

      case WM_MBUTTONDOWN:
        if (mStateBuffer.mButtonStates[INPUT_MS_MBUTTON].state == KeyState(TRIGGER))
          MSUPDATEKEYSTATEBUFFER(INPUT_MS_MBUTTON, KeyState(DOWN));
        else
          MSUPDATEKEYSTATEBUFFER(INPUT_MS_MBUTTON, KeyState(TRIGGER));
        break;

      case WM_XBUTTONUP:
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
        {
          if (mStateBuffer.mButtonStates[INPUT_MS_BBUTTON].state != KeyState(UP))
            MSUPDATEKEYSTATEBUFFER(INPUT_MS_BBUTTON, KeyState(RELEASE));
        }
        else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
        {
          if (mStateBuffer.mButtonStates[INPUT_MS_FBUTTON].state != KeyState(UP))
            MSUPDATEKEYSTATEBUFFER(INPUT_MS_FBUTTON, KeyState(RELEASE));
        }
        break;

      case WM_XBUTTONDOWN:
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
        {
          if (mStateBuffer.mButtonStates[INPUT_MS_BBUTTON].state == KeyState(TRIGGER))
            MSUPDATEKEYSTATEBUFFER(INPUT_MS_BBUTTON, KeyState(DOWN));
          else
            MSUPDATEKEYSTATEBUFFER(INPUT_MS_BBUTTON, KeyState(TRIGGER));
        }
        else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
        {
          if (mStateBuffer.mButtonStates[INPUT_MS_FBUTTON].state == KeyState(TRIGGER))
            MSUPDATEKEYSTATEBUFFER(INPUT_MS_FBUTTON, KeyState(DOWN));
          else
            MSUPDATEKEYSTATEBUFFER(INPUT_MS_FBUTTON, KeyState(TRIGGER));
        }
        break;

      case WM_MOUSELEAVE:
        if (mStateBuffer.mButtonStates[INPUT_MS_LBUTTON].state != KeyState(UP))
        {
          mStateBuffer.mButtonStates[INPUT_MS_LBUTTON].state = KeyState(RELEASE);
          mStateBuffer.mButtonStates[INPUT_MS_LBUTTON].lastChange = gCore->GetFrameCount();
        }

        if (mStateBuffer.mButtonStates[INPUT_MS_RBUTTON].state != KeyState(UP))
        {
          mStateBuffer.mButtonStates[INPUT_MS_RBUTTON].state = KeyState(RELEASE);
          mStateBuffer.mButtonStates[INPUT_MS_RBUTTON].lastChange = gCore->GetFrameCount();
        }

        if (mStateBuffer.mButtonStates[INPUT_MS_MBUTTON].state != KeyState(UP))
        {
          mStateBuffer.mButtonStates[INPUT_MS_MBUTTON].state = KeyState(RELEASE);
          mStateBuffer.mButtonStates[INPUT_MS_MBUTTON].lastChange = gCore->GetFrameCount();
        }

        if (mStateBuffer.mButtonStates[INPUT_MS_BBUTTON].state != KeyState(UP))
        {
          mStateBuffer.mButtonStates[INPUT_MS_BBUTTON].state = KeyState(RELEASE);
          mStateBuffer.mButtonStates[INPUT_MS_BBUTTON].lastChange = gCore->GetFrameCount();
        }

        if (mStateBuffer.mButtonStates[INPUT_MS_FBUTTON].state != KeyState(UP))
        {
          mStateBuffer.mButtonStates[INPUT_MS_FBUTTON].state = KeyState(RELEASE);
          mStateBuffer.mButtonStates[INPUT_MS_FBUTTON].lastChange = gCore->GetFrameCount();
        }

        if (mStateBuffer.mButtonStates[INPUT_MS_MOVE_X].state != KeyState(UP))
        {
          mStateBuffer.mButtonStates[INPUT_MS_MOVE_X].state = KeyState(RELEASE);
          mStateBuffer.mButtonStates[INPUT_MS_MOVE_X].lastChange = gCore->GetFrameCount();
        }

        if (mStateBuffer.mButtonStates[INPUT_MS_MOVE_Y].state != KeyState(UP))
        {
          mStateBuffer.mButtonStates[INPUT_MS_MOVE_Y].state = KeyState(RELEASE);
          mStateBuffer.mButtonStates[INPUT_MS_MOVE_Y].lastChange = gCore->GetFrameCount();
        }

        if (mStateBuffer.mButtonStates[INPUT_MS_MOVE_XY].state != KeyState(UP))
        {
          mStateBuffer.mButtonStates[INPUT_MS_MOVE_XY].state = KeyState(RELEASE);
          mStateBuffer.mButtonStates[INPUT_MS_MOVE_XY].lastChange = gCore->GetFrameCount();
        }

        if (mStateBuffer.mButtonStates[INPUT_MS_WHEEL].state != KeyState(UP))
        {
          mStateBuffer.mButtonStates[INPUT_MS_WHEEL].state = KeyState(RELEASE);
          mStateBuffer.mButtonStates[INPUT_MS_WHEEL].lastChange = gCore->GetFrameCount();
        }

        mStateBuffer.mMouseOverWindow = false;
        break;


      // Ansi Keys.


      case WM_CHAR:
        {
          RawStore args;
          args.AddU8("key", wParam);
          gCore->Events.Post(H("typewriter"), args, MSG_PRIORITY_MEDIUM);
        }
        break;


      // Virtual Keys.

      case WM_SYSKEYUP:
      case WM_SYSKEYDOWN:
      case WM_KEYUP:
      case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_MENU:       KBUPDATEKEYSTATEBUFFER(INPUT_KB_ALT);               break;
        case VK_BACK:       KBUPDATEKEYSTATEBUFFER(INPUT_KB_BACKSPACE);         break;
        case VK_TAB:        KBUPDATEKEYSTATEBUFFER(INPUT_KB_TAB);               break;
        case VK_RETURN:     KBUPDATEKEYSTATEBUFFER(INPUT_KB_ENTER);             break;
        case VK_SHIFT:      KBUPDATEKEYSTATEBUFFER(INPUT_KB_SHIFT);             break;
        case VK_CONTROL:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_CONTROL);           break;
        case VK_PAUSE:      KBUPDATEKEYSTATEBUFFER(INPUT_KB_PAUSE);             break;
        case VK_CAPITAL:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_CAPS);              break;
        case VK_ESCAPE:     KBUPDATEKEYSTATEBUFFER(INPUT_KB_ESC);               break;
        case VK_SPACE:      KBUPDATEKEYSTATEBUFFER(INPUT_KB_SPACE);             break;
        case VK_PRIOR:      KBUPDATEKEYSTATEBUFFER(INPUT_KB_PAGE_UP);           break;
        case VK_NEXT:       KBUPDATEKEYSTATEBUFFER(INPUT_KB_PAGE_DOWN);         break;
        case VK_END:        KBUPDATEKEYSTATEBUFFER(INPUT_KB_END);               break;
        case VK_HOME:       KBUPDATEKEYSTATEBUFFER(INPUT_KB_HOME);              break;
        case VK_LEFT:       KBUPDATEKEYSTATEBUFFER(INPUT_KB_LEFT);              break;
        case VK_RIGHT:      KBUPDATEKEYSTATEBUFFER(INPUT_KB_RIGHT);             break;
        case VK_DOWN:       KBUPDATEKEYSTATEBUFFER(INPUT_KB_DOWN);              break;
        case VK_UP:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_UP);                break;
        case VK_PRINT:
        case VK_SNAPSHOT:   KBUPDATEKEYSTATEBUFFER(INPUT_KB_PRINT_SCREEN);      break;
        case VK_INSERT:     KBUPDATEKEYSTATEBUFFER(INPUT_KB_INSERT);            break;
        case VK_DELETE:     KBUPDATEKEYSTATEBUFFER(INPUT_KB_DELETE);            break;
        case VK_NUMPAD0:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_0);        break;
        case VK_NUMPAD1:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_1);        break;
        case VK_NUMPAD2:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_2);        break;
        case VK_NUMPAD3:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_3);        break;
        case VK_NUMPAD4:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_4);        break;
        case VK_NUMPAD5:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_5);        break;
        case VK_NUMPAD6:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_6);        break;
        case VK_NUMPAD7:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_7);        break;
        case VK_NUMPAD8:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_8);        break;
        case VK_NUMPAD9:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_9);        break;
        case VK_MULTIPLY:   KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_ASTERIX);  break;
        case VK_ADD:        KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_PLUS);     break;
        case VK_SUBTRACT:   KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_MINUS);    break;
        case VK_DECIMAL:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_DOT);      break;
        case VK_DIVIDE:     KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_DIV);      break;
        case VK_F1:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_F1);                break;
        case VK_F2:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_F2);                break;
        case VK_F3:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_F3);                break;
        case VK_F4:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_F4);                break;
        case VK_F5:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_F5);                break;
        case VK_F6:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_F6);                break;
        case VK_F7:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_F7);                break;
        case VK_F8:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_F8);                break;
        case VK_F9:         KBUPDATEKEYSTATEBUFFER(INPUT_KB_F9);                break;
        case VK_F10:        KBUPDATEKEYSTATEBUFFER(INPUT_KB_F10);               break;
        case VK_F11:        KBUPDATEKEYSTATEBUFFER(INPUT_KB_F11);               break;
        case VK_F12:        KBUPDATEKEYSTATEBUFFER(INPUT_KB_F12);               break;
        case VK_NUMLOCK:    KBUPDATEKEYSTATEBUFFER(INPUT_KB_NUMBLOCK_LOCK);     break;
        case VK_SCROLL:     KBUPDATEKEYSTATEBUFFER(INPUT_KB_SCROLL_LOCK);       break;
        case VK_LSHIFT:     KBUPDATEKEYSTATEBUFFER(INPUT_KB_SHIFT);             break;
        case VK_RSHIFT:     KBUPDATEKEYSTATEBUFFER(INPUT_KB_SHIFT_R);           break;
        case 'Q':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_Q);                 break;
        case 'W':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_W);                 break;
        case 'E':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_E);                 break;
        case 'R':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_R);                 break;
        case 'T':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_T);                 break;
        case 'Y':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_Y);                 break;
        case 'U':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_U);                 break;
        case 'I':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_I);                 break;
        case 'O':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_O);                 break;
        case 'P':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_P);                 break;
        case 'A':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_A);                 break;
        case 'S':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_S);                 break;
        case 'D':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_D);                 break;
        case 'F':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_F);                 break;
        case 'G':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_G);                 break;
        case 'H':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_H);                 break;
        case 'J':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_J);                 break;
        case 'K':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_K);                 break;
        case 'L':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_L);                 break;
        case 'Z':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_Z);                 break;
        case 'X':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_X);                 break;
        case 'C':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_C);                 break;
        case 'V':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_V);                 break;
        case 'B':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_B);                 break;
        case 'N':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_N);                 break;
        case 'M':           KBUPDATEKEYSTATEBUFFER(INPUT_KB_M);                 break;
        case VK_OEM_3:      KBUPDATEKEYSTATEBUFFER(INPUT_KB_TILDE);             break;
        }
        break;
      }
    }


    //////////////////////////////////////////////////////////////////////////
    // Controller Component Related

//     void Input::updateControllerComponents( void )
//     {
//       cCompIt begin = controllerComponentRegistry.begin();
//       cCompIt end = controllerComponentRegistry.end();
// 
//       for(; begin != end; ++begin)
//         begin->second->Update();
// 
//       for(u32 i = 0; i < controllerHandles.size(); ++i)
//       {
//         Controller* controller = mControllerComponents.Dereference(controllerHandles[i]);
//         if(controller)
//           controller->Update();
//       }
//     }
// 
//     Handle Input::createControllerComponent( RawStore& data )
//     {
//       // We can simply pass a void pointer to the scope of serialization
//       void* val = data.GetVoid( H("scope") );
//       if(!val) 
//         return Handle();
// 
//       Scope& contScope = *((Scope*)val);
// 
//       //We're creating a new controller for something.
//       Handle newHandle;
//       Controller* controller = mControllerComponents.Acquire(newHandle);
// 
//       controller->serialize(contScope, true);
// 
//       controllerHandles.push_back(newHandle);
//       return newHandle;
//     }
// 
//     u1 Input::isActionFired( hstring const& action )
//     {
//       ActionBindings::const_iterator iKey = mKeyActionBindings.begin();
//       ActionBindings::const_iterator iKeyEnd = mKeyActionBindings.end();
// 
//       for (; iKey != iKeyEnd; ++iKey)
//       {
//         hstring key = iKey->first;
//         std::vector<BoundAction>::const_iterator iAction = iKey->second.begin();
//         std::vector<BoundAction>::const_iterator iActionEnd = iKey->second.end();
// 
//         for (; iAction != iActionEnd; ++iAction)
//         {
//           if (*iAction == action)
//           {
//             if (mStateBuffer.mButtonStates[key].state != KeyState(UP))
//               return true;
//           }
//         }
//       }
// 
//       return false;
//     }

    void Input::vRegisterComponent( TypeID id, Component* component )
    {
//       static TypeID cController = gCore->getTypeID<Controller>();

//       if(id == cController)
//       {
//         Controller* controllerComp = SCAST(Controller*, component);
//         ErrorIf(controllerComponentRegistry.find(controllerComp->getInstanceID()) != controllerComponentRegistry.end(), "Duplicate component registration");
//         controllerComponentRegistry[controllerComp->getInstanceID()] = controllerComp;
//       }
//       else
        printDebug("Trying to create something that isn't for input...");
    }

    void Input::vUnregisterComponent( TypeID id, Component* component )
    {
//       static TypeID cController = gCore->getTypeID<Controller>();
// 
//       if(id == cController)
//         controllerComponentRegistry.erase(component->getInstanceID());
//       else
        printDebug("Trying to kill a component that doesn't exist in input...");
      
    }


} // namespace Virtus