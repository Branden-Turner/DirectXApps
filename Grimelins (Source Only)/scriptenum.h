/*! ====================================================================================================================
  @file scriptEnum.h

  Codename Grimelins

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Sep.30.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIMELINS_SCRIPTENUM_H
#define GRIMELINS_SCRIPTENUM_H

namespace Grimelins
{


  class ScriptENUM
  {
  public:
    ScriptENUM(Text const& script_container);
    //create an enum in script, and return the assigned value
    void newEnum(Text const& name, s32 val);

    //locate the value of a created enum by name
    int getEnum(Text const& name);

    //locate the name of an enum by it's value
    std::string getEnum(s32 const& val);

    void bind(void);

  private:
    typedef std::map<Text, s32> NameMap;
    typedef std::map<s32, Text> GUIDMap;
    NameMap m_byName;
    GUIDMap m_byGUID;
    Sqrat::Enumeration m_enum;
    Text m_container;
  };


};//Virtus namespace
#endif //VIRTUS_SCRIPTENUM_H
