/*! ====================================================================================================================
  @file symbols.h

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.03.2012

  @brief 
    Symbol declaration fetcher.

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once

namespace Grimelins {

// Build Enum
#define DECLARE_SYMBOL(sym) sym,
  enum Symbol
  {
    #include "symbolmerger.h"
    NUM_SYMBOLS,
    INVALID_SYMBOL,
    FORCE_INT = 0x7FFFFFFF
  };
#undef DECLARE_SYMBOL

#define DECLARE_SYMBOL(x) #x,
  static char const* gSymbolNames[] =
  {
    #include "symbolmerger.h"
    "NUM_SYMBOLS",
    "INVALID_SYMBOL"
  };
#undef DECLARE_SYMBOL

// Build Visual AssistX Helper Code
#ifdef VA_HELPER_CODE
#define DECLARE_SYMBOL(x) x,
  enum VA_HELPER_REGISTER_MESSAGE
  {
    #include "symbolmerger.h"
    NUM_SYMBOLS,
    INVALID_SYMBOL,
  };
#undef DECLARE_SYMBOL
#endif

  char const* GetSymbolName(unsigned int symbol);
  unsigned int FindSymbol(char const* symbolName);
  void FindIncompleteSymbols( char const* incompleteSymbol, std::vector<std::string>& outSymbols );

}