/*! ====================================================================================================================
  @file userinterface.cpp

  Codename Grimelins 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Nov.23.2011

  @brief User Interface system.

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "userinterface.h"
#include "uigroup.h"
 namespace Grimelins
 {

   UserInterface*  USERINTERFACE = NULL;

   SCRIPT_ClassStart(UserInterface);
   SCRIPT_CTOR_Disabled();
   SCRIPT_Method(openBaseMenu);

   SCRIPT_Method(hide);
   SCRIPT_Method(show);
   SCRIPT_Method(open);
   SCRIPT_Method(close);

   SCRIPT_Method(hideThis);
   SCRIPT_Method(showThis);
   SCRIPT_Method(closeThis);

   SCRIPT_Method(UIOnly);
   SCRIPT_Method(nextStatic);
   SCRIPT_Method(prevStatic);
   SCRIPT_Method(resetStatic);
   SCRIPT_Method(addTimer);
   SCRIPT_ClassEnd();
   static const unsigned INVALID_CONTROL = -1;
  UserInterface::UserInterface( void )
  {

  }

  UserInterface::UserInterface( UserInterface const& rhs )
  {

  }

  UserInterface::~UserInterface( void )
  {

  }

  void UserInterface::vLoadSystem( void )
  {
    USERINTERFACE = this;
    gCore->Events.Subscribe( H("menu_next"), this );
    gCore->Events.Subscribe( H("menu_prev"), this );
    gCore->Events.Subscribe( H("menu_back"), this );
    gCore->Events.Subscribe( H("menu_pause"), this );
    gCore->Events.Subscribe( H("menu_active"), this );
    gCore->Events.Subscribe( H("LoadUI"), this );

    gCore->Events.Subscribe( H("player_win"), this );
    gCore->Events.Subscribe( H("player1_death"), this );
    gCore->Events.Subscribe( H("game_over"), this );

//     m_layers.reserve(UINUM_UILAYERS);
//     for(int i =0; i<UINUM_UILAYERS; ++i)
//     m_layers.push_back(new UILayer());
//     m_controlLayer = UIGlobal;
//     m_HighlightElement = new UIElements();
//     m_HighlightElement->enabled = false;
    m_control = "";
    m_controlIndex=m_controlPriority=INVALID_CONTROL;

    SCRIPT_RegisterGlobal(UserInterface);

    serializeMenus();
  }

  void UserInterface::vUnloadSystem( void )
  {
    for(size_t i=0; i<m_groups.size();++i)
      for(size_t j=0; j<m_groups[i].size();++j)
        delete m_groups[i][j];
  }

  void UserInterface::vUpdateSystem( void )
  {
    size_t priorities = m_groups.size()-1;

    for(size_t i=0; i<m_groups.size();++i)
      for(size_t j=0; j<m_groups[priorities-i].size();++j)
        m_groups[priorities-i][j]->update();
  }

  Grimelins::hstring UserInterface::vGetSystemID( void ) const
  {
    static const hstring id = H("UserInterface");
    return id;
  }

  Grimelins::u1 UserInterface::handleEvent( hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp )
  {

    MSG_StartHandling();
    MSG_ConsumeIf("menu_next")
    {
      //if no menu is open, ignore
      if(m_controlPriority==INVALID_CONTROL) return false;

      //call next function
      m_groups[m_controlPriority][m_controlIndex]->Next();
    }
    MSG_PeekIf("menu_prev")
    {
      //if no menu is open, ignore
      if(m_controlPriority==INVALID_CONTROL) return false;

      //call previous function
      m_groups[m_controlPriority][m_controlIndex]->Prev();
    }
    MSG_ConsumeIf("menu_pause")
    {
      if(m_controlIndex != INVALID_CONTROL && m_control != "menu_main")
        close(m_control);
      else if(gCore->GameRunning)
        open("menu_pause");
    }
    MSG_ConsumeIf("menu_back")
    {
      if(m_controlIndex != INVALID_CONTROL && m_control != "menu_main")
        close(m_control);
    }
    MSG_ConsumeIf("menu_active")
    {
      //if no menu is open, ignore
      if(m_controlPriority==INVALID_CONTROL) return false;

      //call activate function
      m_groups[m_controlPriority][m_controlIndex]->Active();
    }
    MSG_PeekIf("LoadUI")
    {
      //setup all menus
      for(size_t i=0; i<m_groups.size(); ++i)
        for(size_t j=0;j<m_groups[i].size(); ++j)
          m_groups[i][j]->setup();

    }
    MSG_ConsumeIf("player_win")
    {
      open("menu_win");
    }
    MSG_ConsumeIf("game_over")
    {
      open("menu_lose");
    }
    MSG_ConsumeIf("player1_death")
    {
      open("menu_lose");
    }
    MSG_DoneHandling();
  }

  void UserInterface::serializeMenus( void )
  {
    FileStream fs;
    fs.newStream(CONFIG_PATH_SCRIPT "_menus");
    while(fs.inScope(EOF))
    {
      Text file = fs.read<Text>();
      UIGroup* uig = new UIGroup(file);
      u32 priority = uig->Priority();
      if(m_groups.size()<(priority+1))
        m_groups.resize(priority+1);

      m_groups[priority].push_back(uig);
      m_priorities[file] = priority;
    }
  }

  void UserInterface::openBaseMenu( Text const& name )
  {
    while(m_control!="")
      close(m_control);
    if(name != "")
      open(name);
  }

  void UserInterface::hide( Text const& name )
  {
    getUIGroup(name)->Hide();
    getUIGroup(name)->m_orderedIndex = 0;
  }

  void UserInterface::show( Text const& name )
  {
    getUIGroup(name)->m_orderedIndex = 0;
    getUIGroup(name)->Show();
  }

  void UserInterface::open( Text const& name )
  {
    //if opening menu during gameplay...
    if(gCore->GameRunning && !gCore->GamePaused)
    {
      gCore->Events.PostNow("gamePause");
      gCore->GamePaused = true;
    }

    if(m_control!="")
      m_groups[m_controlPriority][m_controlIndex]->loseFocus();

    UIGroup* uig = setUIGroup(name);
    uig->m_orderedIndex = 0;
    uig->m_parent = m_control;
    m_control = name;
    uig->open();
  }

  void UserInterface::close( Text const& name )
  {
    UIGroup* uig = getUIGroup(name);
    uig->m_orderedIndex = 0;
    m_control = uig->m_parent;
    uig->m_parent = "";
    uig->close();

    if(m_control!="")
      setUIGroup(m_control)->gainFocus();
    else
    {
      m_controlIndex=m_controlPriority=INVALID_CONTROL;
      if(gCore->GameRunning)
      {
        gCore->GamePaused = false;
        gCore->Events.PostNow("gamePause");
      }
    }
  }


  void UserInterface::hideThis( void )
  {
    hide(m_control);
  }

  void UserInterface::showThis( void )
  {
    show(m_control);
  }

  void UserInterface::closeThis( void )
  {
    close(m_control);
  }

  void UserInterface::UIOnly( void )
  {
    gCore->GameRunning = false;
    gCore->GamePaused = false;
  }

  void UserInterface::nextStatic( void )
  {
    UIGroup* uig = getUIGroup(m_control);
    uig->nextOrderedStatic();
  }

  void UserInterface::prevStatic( void )
  {
    UIGroup* uig = getUIGroup(m_control);
    uig->prevOrderedStatic();
  }

  void UserInterface::addTimer( f32 seconds, Text const& fnName )
  {
    UIGroup* uig = getUIGroup(m_control);
    uig->addTimer(seconds, fnName);
  }

  UIGroup* UserInterface::setUIGroup( Text const& name)
  {
    UIGPriorityLookup::iterator it = m_priorities.find(name);
    ErrorIf(it==m_priorities.end(), "A call to set a UI group %s failed, not found.", name.c_str());
    m_controlPriority = it->second;
    UIGroupList& gl = m_groups[it->second];
    for(size_t i=0; i < gl.size(); ++i)
    {
      if(gl[i]->m_name == name)
      {
        m_controlIndex=i;
        return gl[i];
      }
    }
    ErrorIf(true, "A call to find a UI group %s failed, not found.", name.c_str());
    return NULL;

  }
  UIGroup* UserInterface::getUIGroup( Text const& name)
  {
    UIGPriorityLookup::iterator it = m_priorities.find(name);
    ErrorIf(it==m_priorities.end(), "A call to find a UI group %s failed, not found.", name.c_str());
    UIGroupList& gl = m_groups[it->second];
    for(size_t i=0; i < gl.size(); ++i)
      if(gl[i]->m_name == name)
        return gl[i];
    ErrorIf(true, "A call to find a UI group %s failed, not found.", name.c_str());
    return NULL;
  }

  void UserInterface::resetStatic( void )
  {
    UIGroup* uig = getUIGroup(m_control);
    uig->m_orderedIndex = 0;
  }

 }//namespace
 
