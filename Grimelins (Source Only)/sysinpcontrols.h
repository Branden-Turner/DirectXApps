/*! ====================================================================================================================
  @file appinpcontrols.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 08.Jul.2011

  @brief 
    Valid input controls.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once

namespace Grimelins
{
    ////////////////////// Windows Keyboard //////////////////////

    // Common Virtual Keys
    hstring INPUT_KB_ESC                      = H("INPUT_KB_ESC");
    hstring INPUT_KB_TILDE                    = H("INPUT_KB_TILDE");
    hstring INPUT_KB_TAB                      = H("INPUT_KB_TAB");
    hstring INPUT_KB_CAPS                     = H("INPUT_KB_CAPS");
    hstring INPUT_KB_SHIFT                    = H("INPUT_KB_SHIFT");
    hstring INPUT_KB_CONTROL                  = H("INPUT_KB_CONTROL");
    hstring INPUT_KB_ALT                      = H("INPUT_KB_ALT");
    hstring INPUT_KB_SPACE                    = H("INPUT_KB_SPACE");
    hstring INPUT_KB_ALT_R                    = H("INPUT_KB_ALT_R");
    hstring INPUT_KB_UP                       = H("INPUT_KB_UP");
    hstring INPUT_KB_DOWN                     = H("INPUT_KB_DOWN");
    hstring INPUT_KB_LEFT                     = H("INPUT_KB_LEFT");
    hstring INPUT_KB_RIGHT                    = H("INPUT_KB_RIGHT");
    hstring INPUT_KB_SHIFT_R                  = H("INPUT_KB_SHIFT_R");
    hstring INPUT_KB_ENTER                    = H("INPUT_KB_ENTER");
    hstring INPUT_KB_BACKSLASH                = H("INPUT_KB_BACKSLASH");
    hstring INPUT_KB_BACKSPACE                = H("INPUT_KB_BACKSPACE");

    // Character Keys
    hstring INPUT_KB_Q                        = H("INPUT_KB_Q");
    hstring INPUT_KB_W                        = H("INPUT_KB_W");
    hstring INPUT_KB_E                        = H("INPUT_KB_E");
    hstring INPUT_KB_R                        = H("INPUT_KB_R");
    hstring INPUT_KB_T                        = H("INPUT_KB_T");
    hstring INPUT_KB_Y                        = H("INPUT_KB_Y");
    hstring INPUT_KB_U                        = H("INPUT_KB_U");
    hstring INPUT_KB_I                        = H("INPUT_KB_I");
    hstring INPUT_KB_O                        = H("INPUT_KB_O");
    hstring INPUT_KB_P                        = H("INPUT_KB_P");
    hstring INPUT_KB_A                        = H("INPUT_KB_A");
    hstring INPUT_KB_S                        = H("INPUT_KB_S");
    hstring INPUT_KB_D                        = H("INPUT_KB_D");
    hstring INPUT_KB_F                        = H("INPUT_KB_F");
    hstring INPUT_KB_G                        = H("INPUT_KB_G");
    hstring INPUT_KB_H                        = H("INPUT_KB_H");
    hstring INPUT_KB_J                        = H("INPUT_KB_J");
    hstring INPUT_KB_K                        = H("INPUT_KB_K");
    hstring INPUT_KB_L                        = H("INPUT_KB_L");
    hstring INPUT_KB_Z                        = H("INPUT_KB_Z");
    hstring INPUT_KB_X                        = H("INPUT_KB_X");
    hstring INPUT_KB_C                        = H("INPUT_KB_C");
    hstring INPUT_KB_V                        = H("INPUT_KB_V");
    hstring INPUT_KB_B                        = H("INPUT_KB_B");
    hstring INPUT_KB_N                        = H("INPUT_KB_N");
    hstring INPUT_KB_M                        = H("INPUT_KB_M");

    // Function Keys
    hstring INPUT_KB_F1                       = H("INPUT_KB_F1");
    hstring INPUT_KB_F2                       = H("INPUT_KB_F2");
    hstring INPUT_KB_F3                       = H("INPUT_KB_F3");
    hstring INPUT_KB_F4                       = H("INPUT_KB_F4");
    hstring INPUT_KB_F5                       = H("INPUT_KB_F5");
    hstring INPUT_KB_F6                       = H("INPUT_KB_F6");
    hstring INPUT_KB_F7                       = H("INPUT_KB_F7");
    hstring INPUT_KB_F8                       = H("INPUT_KB_F8");
    hstring INPUT_KB_F9                       = H("INPUT_KB_F9");
    hstring INPUT_KB_F10                      = H("INPUT_KB_F10");
    hstring INPUT_KB_F11                      = H("INPUT_KB_F11");
    hstring INPUT_KB_F12                      = H("INPUT_KB_F12");

    // Number Row
    hstring INPUT_KB_1                        = H("INPUT_KB_1");
    hstring INPUT_KB_2                        = H("INPUT_KB_2");
    hstring INPUT_KB_3                        = H("INPUT_KB_3");
    hstring INPUT_KB_4                        = H("INPUT_KB_4");
    hstring INPUT_KB_5                        = H("INPUT_KB_5");
    hstring INPUT_KB_6                        = H("INPUT_KB_6");
    hstring INPUT_KB_7                        = H("INPUT_KB_7");
    hstring INPUT_KB_8                        = H("INPUT_KB_8");
    hstring INPUT_KB_9                        = H("INPUT_KB_9");
    hstring INPUT_KB_0                        = H("INPUT_KB_0");

    // Number Block
    hstring INPUT_KB_NUMBLOCK_LOCK            = H("INPUT_KB_NUMBLOCK_LOCK");
    hstring INPUT_KB_NUMBLOCK_DIV             = H("INPUT_KB_NUMBLOCK_DIV");
    hstring INPUT_KB_NUMBLOCK_ASTERIX         = H("INPUT_KB_NUMBLOCK_ASTERIX");
    hstring INPUT_KB_NUMBLOCK_MINUS           = H("INPUT_KB_NUMBLOCK_MINUS");
    hstring INPUT_KB_NUMBLOCK_PLUS            = H("INPUT_KB_NUMBLOCK_PLUS");
    hstring INPUT_KB_NUMBLOCK_ENTER           = H("INPUT_KB_NUMBLOCK_ENTER");
    hstring INPUT_KB_NUMBLOCK_DOT             = H("INPUT_KB_NUMBLOCK_DOT");
    hstring INPUT_KB_NUMBLOCK_1               = H("INPUT_KB_NUMBLOCK_1");
    hstring INPUT_KB_NUMBLOCK_2               = H("INPUT_KB_NUMBLOCK_2");
    hstring INPUT_KB_NUMBLOCK_3               = H("INPUT_KB_NUMBLOCK_3");
    hstring INPUT_KB_NUMBLOCK_4               = H("INPUT_KB_NUMBLOCK_4");
    hstring INPUT_KB_NUMBLOCK_5               = H("INPUT_KB_NUMBLOCK_5");
    hstring INPUT_KB_NUMBLOCK_6               = H("INPUT_KB_NUMBLOCK_6");
    hstring INPUT_KB_NUMBLOCK_7               = H("INPUT_KB_NUMBLOCK_7");
    hstring INPUT_KB_NUMBLOCK_8               = H("INPUT_KB_NUMBLOCK_8");
    hstring INPUT_KB_NUMBLOCK_9               = H("INPUT_KB_NUMBLOCK_9");
    hstring INPUT_KB_NUMBLOCK_0               = H("INPUT_KB_NUMBLOCK_0");

    // Special Blocks
    hstring INPUT_KB_PRINT_SCREEN             = H("INPUT_KB_PRINT_SCREEN");
    hstring INPUT_KB_SCROLL_LOCK              = H("INPUT_KB_SCROLL_LOCK");
    hstring INPUT_KB_PAUSE                    = H("INPUT_KB_PAUSE");

    hstring INPUT_KB_INSERT                   = H("INPUT_KB_INSERT");
    hstring INPUT_KB_DELETE                   = H("INPUT_KB_DELETE");
    hstring INPUT_KB_HOME                     = H("INPUT_KB_HOME");
    hstring INPUT_KB_END                      = H("INPUT_KB_END");
    hstring INPUT_KB_PAGE_UP                  = H("INPUT_KB_PAGE_UP");
    hstring INPUT_KB_PAGE_DOWN                = H("INPUT_KB_PAGE_DOWN");


    ////////////////////// WINDOWS MOUSE //////////////////////
    hstring INPUT_MS_LBUTTON                  = H("INPUT_MS_LBUTTON");
    hstring INPUT_MS_RBUTTON                  = H("INPUT_MS_RBUTTON");
    hstring INPUT_MS_MBUTTON                  = H("INPUT_MS_MBUTTON");
    hstring INPUT_MS_FBUTTON                  = H("INPUT_MS_FBUTTON");
    hstring INPUT_MS_BBUTTON                  = H("INPUT_MS_BBUTTON");
    hstring INPUT_MS_MOVE_X                   = H("INPUT_MS_MOVE_X");
    hstring INPUT_MS_MOVE_Y                   = H("INPUT_MS_MOVE_Y");
    hstring INPUT_MS_MOVE_XY                  = H("INPUT_MS_MOVE_XY");
    hstring INPUT_MS_WHEEL                    = H("INPUT_MS_WHEEL");


    ////////////////////// XBOX Gamepad //////////////////////
    hstring INPUT_CTRL_DPAD_UP                = H("INPUT_CTRL_DPAD_UP");
    hstring INPUT_CTRL_DPAD_DOWN              = H("INPUT_CTRL_DPAD_DOWN");
    hstring INPUT_CTRL_DPAD_LEFT              = H("INPUT_CTRL_DPAD_LEFT");
    hstring INPUT_CTRL_DPAD_RIGHT             = H("INPUT_CTRL_DPAD_RIGHT");
    hstring INPUT_CTRL_START                  = H("INPUT_CTRL_START");
    hstring INPUT_CTRL_BACK                   = H("INPUT_CTRL_BACK");
    hstring INPUT_CTRL_THUMB_LEFT             = H("INPUT_CTRL_THUMB_LEFT");
    hstring INPUT_CTRL_THUMB_RIGHT            = H("INPUT_CTRL_THUMB_RIGHT");
    hstring INPUT_CTRL_SHOULDER_LEFT          = H("INPUT_CTRL_SHOULDER_LEFT");
    hstring INPUT_CTRL_SHOULDER_RIGHT         = H("INPUT_CTRL_SHOULDER_RIGHT");
    hstring INPUT_CTRL_A                      = H("INPUT_CTRL_A");
    hstring INPUT_CTRL_B                      = H("INPUT_CTRL_B");
    hstring INPUT_CTRL_X                      = H("INPUT_CTRL_X");
    hstring INPUT_CTRL_Y                      = H("INPUT_CTRL_Y");
    hstring INPUT_CTRL_TRIGGER_LEFT           = H("INPUT_CTRL_TRIGGER_LEFT");
    hstring INPUT_CTRL_TRIGGER_RIGHT          = H("INPUT_CTRL_TRIGGER_RIGHT");
    hstring INPUT_CTRL_STICK_LEFT             = H("INPUT_CTRL_STICK_LEFT");
    hstring INPUT_CTRL_STICK_RIGHT            = H("INPUT_CTRL_STICK_RIGHT");
    hstring INPUT_STICK_LEFT_FORWARD          = H("INPUT_STICK_LEFT_FORWARD");
    hstring INPUT_STICK_LEFT_BACKWARD         = H("INPUT_STICK_LEFT_BACKWARD");
    hstring INPUT_STICK_LEFT_LEFT             = H("INPUT_STICK_LEFT_LEFT");
    hstring INPUT_STICK_LEFT_RIGHT            = H("INPUT_STICK_LEFT_RIGHT");

    
} // namespace Virtus