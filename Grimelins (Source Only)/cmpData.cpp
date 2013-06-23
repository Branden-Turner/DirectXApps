/*! ====================================================================================================================
  @file cmpData.cpp

  Grimelins 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jan.21.2012

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "cmpData.h"

namespace Grimelins
{
  /////// SCRIPT BINDS
  //// Transform
  SCRIPT_ClassStart(Data);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(getTypeName);
  SCRIPT_Method(getTypeID);
  SCRIPT_Method(exists);
  //SCRIPT specialized functions for get
  SCRIPT_Method(gets32);
  SCRIPT_Method(getf32);
  SCRIPT_Method(getbool);
  SCRIPT_Method(gettext);
  SCRIPT_Method(getvec2);
  SCRIPT_Method(getvec3);
  SCRIPT_Method(getvec4);
  SCRIPT_Method(getquat);
  //SCRIPT specialized functions for set
  SCRIPT_Method(sets32);
  SCRIPT_Method(setf32);
  SCRIPT_Method(setbool);
  SCRIPT_Method(settext);
  SCRIPT_Method(setvec2);
  SCRIPT_Method(setvec3);
  SCRIPT_Method(setvec4);
  SCRIPT_Method(setquat);

  SCRIPT_ClassEnd();

    Component* Data::create(void)
    {
      return SCAST(Component*, new Data());
    }

    Data::Data( void )
    {
    }

    Data::~Data( void )
    {
      remRegistry();
    }

    Text Data::getTypeName( void ) const
    {
      return "Data";
    }

    void Data::addRegistry( void )
    {
      /*no system cares... SYSTEM->vRegisterComponent(getTypeID(), this);*/
    }

    void Data::remRegistry( void )
    {
      /*no system cares... SYSTEM->vRegisterComponent(getTypeID(), this);*/
    }

    TypeID Data::getTypeID( void ) const
    {
      return TYPE_ID(Data);
    }

    void Data::serialize( Scope& data, bool read )
    {
//       SERIAL_BeginSerializing(Transform);

    }

    void Data::serialize( Component const* source )
    {
    }

    u1 Data::handleEvent( hstring const& message, RawStore const& args, MessageType type, f32 timestamp )
    {
      MSG_StartHandling();
      MSG_DoneHandling();
    }

    //exists macro
#define IF_TYPE_MATCHES_RETURN_EXISTS(type) \
  if(TypeName == #type)                     \
  return m_##type##.find(varName)!=m_##type##.end()

    bool Data::exists( Text const& varName, Text TypeName )
    {
      IF_TYPE_MATCHES_RETURN_EXISTS(s32);
      IF_TYPE_MATCHES_RETURN_EXISTS(f32);
      IF_TYPE_MATCHES_RETURN_EXISTS(bool);
      IF_TYPE_MATCHES_RETURN_EXISTS(text);
      IF_TYPE_MATCHES_RETURN_EXISTS(vec2);
      IF_TYPE_MATCHES_RETURN_EXISTS(vec3);
      IF_TYPE_MATCHES_RETURN_EXISTS(vec4);
      IF_TYPE_MATCHES_RETURN_EXISTS(quat);

      return false;
    }



    /////// G E T  /////
    //////////////////////////////////////////////////////////////////////////
    //defines get function in a macro, _N_ is the names used in this class
    // _T_ is the c++ literal type it represents.
#define GET_VARIABLE_FUNCTION_IMPL(_N_, _T_)               \
    _T_ Data::get##_N_( Text const& getName ) const        \
    {                                                      \
      _N_##Data::const_iterator it = m_##_N_.find(getName);\
      if(it!=m_##_N_.end())                                \
        return it->second;                                 \
      else{                                                \
      ErrorIf(it==m_##_N_.end(), "Script called object.data.get%s(\"%s\") that was not found.", #_N_, getName.c_str());\
      return _T_();}                                       \
    }

    //f32 Data::getf32( Text const& getName ) const        
    //{                                                      
    //  f32Data::const_iterator it = m_f32.find(getName);
    //  if(it!=m_f32.end())                                
    //    return it->second;                                 
    //  else                                                 
    //    return f32();                                      
    //}

    GET_VARIABLE_FUNCTION_IMPL(s32, s32);
    GET_VARIABLE_FUNCTION_IMPL(f32, f32);
    GET_VARIABLE_FUNCTION_IMPL(bool, bool);
    GET_VARIABLE_FUNCTION_IMPL(text, Text);
    GET_VARIABLE_FUNCTION_IMPL(vec2, ScriptVec2);
    GET_VARIABLE_FUNCTION_IMPL(vec3, ScriptVec3);
    GET_VARIABLE_FUNCTION_IMPL(vec4, ScriptVec4);
    GET_VARIABLE_FUNCTION_IMPL(quat, Quat);




    /////// S E T ////////
    //////////////////////////////////////////////////////////////////////////
    //defines set function in a macro, _N_ is the names used in this class
    // _T_ is the c++ literal type it represents.
#define SET_VARIABLE_FUNCTION_IMPL(_N_,_T_)           \
    void Data::set##_N_( Text const& setName, _T_ val )  \
    {                                                 \
      m_##_N_[setName] = val;                         \
    }

    //void Data::setf32( Text const& setName, f32 val )  
    //{                                                 
    //  m_f32[setName] = val;                         
    //}

    SET_VARIABLE_FUNCTION_IMPL(s32, s32);
    SET_VARIABLE_FUNCTION_IMPL(f32, f32);
    SET_VARIABLE_FUNCTION_IMPL(bool, bool);
    SET_VARIABLE_FUNCTION_IMPL(text, Text);
    SET_VARIABLE_FUNCTION_IMPL(vec2, ScriptVec2);
    SET_VARIABLE_FUNCTION_IMPL(vec3, ScriptVec3);
    SET_VARIABLE_FUNCTION_IMPL(vec4, ScriptVec4);
    SET_VARIABLE_FUNCTION_IMPL(quat, Quat);

}//namespace
