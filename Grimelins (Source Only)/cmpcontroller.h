/**********************************************************************/
/*
  
  \file
    cmpcontroller.h
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This will be the character controller for our game.  We'll attach it 
    to anything we'd like to have controlled by the player.
    
*/
/**********************************************************************/

#ifndef VIRTUS_CMPCONTROLLER_H
#define VIRTUS_CMPCONTROLLER_H

#include "component.h"

namespace Grimelins
{
    //An action is simply a vector of scripts with a name for debugging.  All of the scripts 
    //Get executed on an action when execute is called.
    struct Action
    {
      std::string name;

      std::vector<Script> scripts;
      void Execute(void);
    };

    SCRIPT_HeaderDefine(Controller);
    //Current component interface is simply commented out.
    class Controller : public Component
    {
      //The input system simply updates each controller component, and then they handle themselves
      public:
        void Update(void);
        virtual ~Controller();
        Controller();

      //Component interface
      public:
        virtual void addRegistry( void );
        virtual void remRegistry( void );
        virtual Text getTypeName(void) const;
        virtual TypeID getTypeID(void) const;
        virtual void serialize( Scope& data, bool read );
        virtual void serialize( Component const* source);
        virtual u1 handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp) {return false;};

      //Input system interaction
      friend class Input;
      friend class Factory;
      private:
        static Component* create(void);
        //These two vectors should always be the same size
        std::vector<hstring> inputActions;
        std::vector<Action>  gameLogicActions;
        u1 CheckInputAction(hstring &action);
        Text actions_;
        Text do_;

        //For debugging
        Text actionString;
        Text doString;
    };
}


#endif  VIRTUS_CMPCONTROLLER_H