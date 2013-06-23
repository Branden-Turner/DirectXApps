/*! ====================================================================================================================
  @file filepath.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.24.2011

  @brief 
    Defines abstraction for file paths in the OS.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#ifndef VIRTUS_APPFILES_H
#define VIRTUS_APPFILES_H

namespace Grimelins
{
  namespace Application
  {

    extern const ch PATH_DELIMETER;

    void GetPathTree(std::vector<std::string>* outPathTree, std::string path);
    
    std::string GetWorkingDirectory(void);
    std::string GetAssetDirectory(std::string subdirectory = "");
    std::string GetFullPath(std::string relpath);

    class FilePath
    {
    public: // Autos
      FilePath(std::string path);

    public: // Ops
      FilePath& operator=(std::string path);

    public: // Meth
      std::string RelPath(void) const;
      std::string AbsPath(void) const;
      std::string Extension(void) const;
      std::string Name(void) const;
      std::string Original(void) const;
      std::string FileName(void) const;
      bool isRelative(void) const;


      std::vector<std::string> Tree(void) const;
      u1 Exists(void) const;

    private: // Meth
      void setup(std::string path);

    private: // Dat
      u1 mRel;
      std::string mOriginal;
      std::string mRelPath;
      std::string mAbsPath;
      std::string mExt;
      std::string mName;
    };
  }//namespace Application
} // namespace Virtus

#endif // include guard
