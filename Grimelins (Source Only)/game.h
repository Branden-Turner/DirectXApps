/*! ====================================================================================================================
  @file gamelogic.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.14.2011

  @brief 
    Game Logic layer management

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_GAME_H
#define VIRTUS_GAME_H

#include "ilayer.h"

namespace Grimelins
{
    class GameLayer : public ILayer, public IListener
    {
    public: // autos
      GameLayer(void);
      virtual ~GameLayer(void);

    public: // virtual interface
      virtual void vLoadLayer(void);
      virtual void vUnloadLayer(void);
      virtual void vUpdateLayer(void);
      virtual hstring vGetLayerID(void) const;
      virtual u1 handleEvent(hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp);
      virtual u1 parseCommand(hstring const& command, std::vector<std::string> const& commandLine);

    };
} // namespace Virtus

#endif // Include Guard