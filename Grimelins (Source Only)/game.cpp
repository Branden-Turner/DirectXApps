/*! ====================================================================================================================
  @file gamelogic.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.14.2011

  @brief 
    Management of gamelogic systems.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "game.h"
#include "factory.h"
#include "physics.h"
#include "userinterface.h"
namespace
{
  void consolePrint(const char *s, va_list vl)
  {
    static const int BufSz = 512;
    char msg[BufSz];
    vsnprintf_s(msg, BufSz, _TRUNCATE , s, vl);
    std::string raw = msg;
    std::vector<std::string> segments = Grimelins::ExplodeString("\n", raw);

    Grimelins::RawStore arg;

    for(size_t i=0; i<segments.size(); ++i)
    {
      arg.AddString("message", segments[i]);
      Grimelins::gCore->Events.PostNow(H("con_print"), arg, Grimelins::MSG_PRIORITY_MEDIUM);
    }
  }

}
namespace Grimelins
{

    GameLayer::GameLayer(void)
    {

    }

    GameLayer::~GameLayer(void)
    {

    }

    void GameLayer::vLoadLayer(void)
    {
      // Add systems here
      this->AddSystem(Physics());
      this->AddSystem(UserInterface());
      this->AddSystem(Factory());
      this->LoadMySystems();

      gCore->Events.Subscribe(H("core_startup"), this);
      gCore->Events.Subscribe(H("exit_game"), this);
    }

    void GameLayer::vUnloadLayer(void)
    {
      this->UnloadMySystems();

      this->RemoveAllMySystems();
    }

    void GameLayer::vUpdateLayer(void)
    {
      this->UpdateMySystems();
    }

    hstring GameLayer::vGetLayerID(void) const
    {
      return H("GameLogic");
    }

    u1 GameLayer::handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp)
    {
      OnMessage(core_startup)
        CfgFile userconfig("assets\\userconfig.cfg");
        userconfig.Dispatch();
      ConsumeMessage


      OnMessage(exit_game)
        gCore->Stop();
      ConsumeMessage

      return false;
    }

    u1 GameLayer::parseCommand(hstring const& command, std::vector<std::string> const& commandLine)
    {
      if (command == H("exit"))
      {
        gCore->Stop();
        return true;
      }

//       if (command == H("script"))
//       {
//         if (commandLine.size() == 1) // Has no data, print the data we have to console
//         {
//           RawStore arg;
//           string msg = "    USAGE 1: script [file] [[file2]...]";
//           string msga = "    NOTES:";
//           string msg1 = "           The .nut extention is implied and not required.";
//           string msg2 = "           Bin\\scripts\\ is implied if no path is supplied.";
//           string msg3 = "           If you supply a relative path, it must be relative to the Bin\\ directory.";
//           string msg4 = "";
//           string msg5 = "    EXAMPLE: script myScript c:\\script2.nut c:\\folders\\script3 ..\\Source\\Project\\script4.nut";
//           string msgb = "";
//           string msgc = "    USAGE 2: script -do [Inline squirrel code here]";
// 
//           arg.SetString(H("message"), msg);
//           gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//           arg.SetString(H("message"), msga);
//           gCore->Events.PostNow(H("console_print"), arg, MSG_PRIORITY_MEDIUM);
//           arg.SetString(H("message"), msg1);
//           gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//           arg.SetString(H("message"), msg2);
//           gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//           arg.SetString(H("message"), msg3);
//           gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//           arg.SetString(H("message"), msg4);
//           gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//           arg.SetString(H("message"), msg5);
//           gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//           arg.SetString(H("message"), msgb);
//           gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//           arg.SetString(H("message"), msgc);
//           gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//         }
//         else if(commandLine[1] == "-do")
//         {
//           if(commandLine.size() == 2)
//           {
//             RawStore arg;
//             string msg1 = "    USAGE: script -do [squirrel code inline here]";
//             string msg2 = "    EXAMPLE: script -do print(\"hello world\");";
//             arg.SetString(H("message"), msg1);
//             gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//             arg.SetString(H("message"), msg2);
//             gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//           }
//           else
//           {
//             std::string scriptstring;
//             for(size_t x = 2; x < commandLine.size(); ++x)
//             {
//               scriptstring += " " + commandLine[x];
//             }
//             if(Virtus::SQ->RunString(scriptstring))
//             {
//               RawStore arg;
//               string msg = "    Compiled, executed.";
//               arg.SetString(H("message"), msg);
//               gCore->Events.PostNow(H("console_print"), arg, MSG_PRIORITY_MEDIUM);
//             }
//             else
//             {
//               RawStore arg;
//               string msg = "    Compile failed.";
//               arg.SetString(H("message"), msg);
//               gCore->Events.PostNow(H("console_print"), arg, MSG_PRIORITY_MEDIUM);
//             }
//           }
//         }
//         else if(commandLine[1] == "-console")
//         {
//           if(commandLine.size() == 2)
//           {
//             RawStore arg;
//             string msg = "    USAGE: script -console [on/off]";
//             string msga = "    VALUE: ";
//             msga += (SQ->isDefault ? "OFF, script prints to output window." : "ON, script prints to console");
//             arg.SetString(H("message"), msg);
//             gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//             arg.SetString(H("message"), msga);
//             gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//           }
//           else if(commandLine[2] =="on")
//             SQ->setPrintFunc(consolePrint);
//           else
//             SQ->setPrintFunc();
//         }
//         else
//         {
//           for(size_t i = 1; i < commandLine.size(); ++i)
//           {
// 
//             if(Virtus::SQ->RunScript(commandLine[i]))
//             {
//               RawStore arg;
//               string msg = "    " + commandLine[i];
//               for(size_t x = commandLine[i].size(); x < 30; ++x)
//                 msg += ".";
//               msg += " Succeeded.";
//               arg.SetString(H("message"), msg);
//               gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//             }
//             else
//             {
//               RawStore arg;
//               string msg = "    " + commandLine[i];
//               for(size_t x = commandLine[i].size(); x < 30; ++x)
//                 msg += ".";
//               msg += " Not Found.";
// 
//               arg.SetString(H("message"), msg);
//               gCore->Events.PostNow(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
//             }
//           }
//         }
//         return true;
//       }

      return false;
    }
}
