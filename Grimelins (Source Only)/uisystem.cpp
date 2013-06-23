/*! ====================================================================================================================
  @file uisystem.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.07.2011

  @brief
    CPP file for the UI System

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

#include "precompiled.h"
#include "uisystem.h"
#include "dx9device.h"
#include "dx9shaders.h"
#include "dx9resourcemanager.h"
#include "dx9shapes.h"

namespace Grimelins {

UISystem::UISystem( DX9Device* device ) 
  : mDevice(device),
    mDXDevice(NULL),
    mRM(NULL),
    mDX9Shapes(NULL),
    mQuad(NULL),
    mPShader(NULL),
    mVShader(NULL),
    mInstanceBuffer(NULL)
{
  mRM        = mDevice->GetRM();
  mDXDevice  = mDevice->GetDevice();
  mDX9Shapes = mDevice->GetShapes();

  mVShader = mRM->GetVertexShader("dx9text.vs");
  mPShader = mRM->GetPixelShader ("dx9text.ps");
  mQuad    = mDX9Shapes->GetShape(ShapeQuad2D);

  mInstanceBuffer = mRM->GetVertexBuffer( TextInstanceBufferSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DPOOL_DEFAULT );

  std::string const& fontPath = CONFIG_PATH_FONT;

  for(u32 i = 0; i < MaxFonts; ++i)
  {
    LoadBitmapText( mFontData[i], fontPath + FontFiles[i] + ".fnt" );
    mTextures[i] = mRM->GetTextureFromFile( fontPath + FontFiles[i] + ".dds" );
  }
}

UISystem::~UISystem( void )
{
}

void UISystem::PrepareToDrawText( void )
{
  mDevice->SetShader( mVShader );
  mDevice->SetShader( mPShader );
  mDevice->SetDecl( TextInstance::Decl );
}

void UISystem::PrepareDraw( void )
{
  mDXDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
  mDXDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
  mDXDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

  mDXDevice->SetRenderState( D3DRS_ZENABLE, 0 );
  mDXDevice->SetRenderState( D3DRS_ZWRITEENABLE, 0 );
  mDXDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

  mDXDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, 1 );
  mDXDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
  mDXDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

  mDXDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

void UISystem::AddTextBox( TextBox const& tb )
{
  mTextBoxs[tb.mFont][tb.mColor].push_back(tb);
}

void UISystem::AddScreenQuad( ScreenQuad const& sq )
{
  mScreenQuads.push_back(sq);
}

void UISystem::ClearFrameData( void )
{
  for(u32 i = 0; i < MaxFonts; ++i)
    mTextBoxs[i].clear();

  mScreenQuads.resize(0);
  mUIElements.resize(0);
}

void UISystem::ScreenToNDCText( vec2& pos, vec2 const& sdim )
{
  pos += vec2(0.5f, 0.5f);
  pos /= sdim;
  pos *= vec2(2.0f, -2.0f);
  pos += vec2(-1.0f, 1.0f);
}

void UISystem::ScreenToNDC( vec2& pos, vec2 const& sdim )
{
  pos /= sdim;
  pos *= vec2(2.0f, -2.0f);
  pos += vec2(-1.0f, 1.0f);
}

void UISystem::TextSetTexture( DX9Texture* tex )
{
  mDevice->SetTexture( tex );
}

void UISystem::TextSetFaceColor( u32 index )
{
  mDevice->SetPSData( &GetColorV((Color)index), 1, offsetof(DebugLinePSData, Color) / 16 );
}

void UISystem::TextSetUseOutline( u1 useOutline )
{
  mTextVars.x = (useOutline) ? 1.0f : 0.0f;

  mDevice->SetPSData( &mTextVars, 1, offsetof(DebugLinePSData, Vars) / 16 );
}

f32 UISystem::TextGetPixelWidth( std::string const& str, u32 index )
{
  f32 tmp = 0.0f;

  for(u32 i = 0, iend = str.size(); i < iend; ++i)
    tmp += mFontData[index].mCharacters[str[i]].mScale.x;

  return tmp;
}

void UISystem::TextSendBatch( u32 numQuads )
{
  if(numQuads == 0)
    return;

  mDevice->SetStreamFreq( 0, (D3DSTREAMSOURCE_INDEXEDDATA | numQuads) );
  mDevice->SetStreamFreq( 1, (D3DSTREAMSOURCE_INSTANCEDATA | 1) );
  mDevice->SetBuffer( mQuad->mIBuffer );
  mDevice->SetBuffer( mQuad->mVBuffer, sizeof(Vertex_XY_UV), 0 );
  mDevice->SetBuffer( mInstanceBuffer, sizeof(TextInstance), 1 );

  mDevice->DrawIndexedTris( mQuad->mNumTris, mQuad->mNumVerts );

  mDevice->SetStreamFreq( 0, 1 );
  mDevice->SetStreamFreq( 1, 1 );
}

void UISystem::RenderText( void ) 
{ 
  vec2& screenDim = mDevice->GetScreenDim();
  vec2& halfScreenDim = screenDim * 0.5f;
  u32   numQuads = 0;

  for( u32 findex = 0; findex < MaxFonts; ++findex ) 
  {
    for( TextBoxDataIter ib = mTextBoxs[findex].begin(), ie = mTextBoxs[findex].end(); ib != ie; ++ib ) 
    {
      TextBoxes const& vec = ib->second;

      Color cFaceColor = ib->first;

      TextInstance* vBuffer = NULL;
      mInstanceBuffer->Lock(vBuffer, D3DLOCK_DISCARD);

      for(u32 k = 0; k < vec.size(); ++k)
      { 
        u1 UseOutline = vec[k].mUseOutline;
        u1 WordWrap   = vec[k].mWordWrap;

        FontData const& fdata = mFontData[vec[k].mFont];   

        vec2 iCursor = vec[k].mUL;
        vec2 cursor = iCursor;
        vec2 const& texDim  = fdata.mTextureDim;
        vec2 const& boxDim = vec[k].mBR - vec[k].mUL;

        u1 mContinue = false;
        u1 mMultipleBatch = false;

        
        u32 strIndex = vec[k].mStartIndex;

        for(u32 lIndex = 0, lEnd = vec[k].mStr.size(); lIndex < lEnd; ++ lIndex)
        {
          u32 inc = vec[k].mStr[lIndex].size();

          if( strIndex >= inc && (lIndex != lEnd - 1) )
          {
            cursor.y += fdata.mLineHeight;
            continue;
          }

          std::string st;
          if( lIndex == lEnd - 1)
            st = vec[k].mStr[lIndex];
          else
            st = (inc > strIndex ) ? vec[k].mStr[lIndex].substr(strIndex) : vec[k].mStr[lIndex] ;

          std::vector<std::string> const& words = ExplodeString(" ", st);

          f32 outlineOffset = ( (UseOutline) ? ( fdata.mOutlineSize ) : 0.0f );

          cursor.x = iCursor.x;

          for(u32 i = 0, iend = words.size(); i < iend; ++i)
          {
            if( (cursor.x - iCursor.x) + TextGetPixelWidth( words[i], findex) > boxDim.x )
            {
              if( WordWrap )
              {
                cursor.x = iCursor.x + fdata.mCharacters[' '].mXAdvance * 2.0f;
                cursor.y += fdata.mLineHeight;
              }
              else
              {
                cursor.x = iCursor.x;
                cursor.y += fdata.mLineHeight;
                break;
              }
            }
            
            for(u32 j = 0, jend = words[i].size(); j < jend; ++j)
            {
              std::string const& str = words[i];

              u1 mContinue = false;

              switch(str[j])
              {
              case '\t':
                cursor.x += fdata.mCharacters[' '].mXAdvance * 2.0f;
                mContinue = true;
                break;

              case '\n':
                cursor.x = iCursor.x;
                cursor.y += fdata.mLineHeight;
                mContinue = true;
                break;
              };

              if(mContinue)
                continue;

              vec2 charScalePixels = fdata.mCharacters[str[j]].mScale;

              vec2 texcoord = fdata.mCharacters[str[j]].mPos / texDim;
              vec2 texscale = charScalePixels / texDim;

              vec2 charHalfDim = (charScalePixels * 0.5f);
              vec2 charScale = (charScalePixels / halfScreenDim);

              vec2 pos = cursor + fdata.mCharacters[str[j]].mOffset + charHalfDim;

              ScreenToNDCText(pos, screenDim);

              vBuffer[numQuads].pos_sca = vec4( pos.x, pos.y, charScale.x, charScale.y );
              vBuffer[numQuads].tex_off = vec4( texcoord.x, texcoord.y, texscale.x, texscale.y );

              cursor.x += fdata.mCharacters[str[j]].mXAdvance + outlineOffset;

              if( j < jend )
                cursor.x += fdata.mKerning[str[j]][str[j+1]];

              ++numQuads;
            }

            cursor.x += fdata.mCharacters[' '].mXAdvance + outlineOffset;
          }
        }

        mInstanceBuffer->Unlock();

        TextSetFaceColor(cFaceColor);
        TextSetUseOutline(UseOutline);
        TextSetTexture(mTextures[findex]);
        TextSendBatch(numQuads);

        numQuads = 0;
      }
    } 
  }  
}

void UISystem::Draw( void )
{
  RenderQuads();
  RenderUIElements();
  PrepareToDrawText();
  RenderText();
  ClearFrameData();
}

void UISystem::LoadBitmapText( FontData& fdata, std::string const& filename )
{
  std::fstream file( filename.c_str() );

  if( !file.good() ) 
  {
    file.close();
    return;
  }

  std::string Line, Read, Key, Value;
  std::size_t i, j;

  while( !file.eof() )
  {
    std::stringstream LineStream;
    std::getline( file, Line );
    LineStream << Line;

    //read the line's type
    LineStream >> Read;

    if( Read == "info" )
    {
      while( !LineStream.eof() )
      {
        std::stringstream Converter;
        LineStream >> Read;
        std::string FileString;

        i = Read.find( '=' );
        j = Read.find( '"', i + 2 );
        Key = Read.substr( 0, i );
        Value = Read.substr( i + 1 );
        FileString = Read.substr( i + 2, j - i - 2);

        //assign the correct value
        Converter << Value;

        if( Key == "face" )
          fdata.mFaceName = FileString;
        else if( Key == "outline" )
          Converter >> fdata.mOutlineSize;
      }
    }
    else if( Read == "common" )
    {
      //this holds common data
      while( !LineStream.eof() )
      {
        std::stringstream Converter;
        LineStream >> Read;

        i = Read.find( '=' );
        Key = Read.substr( 0, i );
        Value = Read.substr( i + 1 );

        //assign the correct value
        Converter << Value;

        if( Key == "scaleW" )
          Converter >> fdata.mTextureDim.x;
        else if( Key == "scaleH" )
          Converter >> fdata.mTextureDim.y;
        else if( Key == "lineHeight" )
          Converter >> fdata.mLineHeight;
      }
    }
    else if( Read == "char" )
    {
      //this is data for a specific char
      u32 CharID = 0;

      while( !LineStream.eof() )
      {
        std::stringstream Converter;
        LineStream >> Read;
        i = Read.find( '=' );
        Key = Read.substr( 0, i );
        Value = Read.substr( i + 1 );

        //assign the correct value
        Converter << Value;
        if( Key == "id" )
          Converter >> CharID;
        else if( Key == "x" )
          Converter >> fdata.mCharacters[CharID].mPos.x;
        else if( Key == "y" )
          Converter >> fdata.mCharacters[CharID].mPos.y;
        else if( Key == "width" )
          Converter >> fdata.mCharacters[CharID].mScale.x;
        else if( Key == "height" )
          Converter >> fdata.mCharacters[CharID].mScale.y;
        else if( Key == "xoffset" )
          Converter >> fdata.mCharacters[CharID].mOffset.x;
        else if( Key == "yoffset" )
          Converter >> fdata.mCharacters[CharID].mOffset.y;
        else if( Key == "xadvance" )
          Converter >> fdata.mCharacters[CharID].mXAdvance;
        else if( Key == "page" )
          Converter >> fdata.mCharacters[CharID].mPageIndex;
      }
    }
    else if ( Read == "kerning" )
    {
      s32 first, second;
      f32 amount;

      while( !LineStream.eof() )
      {
        std::stringstream Converter;
        LineStream >> Read;
        i = Read.find( '=' );
        Key = Read.substr( 0, i );
        Value = Read.substr( i + 1 );

        //assign the correct value
        Converter << Value;
        if( Key == "first" )
          Converter >> first;
        else if( Key == "second" )
          Converter >> second;
        else if( Key == "amount" )
          Converter >> amount;
      }

      fdata.mKerning[first][second] = amount;
    }
  }
}

void UISystem::RenderQuads( void )
{
  static DX9PixelShader* quadps = mRM->GetPixelShader("dx9screenquad.ps");
  static DX9VertexShader* quadvs = mRM->GetVertexShader("dx9screenquad.vs");
  static Shape* quad = mDevice->GetShapes()->GetShape(ShapeQuad2D);

  u32 numQuads = mScreenQuads.size();

  if(numQuads == 0)
    return;

  mDevice->SetShader(quadvs);
  mDevice->SetShader(quadps);

  mDevice->SetBuffer(quad->mIBuffer);
  mDevice->SetBuffer(quad->mVBuffer, sizeof(Vertex_XY_UV) );
  mDevice->SetDecl(quad->mDecl);

  for(u32 i = 0; i < numQuads; ++i)
  {
    float2 dim = mScreenQuads[i].dim;
    float2 pos = mScreenQuads[i].pos;

    float2 screenDim = mDevice->GetScreenDim();

    ScreenToNDC(pos, screenDim);

    screenDim /= 2.0f;

    dim /= screenDim;

    float4 PSVars(0.0f, 0.0f, 0.0f, mScreenQuads[i].alpha);

    float4 VSPos( pos.x, pos.y, dim.x, dim.y );

    mDevice->SetVSData(&VSPos, 1, 0);
    mDevice->SetPSData(&PSVars, 1, 0);

    mDevice->DrawIndexedTris(quad->mNumTris, quad->mNumVerts);
  }
}

void UISystem::AddUIElement( UIElement const& elem )
{
  mUIElements.push_back(elem);
}

void UISystem::RenderUIElements( void )
{
  static DX9PixelShader* uips = mRM->GetPixelShader("dx9uielement.ps");
  static DX9VertexShader* uivs = mRM->GetVertexShader("dx9uielement.vs");
  static Shape* quad = mDevice->GetShapes()->GetShape(ShapeQuad2D);

  u32 numelements = mUIElements.size();

  if(numelements == 0)
    return;

  mDevice->SetShader(uivs);
  mDevice->SetShader(uips);

  mDevice->SetBuffer(quad->mIBuffer);
  mDevice->SetBuffer(quad->mVBuffer, sizeof(Vertex_XY_UV) );
  mDevice->SetDecl(quad->mDecl);

  float2 screenDim = mDevice->GetScreenDim();

  for(u32 i = 0; i < numelements; ++i)
  {
    float2 dim = mUIElements[i].dim;
    float2 pos = mUIElements[i].pos;

    mDevice->SetTexture(mUIElements[i].tex);

    pos *= screenDim;

    ScreenToNDC(pos, screenDim);

    dim *= 2;

    float4 PSVars(0.0f, 0.0f, 0.0f, mUIElements[i].alpha);

    float4 VSPos( pos.x, pos.y, dim.x, dim.y );

    mDevice->SetVSData(&VSPos, 1, 0);
    mDevice->SetPSData(&PSVars, 1, 0);

    mDevice->DrawIndexedTris(quad->mNumTris, quad->mNumVerts);
  }
}

void UISystem::ClearUIElemByType( UIType type )
{
  std::remove(mUIElements.begin(), mUIElements.end(), type);
}


bool UIElement::operator==( const UIType _type )
{
  return type == _type;
}

UIElement::UIElement() : pos(0.0f, 0.0f), dim(0.0f, 0.0f), alpha(1.0f), tex(NULL), type(HUD)
{
}

} // namespace Views