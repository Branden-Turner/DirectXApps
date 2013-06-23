/*! ====================================================================================================================
  @file stringutils.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Oct.14.2011

  @brief 
    String utilities

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_STRINGUTILS_H
#define VIRTUS_STRINGUTILS_H

namespace Grimelins
{
  
  std::vector<std::string> ExplodeString(ch const* delim, std::string const& str);

} // namespace Virtus

#endif // Include Guard