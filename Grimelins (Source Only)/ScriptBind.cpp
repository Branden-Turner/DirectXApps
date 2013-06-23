/*! ====================================================================================================================
  @file ScriptBind.cpp

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Nov.1.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "ScriptBind.h"
#include "object.h"
#include "comptransform.h"
#include "cmpgfx.h"
#include "compbody.h"
#include "cmpData.h"
#include "userinterface.h"
#include "uigroup.h"
#include "scriptGlobal.h"
#include "camera.h"
#include "script_updates.h"
#include "script_robot.h"

namespace
{
  void coreSendMessage(std::string const& message)
  {
    Grimelins::gCore->Events.Post(H(message.c_str()));
  }

  void coreSendDelayedMessage(std::string const& message, float seconds, bool allowLateSub)
  {
    Grimelins::gCore->Events.PostDelayed(H(message.c_str()), seconds, allowLateSub);
  }

  void coreSendMessageTo(std::string const& message, Grimelins::Object* obj)
  {
    Grimelins::gCore->Events.PostTo(obj->GetMyListenerHandle(), H(message.c_str()));
  }

  void coreSendDelayedMessageTo(std::string const& message, Grimelins::Object* obj, float seconds, bool allowLateSub)
  {
    Grimelins::gCore->Events.PostToDelayed(obj->GetMyListenerHandle(), H(message.c_str()), seconds, allowLateSub);
  }


  void coreSendMessageWith(std::string const& message)
  {
    Grimelins::gCore->Events.Post(H(message.c_str()), Grimelins::RawStore());
  }

  void coreSendDelayedMessageWith(std::string const& message, float seconds, bool allowLateSub)
  {
    Grimelins::gCore->Events.PostDelayed(H(message.c_str()), Grimelins::RawStore(), seconds, allowLateSub);
  }

  void coreSendMessageToWith(std::string const& message, Grimelins::Object* obj)
  {
    Grimelins::gCore->Events.PostTo(obj->GetMyListenerHandle(), H(message.c_str()), Grimelins::RawStore());
  }

  void coreSendDelayedMessageToWith(std::string const& message, Grimelins::Object* obj, float seconds, bool allowLateSub)
  {
    Grimelins::gCore->Events.PostToDelayed(obj->GetMyListenerHandle(), H(message.c_str()), Grimelins::RawStore(), seconds, allowLateSub);
  }
}
namespace Grimelins
{


  void RunCoreBinding(void)
  {
    Sqrat::Table coreTable(gCore->Scripter);
    coreTable.Func("send", &coreSendMessage)
             .Func("sendDelayed", &coreSendDelayedMessage)
             .Func("sendTo", &coreSendMessageTo)
             .Func("sendToDelayed", &coreSendDelayedMessageTo)
             .Func("sendWtih", &coreSendMessageWith)
             .Func("sendDelayedWith", &coreSendDelayedMessageWith)
             .Func("sendToWith", &coreSendMessageToWith)
             .Func("sendToDelayedWith", &coreSendDelayedMessageToWith);
    Sqrat::RootTable(gCore->Scripter).Bind("gCore", coreTable);

    bind_update_functions();
    bind_robot_functions();

    SCRIPT_RegisterType(Object, Grimelins);
    SCRIPT_RegisterType(Transform, Grimelins);
    SCRIPT_RegisterType(gfxShape, Grimelins);
    SCRIPT_RegisterType(gfxModel, Grimelins);
    SCRIPT_RegisterType(Body, Grimelins);
    SCRIPT_RegisterType(Data, Grimelins);
    SCRIPT_RegisterType(UIGroup, Grimelins);
    SCRIPT_RegisterType(Camera, Grimelins);

    SCRIPT_RegisterType(ScriptVec2, Grimelins);
    SCRIPT_RegisterType(ScriptVec3, Grimelins);
    SCRIPT_RegisterType(ScriptVec4, Grimelins);
    SCRIPT_RegisterType(ScriptMatrix4, Grimelins);
    SCRIPT_RegisterType(Quaternion, Grimelins);  

    SCRIPT_RegisterType(RawStore, Grimelins);

    SCRIPTGLOBALS = new ScriptGlobal();

//     SCRIPT_EnumStart("gMSG");
//     SCRIPT_EnumValueAs(MSG_DEFAULT,         "DEFAULT");
//     SCRIPT_EnumValueAs(MSG_TYPE_NETWORK,    "TYPE_NETWORK");
//     SCRIPT_EnumValueAs(MSG_PRIORITY_LOW,    "PRIORITY_LOW");
//     SCRIPT_EnumValueAs(MSG_PRIORITY_MEDIUM, "PRIORITY_MEDIUM");
//     SCRIPT_EnumValueAs(MSG_PRIORITY_HIGH,   "PRIORITY_HIGH");
//     SCRIPT_EnumValueAs(MSG_PRIORITY_TOP,    "PRIORITY_TOP");
//     SCRIPT_EnumValueAs(MSG_NOT_CONSUMABLE,  "NOT_CONSUMABLE");
//     SCRIPT_EnumEnd();

  }

};//Grimelins namespace
