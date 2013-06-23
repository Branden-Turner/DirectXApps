/*! ====================================================================================================================
  @file cfgfile.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 23.Sep.2011

  @brief 
    Loads configuration files

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_CFGFILE_H
#define VIRTUS_CFGFILE_H

namespace Grimelins
{
  
  class CfgFile
  {
    typedef std::vector<std::string> CfgLines;

  public:
    enum ErrorCodes
    {
      CFGERR_INVALIDFILE,
      CFGERR_FILENOTFOUND
    };

    CfgFile(std::string file);
    ~CfgFile(void);

  public:
    std::string GetCurrentFile(void) const;
    void Dispatch(void);

  private:
    void readFile(void);

  private:
    std::string mFile;
    CfgLines mLines;
  };

} // namespace Virtus

#endif // include guard