/*! ====================================================================================================================
  @file texttype.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.07.2011

  @brief
    Assorted data structures for the Text System

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_TEXTTYPE_H
#define VIRTUS_TEXTTYPE_H

namespace Grimelins {

struct Character
{
  Character(void) : mXAdvance(0), mPageIndex(0) {}
  vec2 mPos;
  vec2 mScale;
  vec2 mOffset;
  s32 mXAdvance;
  s32 mPageIndex;
};

struct FontData
{
  FontData(void)
  {
    std::memset( mKerning, 0, sizeof(mKerning) );
    std::memset( mCharacters, 0, sizeof(mCharacters) );
  }
    // Waste a bit of space, but constant time access.
  f32       mKerning[256][256];
  Character mCharacters[256];
  f32    mLineHeight;
  std::string mFaceName;
  vec2   mTextureDim;
  f32    mOutlineSize;
};

} // namespace Views

#endif // Include Guard