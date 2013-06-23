/*! ====================================================================================================================
  @file uicontrol.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 27.Sep.2011

  @brief 
    Base class for all ui controls

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_UICONTROL_H
#define VIRTUS_UICONTROL_H

namespace Grimelins
{


    struct UIControlRect
    {
      vec2 TopLeft;
      vec2 BottomRight;

      float Width(void) { return (BottomRight.x - TopLeft.x); }
      float Height(void) { return (TopLeft.y - BottomRight.y); }
    };
    
    class UIControl : public virtual IListener
    {
      typedef std::vector<shrdptr<UIControl>> UIControlChain;

    public: // Virtual Interface
      virtual UIControlRect GetRect(void) const = 0;
      virtual void SetRect(UIControlRect const& position) = 0;

    protected: // Virtual Interface
      virtual void vActivate(void) = 0;
      virtual void vDeactivate(void) = 0;
      virtual void vEnable(void) = 0;
      virtual void vDisable(void) = 0;

    public: // Autos
      UIControl(u1 enabled = true, u1 active = false);
      virtual ~UIControl(void);

    public: // Implemented
      void Activate(void);
      void Deactivate(void);
      void Enable(void);
      void Disable(void);
      u1 IsActive(void) const;
      u1 IsEnabled(void) const;

    public: // Control Group Chain
      template <typename T>
      void AddChildControl(T const& control)
      {
        mChildren.push_back(checked_cast<UIControl*>(new T(control)));
      }

    private:
      UIControlChain mChildren;
      u1 mEnabled;
      u1 mActive;
    };


} // namespace Virtus

#endif // include guard