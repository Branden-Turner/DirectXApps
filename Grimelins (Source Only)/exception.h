/*! ====================================================================================================================
  @file exception.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 16.Sep.2011

  @brief 
    Virtus engine exception

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_EXCEPTION_H
#define VIRTUS_EXCEPTION_H

namespace Grimelins
{
  class Exception
  {
  public:
    enum Code
    {
      UNKNOWN
    };

  public:
    Exception(void) : mCode(UNKNOWN), mError(0)  {}
    Exception(u32 code, ch const* error = 0) : mCode(code), mError(error) {}
    Exception(ch const* error, u32 code = UNKNOWN) : mCode(UNKNOWN), mError(error) {}
    virtual ~Exception(void) {}

  public:
    virtual ch const* what(void) const { return mError; }
    virtual u32 code(void) const { return mCode; }

  private:
    u32 mCode;
    ch const* mError;
  };
}

#endif // include guard