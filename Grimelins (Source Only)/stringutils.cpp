/*! ====================================================================================================================
  @file stringutils.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Oct.14.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "stringutils.h"

namespace Grimelins
{
  std::vector<std::string> ExplodeString(ch const* delimeter, std::string const& str)
  {
    std::vector<std::string> result;

    size_t offset = 0;

    do
    {
      size_t end = str.find_first_of(delimeter, offset);

      if (end != std::string::npos)
        result.push_back(str.substr(offset, end - offset));
      else
      {
        result.push_back(str.substr(offset, end)); // Last chunk
        break;
      }

      offset = end + 1;

    } while(1);

    return result;
  }


} // namespace Virtus