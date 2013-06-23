/*! ====================================================================================================================
  @file scriptVM.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.18.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#ifndef VIRTUS_SCRIPTVM_H
#define VIRTUS_SCRIPTVM_H
#include "scriptInclude.h"
namespace Grimelins
{


//   typedef void(*ClassBindFunc)(void);
  typedef void(*ScriptPrinter)(const char *s, va_list vl);

  class ScriptVM : public Singleton<ScriptVM>
  {
  public:
    ScriptVM(void);
    ~ScriptVM(void);
    operator HSQUIRRELVM(void);

    bool runScript(std::string const& file);
    bool runString(std::string const& code);
    void setContext(Text const& id, Text const& file);

    void setPrintFunc(void);//sets to default
    void setPrintFunc(ScriptPrinter fn);
    void print(const char* s, va_list vl);

    template<typename T>
    void Bind(const char* name, T* instance)
    {
      Sqrat::RootTable().SetInstance( name, instance);
    }

    static ScriptPrinter scriptPrinter;
  private:
    void initScripting(void);

    HSQUIRRELVM m_vm;
    Sqrat::Function* m_setup;
  };

};//Virtus namespace
#endif //VIRTUS_SCRIPTVM_H