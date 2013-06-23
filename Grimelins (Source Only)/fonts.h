/*! ====================================================================================================================
  @file fonts.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.23.2011

  @brief
    Just some font related dataz.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_FONTS_H
#define VIRTUS_FONTS_H
#include "color.h"
namespace Grimelins {

    class DX9Texture;

    enum Font
    {
      Terminal_8,
      Terminal_12,
      ArialBlack_12,
      LucidaConsole_12,
      ComicSans_24,
      MaxFonts
    };

    enum TBAlign
    {
      AlignLeft,
      AlignCenter,
      AlignRight,
      MaxTextBoxAlignments
    };

    static const std::string FontFiles[] = 
    {
      "terminal8",
      "terminal12",
      "arialblack12",
      "lucida12",
      "comicsans24"
    };

    struct TextBox
    {
      TextBox::TextBox( void )
        : mAlign(AlignLeft),
          mColor(Green),
          mFont(Terminal_8),
          mStr(),
          mStartIndex( 0 ),
          mUL( vec2(5.0f, 5.0f) ),
          mBR( vec2(15.0f,15.0f) ),
          mUseOutline( false ),
          mWordWrap( false )
      {
      }

      TBAlign mAlign;
      Color   mColor;
      Font    mFont;
      std::vector<std::string> mStr;
      vec2    mUL;
      vec2    mBR;
      u32     mStartIndex;
      u1      mUseOutline;
      u1      mWordWrap;
    };

    struct ScreenQuad
    {
      vec2 pos;
      vec2 dim;
      f32  alpha;
      f32  texScale;
      DX9Texture* tex;

    };

} // namespace Virtus

#endif // Include Guard