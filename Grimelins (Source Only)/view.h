/*! ====================================================================================================================
  @file view.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.14.2011

  @brief 
    View system. Manages views connected to the game.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_VIEW_H
#define VIRTUS_VIEW_H

namespace Grimelins
{
    class ViewLayer : public ILayer
    {
    public: // autos
      ViewLayer(void);
      ViewLayer(RawStore const& startupinfo);
      virtual ~ViewLayer(void);

    public: // virtual interface
      virtual void vLoadLayer(void);
      virtual void vUnloadLayer(void);
      virtual void vUpdateLayer(void);
      virtual hstring vGetLayerID(void) const;

    private: // data
      Application::OSINSTANCE instance;
      Application::OSINSTANCE prevInstance;
      int cmdShow;
    };
} // namespace Virtus

#endif // Include Guard
