/**********************************************************************/
/*
  
  \file
    cmpcontroller.cpp
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This will be the character controller for our game.  We'll attach it 
    to anything we'd like to have controlled by the player.
    
*/
/**********************************************************************/
#include "precompiled.h"
#include "sysinput.h"

namespace Grimelins
{
  /////// SCRIPT BINDS
  //// Controller
  SCRIPT_ClassStart(Controller);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(getTypeName);
  SCRIPT_Method(getTypeID);
  //SCRIPT specialized functions
  //none
  SCRIPT_ClassEnd();

    void Controller::Update( void )
    {
      for(u32 i = 0; i < inputActions.size(); ++i)
        if(CheckInputAction(inputActions[i]))
          gameLogicActions[i].Execute();
    }

    u1 Controller::CheckInputAction( hstring &action )
    {
      return INPUT->isActionFired(action);
    }

    void Controller::serialize( Scope& data, bool read )
    {
      SERIAL_BeginSerializing(Controller);
      SERIAL_Var(Text, "actions", actions_);
      SERIAL_Var(Text, "do", do_);

      actionString = actions_;
      doString = do_;

      //The two strings should be set up like this :
      // ACTIONS = "action1,action2,action3,action4"
      // DO      = "1,do1,1,doalso1,2,do2,2,doalso2,3,do3,4,do4"  basically "indexOfAction,doforaction,etc"

      //Grab all the input actions
      std::string token;
      while(actions_.size())
      {
        token = actions_.substr(0, actions_.find_first_of(',') );
        actions_.erase(0, token.size() + 1);
        inputActions.push_back( H(token.c_str()) );
      }

      //There are only as many game logic actions as input actions, but there could be more
      //than one script associated with a game logic action
      gameLogicActions.resize(inputActions.size());

      //Grab all the scripts
      while(do_.size())
      {
        //Grab the index
        token = do_.substr(0, do_.find_first_of(',') );
        do_.erase(0, token.size() + 1);
        int index = std::atoi(token.c_str());

        //Grab the script and push it back to the right index
        token = do_.substr(0, do_.find_first_of(',') );
        do_.erase(0, token.size() + 1);
        gameLogicActions[index].scripts.push_back( Script(token, "cmpcontroller(Serialize)") );
      }
    }

    void Controller::serialize( Component const* source )
    {

      Controller const* controller = SCAST(Controller const*, source);
      actionString = controller->actionString;
      doString = controller->doString;
      inputActions = controller->inputActions;
      gameLogicActions = controller->gameLogicActions;
    }

    void Controller::addRegistry( void )
    {
      INPUT->vRegisterComponent(getTypeID(), this);
    }

    void Controller::remRegistry( void )
    {
      INPUT->vUnregisterComponent(getTypeID(), this);
    }

    Grimelins::Text Controller::getTypeName( void ) const
    {
      return Text("Controller");
    }

    TypeID Controller::getTypeID( void ) const
    {
      return gCore->getTypeID<Controller>();
    }

    Component* Controller::create( void )
    {
      return SCAST(Component*, new Controller());
    }

    Controller::~Controller()
    {
      remRegistry();
    }

    Controller::Controller()
    {
      ANT__Bar_Init(bar, "Controller", false);
      ANT__Bar_AddButton(bar, "HIDE", ANT__Hide, ANT__PBar(bar));
      ANT__Bar_AddSeperatorAny(bar);

    }

    void Action::Execute( void )
    {
      //Iterate through all the scripts and execute them
      for(u32 i = 0; i < scripts.size(); ++i)
        scripts[i].run();
    }
}

