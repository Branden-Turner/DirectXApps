/*! ====================================================================================================================
  @file devconsole.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.28.2011

  @brief
    Development Console!

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "devconsole.h"
#include "uisystem.h"
#include "graphics.h"
#include "dx9device.h"

namespace Grimelins {

cvar cvDevLines("con_lines", "Number of lines to display in the console", 24, 4, 48);
cvar cvFont("con_font", " Terminal_8 = 0, Terminal_12 = 1, ArialBlack_12 = 2, LucidaConsole_12 = 3, ComicSans_24 = 4", 0, 0, 4);

DevConsole::DevConsole( Graphics* pnt )
  : mCurrentLine(""),
    mGraphics(pnt),
    mUI(NULL),
    mActive(false),
    mTextBox(),
    mCommandOffset(0),
    mLinesToDraw(24),
    mLineNumber(0),
    mIndexOffset(0),
    mStringOffset(0)
{
  mBuffer.reserve(256);
  mCommandBuffer.reserve(256);

  mTextBox.mUseOutline = true;

  mUI = mGraphics->mUISystem;

  gCore->Events.Subscribe( H("typewriter"),  this );
  gCore->Events.Subscribe( H("con_toggle"),  this );
  gCore->Events.Subscribe( H("con_print"),   this );
  gCore->Events.Subscribe( H("con_left"),    this );
  gCore->Events.Subscribe( H("con_right"),   this );
  gCore->Events.Subscribe( H("con_prevcmd"), this );
  gCore->Events.Subscribe( H("con_nextcmd"), this );
  gCore->Events.Subscribe( H("con_scroll"),  this );

}

DevConsole::~DevConsole( void )
{

}

static ch const* MitchQuotes[] = {
  "\"Sometimes I wave to people I don't know. It's very dangerous to wave to someone you don't know because, what if they don't have a hand? They'll think you're cocky. \"Look what I got motherfucker! This thing is useful. I'm gonna go pick something up!\"\"\n",
  "\"I used to do drugs. I still do, but I used to, too.\"\n",
  "\"I haven't slept for ten days, because that would be too long.\"\n",
  "\"I'm an ice sculptor - last night I made a cube.\"\n",
  "\"Here's a thought for sweat shop owners: Air Conditioning! Problem solved...\"\n",
  "\"My fake plants died because I did not pretend to water them.\"\n",
  "\"I was walking down the street with my friend and he said, \"I hear music\", as if there is any other way you can take it in. You're not special, that's how I receive it too. I tried to taste it but it did not work.\"\n",
  "\"I was gonna get a candy bar; the button I was supposed to push was \"HH\", so I went to the side, I found the \"H\" button,I pushed it twice. Fuckin'...potato chips came out, man, because they had an \"HH\" button for Christ's sake! You need to let me know. I'm not familiar with the concept of \"HH\". I did not learn my AA-BB-CC's. God god, dammit dammit.\"\n",
  "\"By the way, you don't have to be sweaty and holding a basketball to enjoy a Gatorade. You could just be...a thirsty dude! Gatorade forgets about this demographic!\"\n",
  "\"I saw a commercial on late night TV, it said,\"Forget everything you know about slipcovers.\" So I did. And it was a load off my mind. Then the commercial tried to sell me slipcovers, and I didn't know what the fuck they were.\"\n",
  "\"I like vending machines 'cause snacks are better when they fall. If I buy a candy bar at a store, oftentimes, I will drop it... so that it achieves its maximum flavor potential.\"\n",
  "\"I got a 'do not disturb' sign on my hotel door. It says 'do not disturb.' Its time to go with 'don't disturb.' Its been 'do not disturb' for too long. We need to embrace the contraction.\""
    "'Don't disturb,' 'Do Not' psyches you out. \"'Do,' alright I need to disturb this guy... 'Not,' SHIT! I need to read faster!\" I like to wear 'do not disturb' signs around my neck so that little kids can't tell me knock knock jokes. Say, \"How you doin', nephew.\" \"Knock Knock?\" \"Read the sign, punk!\"\"\n"
};

static u32 numMitchQuotes = sizeof(MitchQuotes) / sizeof(void*);

void DevConsole::Update( f32 dt )
{
  static f32 mTimeAccum = 0.0f;

  if(!mActive)
    return;

  mTimeAccum += dt;

  if(mTimeAccum > 0.5f)
  {
    mCursorFlip = !mCursorFlip;
    mTimeAccum = 0.0f;
  }

  mLinesToDraw = cvDevLines;
  mTextBox.mFont = (Font)(int)cvFont;

  mTextBox.mStartIndex = mStringOffset;

  u32 totalStrings = mBuffer.size();

  mIndex = 0;
  mPadding = 0;

  if( totalStrings < mLinesToDraw )
    mPadding = mLinesToDraw - totalStrings;
  else
    mIndex = totalStrings - mLinesToDraw;

  for( u32 i = 0; i < mPadding; ++i )
    mTextBox.mStr.push_back("\n");

  for(u32 i = mIndex - mIndexOffset; i < totalStrings - mIndexOffset; ++i)
    mTextBox.mStr.push_back(mBuffer[i]);

  std::string cursor = (mCursorFlip) ? " " : "_";
  
  mTextBox.mStr.push_back(std::string("> ") + mCurrentLine + cursor);

  ScreenQuad sq;

  sq.alpha = 0.66f;
  sq.dim = mGraphics->mDevice->GetScreenDim();
  sq.texScale = sq.dim.x / 256.0f;

  sq.dim.y = ( mUI->mFontData[mTextBox.mFont].mLineHeight ) * (mLinesToDraw + 2) + 5.0f;
  mTextBox.mBR.x = sq.dim.x;
  sq.pos = sq.dim / 2.0f;

  mUI->AddScreenQuad(sq);
  mUI->AddTextBox(mTextBox);

  mTextBox.mStr.resize(0);
}

bool DevConsole::handleEvent( hstring const& message, RawStore const& args, MessageType type, f32 timestamp )
{
  //////////////////////////////////////////////////////////////////////////
  // msg: con_toggle
  // use: Toggle the console
  //////////////////////////////////////////////////////////////////////////

  OnMessage(con_toggle)

  RawStore ar;
  ar.AddU32("subscriber", GetMyListenerHandle());

  mActive = !mActive;

  if(mActive)
    gCore->Events.Request( H("input_claim_focus"), ar );
  else
    gCore->Events.Request( H("input_release_focus"), ar );

  ConsumeMessage

  if(!mActive) 
    return false;

  //////////////////////////////////////////////////////////////////////////
  // msg: con_left
  // use: shifts the displayed text in the console to the left
  //////////////////////////////////////////////////////////////////////////
    
  OnMessage(con_left)

  if(mStringOffset)
    --mStringOffset;

  ConsumeMessage

  //////////////////////////////////////////////////////////////////////////
  // msg: con_right
  // use: shifts the displayed text in the console to the right
  //////////////////////////////////////////////////////////////////////////
    
  OnMessage(con_right)

  if(mStringOffset < 1024)
    ++mStringOffset;
  else
    mStringOffset = 0;

  ConsumeMessage

  //////////////////////////////////////////////////////////////////////////
  // msg: con_prevcmd
  // use: sets the current line to the previous command in the command list
  //////////////////////////////////////////////////////////////////////////

  OnMessage(con_prevcmd)

  u32 buffsize = mCommandBuffer.size();

  if(buffsize == 0)
    return false;

  if( mCommandOffset < buffsize )
  {
    ++mCommandOffset;
    mCurrentLine = mCommandBuffer[buffsize - mCommandOffset];
  }

  ConsumeMessage

  //////////////////////////////////////////////////////////////////////////
  // msg: con_nextcmd
  // use: sets the current line to the next command in the command list
  //////////////////////////////////////////////////////////////////////////

  OnMessage(con_nextcmd)

  u32 buffsize = mCommandBuffer.size();

  if(buffsize == 0)
    return false;

  if( mCommandOffset )
    --mCommandOffset;

  if( mCommandOffset )
    mCurrentLine = mCommandBuffer[buffsize - mCommandOffset];
  else
    mCurrentLine = std::string("");

  ConsumeMessage

  //////////////////////////////////////////////////////////////////////////
  // msg: con_scroll
  // use: scrolls through the message buffer
  //////////////////////////////////////////////////////////////////////////

  OnMessage(con_scroll)

  s32 delta = args.GetS32("mouse_wheel_delta") / WHEEL_DELTA;

  if(mPadding) 
    return false;

  if(delta > 0)
  {
    if( mIndex >= mIndexOffset + delta )
      mIndexOffset += delta;
  }
  else if(delta < 0)
    if(mIndexOffset)
      --mIndexOffset;

  ConsumeMessage

  static std::stringstream streamcon;
  static std::string tmpstr;

  streamcon.width(8);
  streamcon.fill('0');

  streamcon << mLineNumber;
  streamcon >> tmpstr;
  tmpstr += " ";

  streamcon.clear();

  //////////////////////////////////////////////////////////////////////////
  // msg: con_print
  // use: prints the "message" string to the console
  //////////////////////////////////////////////////////////////////////////

  OnMessage(con_print)

  std::string const& str = args.GetString("message");

  if( str.size() )
  {
    if(str[str.size() - 1] == '\n')
      mBuffer.push_back(tmpstr + str);
    else
      mBuffer.push_back(tmpstr + str + '\n');

    ++mLineNumber;
  }

  ConsumeMessage

  //////////////////////////////////////////////////////////////////////////
  // msg: typewriter
  // use: gets each individual key that's pressed
  //////////////////////////////////////////////////////////////////////////

  OnMessage(typewriter)

  u8 val = args.GetU8("key");

    // Assumed invalid character
  if(val == 255 || val == '`')
    return false;

    // Carriage return
  if(val == '\r')
  {
    mStringOffset = 0;
    mCommandOffset = 0;
    mIndexOffset = 0;

    if( mCurrentLine == "mitch" )
    {
      mCommandBuffer.push_back(mCurrentLine);
      mBuffer.push_back(tmpstr + MitchQuotes[rand() % numMitchQuotes]);
      mCurrentLine.resize(0);
      ++mLineNumber;
    }
    else if( mCurrentLine == "clear" )
    {
      mCommandBuffer.push_back(mCurrentLine);
      mBuffer.resize(0);
      mCurrentLine.resize(0);
      mLineNumber = 0;
    }
    else if( mCurrentLine == "con_savefile" )
    {
      mCommandBuffer.push_back(mCurrentLine);
      SaveBufferToFile("console_dump.log");
      mCurrentLine.resize(0);
    }
    else if( mCurrentLine == "help" || mCurrentLine == "/?" )
    {
      mCommandBuffer.push_back(mCurrentLine);

      mBuffer.push_back("Virtus Engine Development Console\n");
      mBuffer.push_back("  help             - Displays this message\n");
      mBuffer.push_back("  clear            - Clears the console buffer\n");
      mBuffer.push_back("  con_savefile     - Dumps the console and command buffer to a file\n");
      mBuffer.push_back("  up/down arrow    - Cycles through previously entered commands\n");
      mBuffer.push_back("  left/right arrow - Shifts the text left/right\n");

      mLineNumber += 6;

      mCurrentLine.resize(0);
    }
    else if( mCurrentLine.size() )
    {
      mCommandBuffer.push_back(mCurrentLine);
      gCore->Events.Execute(mCurrentLine);

      mBuffer.push_back(tmpstr + mCurrentLine + '\n');
      mCurrentLine.resize(0);
      ++mLineNumber;
    }
  }

    // Backspace
  else if( val == '\b' ) 
  {
    u32 size = mCurrentLine.size();

    if(size)
      mCurrentLine.resize( size - 1 );
  }
    // Else append the character
  else
    mCurrentLine += val;

  ConsumeMessage

  return false;
}

void DevConsole::SaveBufferToFile( ch const* filename )
{
  static u32 num = 0;

  Application::FilePath fpath(filename);

  std::string file;

  if( fpath.Exists() )
  {
    Application::FilePath tmpfp(filename);

    do 
    {
      std::stringstream ss;
      ss << fpath.Name() << "_" << ++num << ".log";
      ss >> file;
      tmpfp = file;

    } while ( tmpfp.Exists() );

  }
  else
    file = filename;

  std::ofstream fp( file.c_str(), std::ofstream::out );

  if( fp.is_open() )
  {
    fp << "Virtus Engine Development Console Dump";
    fp << "\nLog File: " << file;
    fp << "\nCurrent Line: " << mCurrentLine;
    fp << "\nNewline Padding: " << mPadding;
    fp << "\nDisplayed Console Lines: " << mLinesToDraw;
    fp << "\nNumber of Lines in Buffer: " << mLineNumber;
    fp << "\nUp/Down Command Offset: " << mCommandOffset;
    fp << "\nLeft/Right Buffer Offset: " << mStringOffset;
    fp << "\n\n*** Writing Console Buffer ***\n\n";
     
    for(u32 i = 0; i < mBuffer.size(); ++i)
    {
      fp.write(mBuffer[i].c_str(), mBuffer[i].size());
    }

    fp << "\n*** Writing Command Buffer ***\n\n";

    for(u32 i = 0; i < mCommandBuffer.size(); ++i)
    {
      fp << mCommandBuffer[i] << "\n";
    }
  }

  fp.close();

  std::string str = "Saving console to file : ";
  str += file;
  str += "\n";

  mBuffer.push_back(str);
}

} // namespace Virtus
