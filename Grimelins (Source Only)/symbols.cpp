/*! ====================================================================================================================
  @file symbols.cpp

  Project Grimelins

  @author Maximilian Manndorff <max@lo3.com>
  @date Feb.03.2012

  @brief 
    Symbols helper functions

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "symbols.h"

namespace Grimelins {

  char const* GetSymbolName(unsigned int symbol)
  {
    return gSymbolNames[symbol];
  }

  unsigned int FindSymbol(char const* symbolName)
  {
    for (int i = 0; i < NUM_SYMBOLS; ++i)
    {
      if (std::strcmp(symbolName, gSymbolNames[i]) == 0)
        return i;
    }

    return INVALID_SYMBOL;
  }

  void FindIncompleteSymbols(char const* incompleteSymbol, std::vector<std::string>& outSymbols)
  {
    int StringLength = std::strlen(incompleteSymbol);

    if (StringLength == 0)
      return;

    for (int i = 0; i < NUM_SYMBOLS; ++i)
    {
      for (int j = 0; j <= StringLength; ++j)
      {
        if (incompleteSymbol[j] == 0 )
          outSymbols.push_back( gSymbolNames[i] );

        if (incompleteSymbol[j] != gSymbolNames[i][j])
          break;
      }
    }
  }


} // Namespace Grimelins