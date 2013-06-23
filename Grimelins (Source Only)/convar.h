/*! ====================================================================================================================
  @file convar.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 27.Sep.2011

  @brief 
    Implements user changeable convar type.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_CONVAR_H
#define VIRTUS_CONVAR_H


namespace Grimelins
{
    struct ConVarRegistryData
    {
      ConVarRegistryData( u32 hash, hstring const& hname, IListener* listener ) : mHash(hash), mHName(hname), mListener(listener), mRegistered(false) {}

      u32 mHash;
      hstring mHName;
      IListener* mListener;
      mutable u1 mRegistered;
    };

    bool operator< ( ConVarRegistryData const& lhs, ConVarRegistryData const& rhs );
    bool operator> ( ConVarRegistryData const& lhs, ConVarRegistryData const& rhs );

    typedef std::set<ConVarRegistryData> ConVarRegistry;

    ConVarRegistry& GetConvarRegistry(void);

    class ConVar : public IListener
    {
      friend EventHandler; // Event Handler auto registers all instances of convar to commands

    public: // Auto

      template< typename T >
      ConVar(std::string const& name, std::string const& description, T const& default_value );

      template< typename T >
      ConVar(std::string const& name, std::string const& description, T const& default_value, T const& min, T const& max );

      template< typename T >
      void operator=( T const& var );

      template< typename T >
      operator T ( void ) const;



      bool operator !( void ) const;
      operator ch const*( void ) const;
      operator bool( void ) const;

    public: // Interface
      virtual bool handleEvent(hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp);
      virtual bool parseCommand(hstring const& command, std::vector<std::string> const& commandLine);

    private:
      std::stringstream mStream;
      std::stringstream mMin;
      std::stringstream mMax;

      u1 mClamp;

      std::string mName;
      hstring mhName;
      std::string mDescription;

    };

  #define ConVarType ConVar
  #define cvar static ConVarType
   
} // namespace Virtus

#include "convar.inl"

#endif // include guard