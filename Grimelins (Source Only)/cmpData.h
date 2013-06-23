/*! ====================================================================================================================
  @file cmpData.h

  Grimelins 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jan.21.2012

  @brief 
  Generic Variable data container for scripts to store values that are object centric. For example if your object
  has "hit points" you can store such a value here, instead of creating a new component for each unique data that
  your object may have.

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIMELINS_CMPDATA_H
#define GRIMELINS_CMPDATA_H
#include "component.h"
namespace Grimelins
{

    SCRIPT_HeaderDefine(Data);

    class Data : public Component
    {
      friend class Object;
      friend class Factory;

    public:
      Data(void);
      virtual ~Data(void);

    public://Virtual interface
      virtual void addRegistry( void );
      virtual void remRegistry( void );
      virtual Text getTypeName(void) const;
      virtual TypeID getTypeID(void) const;
      virtual void serialize( Scope& data, bool read );
      virtual void serialize( Component const* source);
      virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp);

      bool exists(Text const& varName, Text TypeName);

      //get types
      s32 gets32(Text const& getName) const;
      f32 getf32(Text const& getName) const;
      bool getbool(Text const& getName) const;
      Text gettext(Text const& getName) const;
      ScriptVec2 getvec2(Text const& getName) const;
      ScriptVec3 getvec3(Text const& getName) const;
      ScriptVec4 getvec4(Text const& getName) const;
      Quat getquat(Text const& getName) const;

      //set types
      void sets32(Text const& setName, s32 val);
      void setf32(Text const& setName, f32 val);
      void setbool(Text const& setName, bool val);
      void settext(Text const& setName, Text val);
      void setvec2(Text const& setName, ScriptVec2 val);
      void setvec3(Text const& setName, ScriptVec3 val);
      void setvec4(Text const& setName, ScriptVec4 val);
      void setquat(Text const& setName, Quat val);

    private:
      static Component* create(void);

    private:
      typedef std::map<Text, s32> s32Data;
      typedef std::map<Text, f32> f32Data;
      typedef std::map<Text, bool> boolData;
      typedef std::map<Text, Text> textData;
      typedef std::map<Text, ScriptVec2> vec2Data;
      typedef std::map<Text, ScriptVec3> vec3Data;
      typedef std::map<Text, ScriptVec4> vec4Data;
      typedef std::map<Text, Quat> quatData;

      s32Data m_s32;
      f32Data m_f32;
      boolData m_bool;
      textData m_text;
      vec2Data m_vec2;
      vec3Data m_vec3;
      vec4Data m_vec4;
      quatData m_quat;

    };
};//namespace

#endif