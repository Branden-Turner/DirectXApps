/*! ====================================================================================================================
  @file scriptVM.cpp

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.18.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "scriptVM.h"

#include "appfiles.h"

namespace //unnamed
{
#if defined(_MSC_VER) && defined(_DEBUG)
  int MemAllocHook( int allocType, void *userData, size_t size, int blockType, long requestNumber, const unsigned char *filename, int lineNumber){return 1;}
#endif

  //Default way for scripts to execute print messages
  //This prints to the output window...
  void DefaultPrint(const char *s, va_list vl)
  {
    static const int BufSz = 8192;
    char msg[BufSz];
    vsnprintf_s(msg, BufSz, _TRUNCATE , s, vl); 
    OutputDebugStringA(msg);
  }

  //Function given to squirrel as the print function, this acts as the translator
  // from the print function squirrel requires, and the one defined in ScriptVM
  void printfunc(HSQUIRRELVM v,const SQChar *s,...)
  {
    va_list vl;
    va_start(vl, s);
    Grimelins::ScriptVM::scriptPrinter(s, vl);
    va_end(vl);
  }


}//unnamed namespace


namespace Grimelins
{

  ScriptVM::ScriptVM( void )
  {
    #if defined(_MSC_VER) && defined(_DEBUG)
      _CrtSetAllocHook(MemAllocHook);
    #endif
    m_vm = sq_open(1024);
    sq_setprintfunc(m_vm,printfunc);
    sq_pushroottable(m_vm);
    sqstd_register_bloblib(m_vm);
    sqstd_register_iolib(m_vm);
    sqstd_register_systemlib(m_vm);
    sqstd_register_mathlib(m_vm);
    sqstd_register_stringlib(m_vm);
    sqrat_register_importlib(m_vm);//SQRAT
    sqstd_seterrorhandlers(m_vm);
    Sqrat::DefaultVM::Set(m_vm);
    Sqrat::DefaultVM::Set(m_vm);
    Sqrat::DefaultVM::Set(m_vm);

    initScripting();
  }

  ScriptVM::~ScriptVM( void )
  {
    sq_close(m_vm);
  }

  ScriptVM::operator HSQUIRRELVM( void )
  {
    return m_vm;
  }

  bool ScriptVM::runScript( std::string const& file )
  {
    Sqrat::Script s(m_vm);
    Application::FilePath fp(file);
    std::string compileString;
    if(fp.isRelative())
    {
      compileString = Application::GetAssetDirectory("script");
      compileString += "\\" + fp.FileName();
    }
    else
      compileString = file;

    if(fp.Extension() == "")
      compileString += ".nut";

    try
    {
      s.CompileFile(compileString);
    }
    catch (Sqrat::Exception e)
    {
      std::string ss = "Error compiling " + compileString + " in squirrel virtual machine.";
      printDebug(ss.c_str());
      return false;
    }

    if(!s.IsCompiled())
      return false;

    try
    {
      s.Run();
    }
    catch( Sqrat::Exception e)
    {
      std::string ss = "Error running " + compileString + " in squirrel virtual machine.";
      printDebug(ss.c_str());
      return false;
    }
    return true;

  }

  bool ScriptVM::runString( std::string const& code )
  {
    Sqrat::Script s(m_vm);
    try
    {
      s.CompileString(code);
    }
    catch (Sqrat::Exception e)
    {
      std::string ss = "Error compiling string " + code + " in squirrel virtual machine.";
      printDebug(ss.c_str());
      return false;
    }

    if(!s.IsCompiled())
      return false;

    try
    {
      s.Run();
    }
    catch( Sqrat::Exception e)
    {
      std::string ss = "Error running compiled string " + code + " in squirrel virtual machine.";
      printDebug(ss.c_str());
      return false;
    }
    return true;

  }

  void ScriptVM::setPrintFunc( void )
  {
    ScriptVM::scriptPrinter = DefaultPrint;
  }

  void ScriptVM::setPrintFunc( ScriptPrinter fn )
  {
    ScriptVM::scriptPrinter = fn;
  }

  void ScriptVM::print( const char* s, va_list vl )
  {
    ScriptVM::scriptPrinter(s,vl);
  }

  void ScriptVM::initScripting( void )
  {
    runScript("_SQVMSetup.nut");
    m_setup = new Sqrat::Function(Sqrat::RootTable(m_vm), "SQVMsetup");
    ErrorIf(!m_setup, "ScriptVM failed to allocate heap memory for cheif setup function!");
    ErrorIf(m_setup->IsNull(), "ScriptVM failed to locate SQVMSetup function in _SQVMSetup.nut\n");
  }

  void ScriptVM::setContext( Text const& id, Text const& file )
  {
    m_setup->Execute(id, file);
  }
  Grimelins::ScriptPrinter ScriptVM::scriptPrinter = DefaultPrint;
};//Virtus namespace
