/*! ====================================================================================================================
  @file cfgfile.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 23.Sep.2011

  @brief 
    Configuration file parser

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "cfgfile.h"

namespace Grimelins
{
  
  CfgFile::CfgFile(std::string file) : mFile(file)
  {
    if (file.empty())
      throw Exception("Can not instantiate CfgFile with empty file name!", CFGERR_INVALIDFILE);

    readFile();
  }

  CfgFile::~CfgFile(void)
  {

  }

  std::string CfgFile::GetCurrentFile(void) const
  {
    return mFile;
  }

  void CfgFile::Dispatch(void)
  {
    CfgLines::const_iterator it = mLines.begin();
    CfgLines::const_iterator endIt = mLines.end();

    for (; it != endIt; ++it)
      gCore->Events.Execute(*it);
  }

  void CfgFile::readFile(void)
  {
    Assert(mLines.empty());
    FileStream f;

    f.newStream(mFile);

    ErrorIf(!f.hasData(), "Could not open specified configuration file (%s).", mFile.c_str());

    while(f.hasData())
      mLines.push_back(f.read<Text>());
  }




} // namespace Virtus