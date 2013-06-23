/*! ====================================================================================================================
  @file uisystem.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.07.2011

  @brief
    Header for the UI System of the Virtus Engine

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_UISYSTEM_H
#define VIRTUS_UISYSTEM_H

#include "dx9.h"
#include "texttype.h"
#include "fonts.h"
#include "dx9shaders.h"
#include "dx9vertextype.h"
#include "dx9shapes.h"

namespace Grimelins {

    class DX9Device;
    class DX9VertexBuffer;
    class DX9ResourceManager;
    class DX9Texture;

    enum UIType
    {
      HUD,
      numUITypes
    };

    struct UIElement
    {
      UIElement();
      Text fnName;
      vec2 pos;
      vec2 dim;
      f32  alpha;
      DX9Texture* tex;
      DX9Texture* tex1;
      DX9Texture* tex2;
      UIType type;
      bool operator==(const UIType type);
    };

    const u32 TextInstanceBufferSize = 8192 * sizeof(TextInstance);

    class UISystem
    {
    public:
      UISystem( DX9Device* device );
      ~UISystem( void );


      void AddTextBox( TextBox const& tb );
      void AddScreenQuad( ScreenQuad const& sq );
      void AddUIElement( UIElement const& elem );
      void ClearUIElemByType( UIType type );

      void PrepareDraw( void );
      void Draw( void );

      typedef std::vector<TextBox> TextBoxes;
      typedef TextBoxes::const_iterator TextBoxesConstIter;
      typedef TextBoxes::iterator TextBoxesIter;

      typedef std::map< Color, TextBoxes > TextBoxData;
      typedef TextBoxData::const_iterator TextBoxDataConstIter;
      typedef TextBoxData::iterator TextBoxDataIter;

    public:
      UISystem( void ) {};

  
      void ScreenToNDCText( vec2& pos, vec2 const& sdim );

      void PrepareToDrawText( void );

      void RenderText( void );
      void RenderQuads( void );
      void RenderUIElements( void );

      void LoadBitmapText( FontData& fdata, std::string const& filename );

      void ClearFrameData( void );

      void TextSetUseOutline( u1 useOutline );
      void TextSetFaceColor( u32 index );
      void TextSendBatch( u32 num );
      void TextSetTexture( DX9Texture* tex );
      f32  TextGetPixelWidth( std::string const& str, u32 index );
      void ScreenToNDC( vec2& pos, vec2 const& sdim );
      TextBoxData mTextBoxs[MaxFonts];
      FontData    mFontData[MaxFonts];
      DX9Texture* mTextures[MaxFonts];

      std::vector<ScreenQuad> mScreenQuads;
      std::vector<UIElement> mUIElements;

      float4 mTextVars;

      Shape* mQuad;
      DX9Device* mDevice;
      DXDevice* mDXDevice;
      DX9ResourceManager* mRM;
      DX9Shapes* mDX9Shapes;

      DX9PixelShader* mPShader;
      DX9VertexShader* mVShader;
      DX9VertexBuffer* mInstanceBuffer;
      
    };
} // namespace Views

#endif // Include Guard
