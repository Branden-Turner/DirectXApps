/*! ====================================================================================================================
  @file convar.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 27.Sep.2011

  @brief 
    Implements user changeable console variable type.

    !!!IMPOTRANT: CONVARS HAVE TO BE STATIC GLOBALS

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "convar.h"

#undef ConVar

namespace Grimelins
{
    // Static convar registry
    ConVarRegistry& GetConvarRegistry(void)
    {
      static ConVarRegistry Registry;
      return Registry;
    }

    bool operator< ( ConVarRegistryData const& lhs, ConVarRegistryData const& rhs ) 
    {
      return lhs.mHash < rhs.mHash; 
    }

    bool operator> ( ConVarRegistryData const& lhs, ConVarRegistryData const& rhs )
    { 
      return lhs.mHash > rhs.mHash; 
    }

    bool ConVar::operator !( void ) const
    {
      u32 tmp;
      std::stringstream tmps(mStream.str());
      tmps >> tmp;
      return tmp == 0;
    }

    ConVar::operator ch const*( void ) const
    {
      return mStream.str().c_str();
    }

    ConVar::operator bool( void ) const
    {
      u32 tmp;
      std::stringstream tmps(mStream.str());
      tmps >> tmp;
      return tmp > 0;
    }

    u1 ConVar::handleEvent(hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp)
    {
      return false; // We don't like messages (at all) ((they can't tell us what to do, not our boss))
    }

    u1 ConVar::parseCommand(hstring const& command, std::vector<std::string> const& commandLine)
    {
      if (command != mhName)
        return false;

      if (commandLine.size() == 1) // Has no data, print the data we have to console
      {
        RawStore arg;
        std::string msg = "USAGE: ";
        msg += mDescription;
        msg += "\tVALUE: ";
        msg += mStream.str();
        if(mClamp)
        {
          msg += "\tMIN: ";
          msg += mMin.str();
          msg += "\tMAX: ";
          msg += mMax.str();
        }
        arg.AddString("message", msg);
        gCore->Events.Post(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
      }
      else if (commandLine.size() == 2) // Has data, assign that data
      {
        this->operator=(commandLine[1]);
      }
      else // Has too much data, send error
      {
        RawStore arg;
        std::string msg = "ERROR: Too many arguments given.";
        arg.AddString("message", msg);
        gCore->Events.Post(H("con_print"), arg, MSG_PRIORITY_MEDIUM);
      }

      return true;
    }
} // namespace Virtus