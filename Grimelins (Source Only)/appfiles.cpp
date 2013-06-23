/*! ====================================================================================================================
  @file filepath.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.24.2011

  @brief 
    File path abstraction and helper utilities.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include <direct.h>
#include "appfiles.h"
#include "stringutils.h"

namespace Grimelins
{
  namespace Application
  {

    static const ch PATH_DELIMETER = '\\';
    static const unsigned STR_BUFF_LEN = 1024;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static File I/O
    
    std::string GetFullPath(std::string relPath)
    {
      char outBuffer[STR_BUFF_LEN];
      u32 count = GetFullPathNameA(relPath.c_str(), STR_BUFF_LEN, outBuffer, 0);

      Assert(count != STR_BUFF_LEN, "Buffer overrun prevented, behavior undefined!");
      return std::string(outBuffer);
    }

    
    void GetPathTree(std::vector<std::string>* outPathTree, std::string path)
    {
      outPathTree->clear();

      u32 offset = path.find_last_of("/\\");

      if (path.find_first_of(".", offset) != std::string::npos)
        path.erase(offset);

      *outPathTree = ExplodeString("/\\", path);
    }

    std::string GetWorkingDirectory(void)
    {
      ch buff[STR_BUFF_LEN];
      return std::string(_getcwd(buff, 512));
    }

    std::string GetAssetDirectory( std::string subdirectory /*= ""*/ )
    {
      return GetWorkingDirectory() + "\\assets" + (subdirectory.length() ? ("\\" + subdirectory) : "");
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FilePath
    // 

    /////////// Autos ////////////

    FilePath::FilePath(std::string path) : mOriginal(path)
    {
      setup(path);
    }


    /////////// Ops ////////////

    FilePath& FilePath::operator=(std::string path)
    {
      setup(path);
      return *this;
    }
   
    std::string FilePath::RelPath(void) const
    {
      return mRelPath;
    }
    
    std::string FilePath::AbsPath(void) const
    {
      return mAbsPath;
    }
    
    std::string FilePath::Extension(void) const
    {
      return mExt;
    }
    
    std::string FilePath::Name(void) const
    {
      return mName;
    }

    std::string FilePath::Original(void) const
    {
      return mOriginal;
    }

    std::string FilePath::FileName( void ) const
    {
      return mName + ( (mExt!="") ? ("."+ mExt): "" );
    }

    bool FilePath::isRelative( void ) const
    {
      return mRel;
    }

    std::vector<std::string> FilePath::Tree(void) const
    { 
      std::vector<std::string> tree;
      GetPathTree(&tree, mAbsPath);

      return tree;
    }

    u1 FilePath::Exists(void) const
    {
      FILE* fp = fopen(mAbsPath.c_str(), "rb");

      if (fp)
      {
        fclose(fp);
        return true;
      }

      return false;
    }


    /////////// Private Methods ////////////
    class slashPred
    {
    public:
      bool operator()(const char ch) const
      {
        return ch == '\\';
      }
    };
    void FilePath::setup(std::string path)
    {
      mRel = true;//default is relative

      if( path.size() == 0 )
        return;

      if (path[1] == ':') // Absolute path given
      {
        mRel = false;
        path[0] = tolower(path[0]); // Lower case drive letter
        mAbsPath = path;
      }
      else mRel = true;

      //convert to uniform \ or / ::: Jonathan added 11/4/2011
      std::replace_if(path.begin(), path.end(), slashPred(), '/');

      // Isolate extension
      u32 offDot = path.find_last_of('.');
      if (offDot == std::string::npos || path[offDot + 1] == '/')
        mExt.clear();
      else
        mExt = path.substr(offDot + 1, path.size() - offDot);

      // Isolate Name
      u32 offName = path.find_last_of('/');
      u32 offAdd = 1;
      if (offName == std::string::npos)
      {
        offName = 0;
        offAdd = 0;
      }
      
      mName = path.substr(offName + offAdd, offDot - offName-offAdd);


      // Compute absolute / relative path
      std::string wd = GetWorkingDirectory();
      std::vector<std::string> absPathTree;
      GetPathTree(&absPathTree, wd);

      if (!mRel) // Absolute path given
      {
        std::vector<std::string> pathTree;
        GetPathTree(&pathTree, path);

        unsigned i = 0;

        mRelPath.clear();
        std::string pathToFork;

        while (1)
        {
          if (pathTree[i] != absPathTree[i]) // Fork Found
          {
              // Walk up to fork
            for (u32 j = i; j < absPathTree.size()-1; ++j)
              mRelPath += "../";

            for (u32 j = i+1; j < pathTree.size(); ++j)
              mRelPath += pathTree[j] + "/";

            mRelPath += mName + "." + mExt;
            break;
          }
          else
            pathToFork += absPathTree[i] + "/";

            // Can walk down both trees
          if (i < pathTree.size() && i < absPathTree.size())
            ++i;
        }

      }
      else // Relative path given
      {
        mRelPath = path;
        u32 offset = 0;
        u32 lastDot = 0;
        std::string delim("..");

        while (1)
        {
          offset = path.find(delim, offset);

          if (offset != std::string::npos) // walking up path
          {
            absPathTree.pop_back();
          }
          else
          {
            mAbsPath.resize(0);

            std::vector<std::string>::iterator it = absPathTree.begin();
            std::vector<std::string>::iterator endIt = absPathTree.end();

            for (; it != endIt; ++it)
              mAbsPath += *it + "/";

            mAbsPath += path.substr(lastDot, path.size() - offset);
            break;
          }
         
          offset += 3; // Skip ../ in string
          lastDot = offset;
        }
      }
    }


  } // namespace Application
} // namespace Virtus