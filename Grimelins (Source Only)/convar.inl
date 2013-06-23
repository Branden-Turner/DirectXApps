/*! ====================================================================================================================
  @file convar.inl

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Oct.02.2011

  @brief
    Inline files for templated convar functions.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_CONVAR_INL
#define VIRTUS_CONVAR_INL

namespace Grimelins {

    template< typename T >
    ConVar::ConVar(std::string const& name, std::string const& description, T const& default_value )
      : mName(name), 
        mhName(name),
        mDescription(description),
        mClamp(false)
    {
      u32 hname = CONSTHASH(name.c_str());

      ConVarRegistryData cvardat =  ConVarRegistryData(hname, mhName, this);

      Core* core = CoreInstance();

      cvardat.mRegistered = ( core ) ? true : false;

      ErrorIf(GetConvarRegistry().find( cvardat ) != GetConvarRegistry().end(), "Attempt to create same ConVar multiple times. Note the ConVars are static singletons!");

      *this = default_value;

      GetConvarRegistry().insert( cvardat );

      if( cvardat.mRegistered )
      {
        core->Events.Subscribe(mhName, this);
      }
    }

    template< typename T >
    ConVar::ConVar(std::string const& name, std::string const& description, T const& default_value, T const& min, T const& max )
      : mName(name), 
        mhName(name),
        mDescription(description),
        mClamp(true)
    {
      u32 hname = CONSTHASH(name.c_str());

      ConVarRegistryData cvardat =  ConVarRegistryData(hname, mhName, this);

      Core* core = CoreInstance();

      cvardat.mRegistered = ( core ) ? true : false;

      ErrorIf(GetConvarRegistry().find( cvardat ) != GetConvarRegistry().end(), "Attempt to create same ConVar multiple times. Note the ConVars are static singletons!");

      mMin << min;
      mMax << max;

      *this = default_value;

      GetConvarRegistry().insert( cvardat ); 

      if( cvardat.mRegistered )
      {
        core->Events.Subscribe(mhName, this);
      }
    }

      // Dunno what type we're converting to until it's requested. Woo templates
      // T is required to have operator>> operator<< operator> operator<
    template< typename T >
    ConVar::operator T ( void ) const
    {
      T tmp, mint, maxt;

      std::stringstream tmpstr( mStream.str() );
      std::stringstream tmpmin( mMin.str() );
      std::stringstream tmpmax( mMax.str() );

      tmpstr >> tmp;
      tmpmin >> mint;
      tmpmax >> maxt;

      if(mClamp)
        if( tmp > maxt )
          return maxt;
        else if( tmp < mint ) 
          return mint;

      return tmp;
    }

    template< typename T >
    void ConVar::operator=( T const& var )
    {
      mStream.str("");
      mStream << var;
    }

} // namespace Virtus
#endif // Include Guard