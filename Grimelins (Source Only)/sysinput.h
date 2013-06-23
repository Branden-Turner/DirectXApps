/*! ====================================================================================================================
  @file appinput.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 08.Jul.2011

  @brief 
    Input management.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once

namespace Grimelins
{

    struct StateBuffer;
    //////////////////////////////////////////////////////////////////////////
    // Input system

    SCRIPT_HeaderDefine(Input);
    class Input : public ISystem, public IListener
    {
    public:
      enum KeyState
      {
        UP,
        DOWN,
        TRIGGER,
        RELEASE
      };

    public: // Autos
      Input(void);
      ~Input(void);

    public: // Virtual
      virtual void vLoadSystem(void);
      virtual void vUnloadSystem(void);
      virtual void vUpdateSystem(void);
      virtual hstring vGetSystemID(void) const;
      virtual u1 handleEvent(hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp);
      virtual u1 parseCommand(hstring const& command, std::vector<std::string> const& arguments);
      virtual u1 handleRequest(hstring const& request, RawStore& data);
      virtual void vRegisterComponent(TypeID id, Component* component);
      virtual void vUnregisterComponent(TypeID id, Component* component);

    private: // Methods
      void bindAction(hstring const& key, hstring const& action, KeyState state);
      void bindAction(std::vector<hstring> const& combination, hstring const& action, KeyState state);
      void unbindAction(hstring const& key, hstring const& action, KeyState state);
      void unbindAction(std::vector<hstring> const& combination, hstring const& action, KeyState state);
      void unbindActionFromAll(hstring const& action);
      void unbindAllActionsFromKey(hstring const& key);
      void unbindAllActionsFromKey(std::vector<hstring> combination);
      void notifyFocusSubscriber(hstring const& key);
      void fireKeyActionBindings(hstring const& key);
      

    private: // Controller
      void updateMouse(void);
      void updateVibration(void);
      void updateControllers(void);
      vinline void translateAndUpdateControllerButton(u16 button, u32 XName, hstring const& VName);

      //vibration mixer interface
      class Vibration {
      public:
        f32 timestart,timeend,inTime,outTime,fadein,fadeout;
        vec3 source;
        vec3 receiver;
        f32 magnitude;
        bool operator<(Vibration const& rhs) const {return timestart<rhs.timestart;};
      };

    public:
      //additive vibration mixing
      void vibrateAdd(f32 mag, f32 dur, f32 delay, f32 fadeIn, f32 fadeOut, vec3 const& source = vec3());
      //clear all mixed vibrations
      void vibrateReset(void);
      //override mixed vibrations, clearing the list
      void vibrateSet(f32 mag, f32 dur, f32 delay, f32 fadeIn, f32 fadeOut, vec3 const& source);
      void vibratePause(void);
      void vibrateUnpause(void);

    private: // Keyboard
      static void HandleInputMessage(UINT MSG, WPARAM wParam, LPARAM lParam);

    // Controller components
//     friend class Controller;
//     private: 
//       Handle createControllerComponent(RawStore& data);
//       void updateControllerComponents(void);
//       u1   isActionFired(hstring const& action);
//       std::vector<Handle> controllerHandles;
//       static ResourceManager<Controller>    mControllerComponents;
// 
//       typedef std::map<ComponentID, Controller*>::iterator cCompIt;
//       std::map<ComponentID, Controller*> controllerComponentRegistry;
      

    private: // Decls

      friend class Window;

      struct BoundAction
      {
        std::vector<hstring> additonalKeys;
        hstring action;
        KeyState state;

        u1 operator==(BoundAction const& rhs) const
        { 
          return action == rhs.action && state == rhs.state && additonalKeys == rhs.additonalKeys; 
        }

        u1 operator==(hstring const& rhs) const
        { 
          return action == rhs;
        }
      };

      static u1 IsNotCombination(Input::BoundAction const& action) { return !IsCombination(action); }
      static u1 IsCombination(Input::BoundAction const& action) { return !action.additonalKeys.empty(); }

      typedef std::map<hstring, std::vector<BoundAction>> ActionBindings;
      typedef std::multiset<Vibration> Vibrations;
    private: // Data
      static StateBuffer         mStateBuffer;
      static ActionBindings      mKeyActionBindings;
      u32                        mLastControllerPackage;
      Handle                     mFocusSubscriber;
      f32                        mVibrationTime;
      u1                         mNewVibration;
      Vibrations                 mVibrations;
      bool                       mVibrateLock;
      f32                        mVibratePauseTime;
    };

    //////////////////////////////////////////////////////////////////////////
    // StateBuffer structure

    struct StateBuffer
    {
      struct ButtonState
      {
        Input::KeyState state;
        u64 lastChange;

        ButtonState(void) : state(Input::UP), 
                            lastChange(0) 
        {

        }
      };

      typedef std::map<hstring, ButtonState> ButtonStates;

      ButtonStates mButtonStates;
      s16 mMouseWheelDelta;
      f32 mCtrlLeftTrigger;
      f32 mCtrlLeftTriggerDelta;
      f32 mCtrlRightTrigger;
      f32 mCtrlRightTriggerDelta;
      POINT mMousePosition;
      POINT mMouseDelta;
      vec2 mCtrlLeftStick;
      vec2 mCtrlLeftStickDelta;
      vec2 mCtrlRightStick;
      vec2 mCtrlRightStickDelta;
      u1 mMouseOverWindow;
      u1 mHasFocus;

      StateBuffer(void) : mMouseWheelDelta(0),
        mCtrlLeftTrigger(0.0f),
        mCtrlLeftTriggerDelta(0.0f),
        mCtrlRightTrigger(0.0f),
        mCtrlRightTriggerDelta(0.0f),
        mMouseOverWindow(false),
        mHasFocus(true)
      {
        mMousePosition.x = 0;
        mMousePosition.y = 0;
        mMouseDelta.x = 0;
        mMouseDelta.y = 0;
      }

      void Reset(void)
      {
        mButtonStates.clear();
        mMouseWheelDelta = 0;
        mCtrlLeftTrigger = 0.0f;
        mCtrlLeftTriggerDelta = 0.0f;
        mCtrlRightTrigger = 0.0f;
        mCtrlRightTriggerDelta = 0.0f;
        mMouseOverWindow = false;
        mMousePosition.x = 0;
        mMousePosition.y = 0;
        mMouseDelta.x = 0;
        mMouseDelta.y = 0;
        mHasFocus = true;
      }
    };
} // namespace Virtus