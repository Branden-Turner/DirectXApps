 /*! ====================================================================================================================
   @file userinterface.h
 
   Codename Grimelins 
 
   @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
   @date  Nov.23.2011
 
   @brief UI system
  
   * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
 ===================================================================================================================== */
 #ifndef GRIMELINS_USERINTERFACE_H
 #define GRIMELINS_USERINTERFACE_H
 
 namespace Grimelins
 {
   class UIElements;
   class UIGScript;
   class UIGroup;

   SCRIPT_HeaderDefine(UserInterface);

   class UserInterface : public ISystem, public IListener
   {
   public:
     UserInterface(void);
     UserInterface(UserInterface const& rhs);
     ~UserInterface(void);

     virtual void vLoadSystem( void );
     virtual void vUnloadSystem( void );
     virtual void vUpdateSystem( void );
     virtual hstring vGetSystemID( void ) const;
     
     virtual u1 handleEvent( hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp );

     //this unwinds the current menu stack, closing each open menu
     // then loads the given menu
     void openBaseMenu(Text const& name);
     void hide(Text const& name);
     void show(Text const& name);
     void open(Text const& name);
     void close(Text const& name);

     void hideThis(void);
     void showThis(void);
     void closeThis(void);


     void UIOnly(void);
     void nextStatic(void);
     void prevStatic(void);
     void resetStatic(void);
     void addTimer(f32 seconds, Text const& fnName);

     UIGroup* setUIGroup(Text const& name);
     UIGroup* getUIGroup(Text const& name);

   private:
     void serializeMenus(void);


   private:
     typedef std::vector<UIGroup*> UIGroupList;
     typedef std::vector<UIGroupList> UIGroupPriority;
     typedef std::map<Text, u32> UIGPriorityLookup;
     UIGPriorityLookup m_priorities;
     UIGroupPriority m_groups;

     u32 m_controlPriority;
     u32 m_controlIndex;
     Text m_control;

   };

 }//namespace
 
 #endif
