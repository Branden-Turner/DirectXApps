/*! ====================================================================================================================
  @file devconsole.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.28.2011

  @brief
    Implementing the Development Console

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_DEVCONSOLE_H
#define VIRTUS_DEVCONSOLE_H

#include "fonts.h"

namespace Grimelins {

    class UISystem;
    class Graphics;


    class DevConsole : public IListener
    {
    public:
      DevConsole( Graphics* pnt );
      ~DevConsole(void);

      void Update( f32 dt );

      // ISubscriber interface
    private:
      virtual bool handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp);

    private:
      void SaveBufferToFile(ch const* filename);

    private:
      Graphics* mGraphics;
      std::vector<std::string> mBuffer;
      std::vector<std::string> mCommandBuffer;
      std::string mCurrentLine;

      u1 mActive;
      u1 mCursorFlip;

      f32 mDT;

      TextBox mTextBox;
      
      u32 mCommandOffset;
      u32 mStringOffset;

      u32 mIndexOffset;
      u32 mPadding;
      u32 mIndex;

      u32 mLineNumber;
      u32 mLinesToDraw;
      UISystem* mUI;

    };

} // namespace Virtus
#endif // Include Guard
