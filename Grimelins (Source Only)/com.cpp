/*! ====================================================================================================================
  @file message.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.02.2011

  @brief 
    Message routing / handline.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "com.h"
#include "stringutils.h"
#include "lock.h"


namespace Grimelins
{

    //////////////////////////////////////////////////////////////////////////
    // IListener

    IListener::IListener(void) : mMyEventHandler(NULL)
    {

    }

    IListener::~IListener(void)
    {
      if (mMyEventHandler) 
        mMyEventHandler->UnsubscribeForever(this);
    }

    //////////////////////////////////////////////////////////////////////////
    // class Msg

    EventHandler::Msg::Msg(hstring _message, MessageType _type, RawStore _data, Handle _recipient, f32 _wait, bool _lateSubOK)
      : message(_message),
        type(_type),
        data(_data),
        recipient(_recipient),
        framestamp(gCore->GetFrameCount()),
        sendFrame(framestamp + SCAST(u32, _wait*60.0f)),
        lateSubsOK(_lateSubOK)
    {

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // class MsgHandler
    
    /////////// Autos ////////////
    EventHandler::EventHandler(void)
    {

    }

    EventHandler::~EventHandler(void)
    {
      uninitialize();
    }
    

    /////////// Methods ////////////

    void EventHandler::Post(hstring const& message, RawStore const& arguments, MessageType type)
    {
//       ENTER_CORE_SECURE();
      mOutbound.push(Msg(message, type, arguments));
//       LEAVE_CORE_SECURE();
    }    

    void EventHandler::Post(hstring const& message, MessageType type)
    {
//       ENTER_CORE_SECURE();
      mOutbound.push(Msg(message, type));
//       LEAVE_CORE_SECURE();
    }

    void EventHandler::PostNow(hstring const& message, RawStore const& arguments, MessageType type)
    {
//       ENTER_CORE_SECURE();
      sendToSubscribers(Msg(message, type, arguments));
//       LEAVE_CORE_SECURE();
    }

    void EventHandler::PostNow(hstring const& message, MessageType type)
    {
//       ENTER_CORE_SECURE();
      sendToSubscribers(Msg(message, type));
//       LEAVE_CORE_SECURE();
    }

    void EventHandler::PostDelayed( hstring const& message, RawStore const& arguments, float wait, bool lateSubscribersAllowed, MessageType type )
    {
//       ENTER_CORE_SECURE();
      mOutbound.push(Msg(message, type, arguments, Handle(), wait, lateSubscribersAllowed));
//       LEAVE_CORE_SECURE();
    }

    void EventHandler::PostDelayed( hstring const& message, float wait, bool lateSubscribersAllowed, MessageType type )
    {
//       ENTER_CORE_SECURE();
      mOutbound.push(Msg(message, type, RawStore(), Handle(), wait, lateSubscribersAllowed));
//       LEAVE_CORE_SECURE();
    }

    void EventHandler::PostTo(Handle listener, hstring const& message, RawStore const& arguments, MessageType type)
    {
//       ENTER_CORE_SECURE();
      mOutbound.push(Msg(message, type, arguments, listener));
//       LEAVE_CORE_SECURE();
    }

    void EventHandler::PostTo( Handle listener, hstring const& message, MessageType type )
    {
//       ENTER_CORE_SECURE();
      mOutbound.push(Msg(message, type, RawStore(), listener));
//       LEAVE_CORE_SECURE();
    }

    void EventHandler::PostToNow(Handle listener, hstring const& message, RawStore const& arguments, MessageType type)
    {
      sendToSubscribers(Msg(message, type, arguments, listener));
    }

    void EventHandler::PostToNow( Handle listener, hstring const& message, MessageType type )
    {
      sendToSubscribers(Msg(message, type, RawStore(), listener));
    }

    void EventHandler::PostToDelayed( Handle listener, hstring const& message, RawStore const& arguments, float wait, bool lateSubscribersAllowed, MessageType type )
    {
//       ENTER_CORE_SECURE();
      mOutbound.push(Msg(message, type, arguments, listener, wait, lateSubscribersAllowed));
//       LEAVE_CORE_SECURE();
    }

    void EventHandler::PostToDelayed( Handle listener, hstring const& message, float wait, bool lateSubscribersAllowed, MessageType type )
    {
//       ENTER_CORE_SECURE();
      mOutbound.push(Msg(message, type, RawStore(), listener, wait, lateSubscribersAllowed));
//       LEAVE_CORE_SECURE();
    }



    u1 EventHandler::Request(hstring const& request, RawStore& data)
    {
      Subscriptions::iterator it = mSubscribers.begin();
      Subscriptions::iterator endIt = mSubscribers.end();

      for (; it != endIt; ++it)
      {
        Listeners::iterator it2 = it->second.begin();
        Listeners::iterator endIt2 = it->second.end();

        for (; it2 != endIt2; ++it2)
        {
          if ((*mHResources.Dereference(*it2))->handleRequest(request, data))
            return true;
        }
      }

      return false;
    }

    void EventHandler::Execute(std::string command)
    {
      mCommands.push_back(command);
    }


    void EventHandler::Subscribe(hstring const& message, IListener* subscriber)
    {
      if (!subscriber->mHListener)
      {
        ErrorIf(subscriber->mMyEventHandler, "Attempt to subscribe listener that is registered with a different handler");
        subscriber->mMyEventHandler = this;
        *mHResources.Acquire(subscriber->mHListener) = subscriber;
      }

      addSubscription(message, subscriber->mHListener);
    }

    void EventHandler::Unsubscribe(hstring const& message, IListener* subscriber)
    {
       m_delayedUnsub.push_back(std::pair<hstring, IListener*>(message, subscriber));
    }
    
    void EventHandler::UnsubscribeFromAll(IListener* subscriber)
    {

       m_delayedUnsubAll.push_back(subscriber);
    }

    void EventHandler::UnsubscribeNow( hstring const& message, IListener* subscriber )
    {
      mSubscribers[message].remove(subscriber->mHListener);
    }

    void EventHandler::UnsubscribeFromAllNow( IListener* subscriber )
    {
      rmAllSubscriptions(subscriber->mHListener);
    }

    void EventHandler::UnsubscribeForever( IListener* subscriber )
    {
      rmAllSubscriptions(subscriber->mHListener);
      mHResources.Release(subscriber->mHListener);
      subscriber->mMyEventHandler = NULL;
    }

    u1 EventHandler::IsListener(Handle hand) const 
    {
      return mHResources.IsValid(hand);
    }

    
    /////////// Protected Methods ////////////

    void EventHandler::initialize(void)
    {
      gCore->Resources.InitializeManager(mHResources);
    }

    void EventHandler::uninitialize(void)
    {
//       return;
      {
        Subscriptions::iterator it = mSubscribers.begin();
        Subscriptions::iterator endIt = mSubscribers.end();

        for (; it != endIt; ++it)
        {
          Listeners::iterator lit = it->second.begin();
          Listeners::iterator lendIt = it->second.end();

          for (; lit != lendIt; ++lit)
            if (mHResources.IsValid(*lit))
            {
              IListener* listener = *mHResources.Dereference(*lit);

              if (listener) 
                listener->mMyEventHandler = NULL;

              mHResources.Release(*lit);
            }
        }
      }

      {
        Listeners::iterator it = mListeners.begin();
        Listeners::iterator endIt = mListeners.end();
        
        for (; it != endIt; ++it)
        {
          if (mHResources.IsValid(*it))
          {
            IListener* listener = *mHResources.Dereference(*it);

            if (listener)
              listener->mMyEventHandler = NULL;

            mHResources.Release(*it);
          }
        }
      }

      gCore->Resources.UninitializeManager(mHResources);
    }

    void EventHandler::processQueue(void)
    {
      u32 frameNow = gCore->GetFrameCount();
      std::list<Msg> delayedMessages;

      while (!mOutbound.empty())
      {
        Msg out = mOutbound.top();
        mOutbound.pop();

        if(out.sendFrame <= frameNow)
          sendToSubscribers(out);
        else
          delayedMessages.push_back(out);
      }

      while(!delayedMessages.empty())
      {
        mOutbound.push(delayedMessages.front());
        delayedMessages.pop_front();
      }

      if (mCommands.empty())
        return;

      // Message command parsers
      Subscriptions::iterator it = mSubscribers.begin();
      Subscriptions::iterator endIt = mSubscribers.end();

      for (; it != endIt; ++it)
      {
        Listeners::iterator it2 = it->second.begin();
        Listeners::iterator endIt2 = it->second.end();

        for (; it2 != endIt2; ++it2)
        {
          CommandList::iterator it3 = mCommands.begin();
          CommandList::iterator endIt3 = mCommands.end();


          while (it3 != endIt3)
          {
            u1 ret = false;

            std::vector<std::string> cmds = ExplodeString(" ", *it3);
            ret = (*mHResources.Dereference(*it2))->parseCommand(H(cmds[0].c_str()), cmds);

            if (ret) // Remove command from list
            {

              if (it3 == mCommands.begin())
              {
                mCommands.erase(it3);
                it3 = mCommands.begin();
                continue;
              }
              else if (it3 == --mCommands.end())
              {
                mCommands.erase(it3);
                it3 = mCommands.end();
                continue;
              }
              else
              {
                CommandList::iterator temp = it3;
                --temp;
                mCommands.erase(it3);
                it3 = temp;
              }
            }

              // Optimization, exit this whole crazy process if all commands were dispatched
              // and processed.
              if (mCommands.empty())
                return;

              ++it3;
            }

          }
        }

      if (!mCommands.empty())
      {
        CommandList::iterator it = mCommands.begin();
        CommandList::iterator endIt = mCommands.end();

        for (; it != endIt; ++it)
        {
          RawStore arg;
          std::string msg = "ERROR: Unknown command '" + *it + "'";
          arg.AddString("message", msg);
          gCore->Events.Post("con_print", arg, MSG_PRIORITY_MEDIUM);
        }

        mCommands.clear();
      }
    }

    void EventHandler::clearQueue(void)
    {
      while (!mOutbound.empty())
        mOutbound.pop();
    }

    /////////// Private Helpers ////////////

    void EventHandler::sendToSubscribers(Msg& msg)
    {
      u1 consumable = !(msg.type & MSG_NOT_CONSUMABLE);

      if (mSubscribers.find(msg.message) == mSubscribers.end())
      {
//         printDebug("Warning: Posted message %s that has no subscribers.\n", msg.message.str());
        return; // No subscribers
      }

      Listeners::iterator it = mSubscribers[msg.message].begin();
      Listeners::iterator endIt = mSubscribers[msg.message].end();

      for (; it != endIt; ++it)
      {
        if (msg.recipient && msg.recipient != *it)
          continue;

        u1 ret = (*mHResources.Dereference(*it))->handleEvent(msg.message, msg.data, msg.type, SCAST(f32,msg.framestamp)/60.0f);

        if (ret && consumable) // consumed event
          break;
      }

      // Send to spies
      Listeners::iterator sIt = mListeners.begin();
      Listeners::iterator sEndIt = mListeners.end();

      for (; sIt != sEndIt; ++sIt)
      {
        if ((*mHResources.Dereference(*sIt))->handleEvent(msg.message, msg.data, msg.type, SCAST(f32,msg.framestamp)/60.0f)) // Event consumed?
          break;
      }
    }

    void EventHandler::addSubscription(hstring const& message, Handle subscriber)
    {
      if (message == AllMessages)
      {
        ErrorIf(std::find(mListeners.begin(), mListeners.end(), subscriber) != mListeners.end(),
          "Already listening to all messages.");

        mListeners.push_back(subscriber);
      }
      else
      {
        ErrorIf(std::find(mSubscribers[message].begin(), mSubscribers[message].end(), subscriber) != mSubscribers[message].end(),
          "Already subscribed to message. Did you subscribe multiple times?");

        mSubscribers[message].push_back(subscriber);
      }
    }

    void EventHandler::rmSubscription(hstring const& message, Handle subscriber)
    {
      if (message == AllMessages)
        mListeners.remove(subscriber);
      else
        mSubscribers[message].remove(subscriber);
    }

    void EventHandler::rmAllSubscriptions(Handle subscriber)
    {
      Subscriptions::iterator it = mSubscribers.begin();
      Subscriptions::iterator endIt = mSubscribers.end();

      for (; it != endIt; ++it)
        it->second.remove(subscriber);

      mListeners.remove(subscriber);
    }

    u32 EventHandler::numSubscriptions(Handle listener) const
    {
      u32 count = 0;
      Subscriptions::const_iterator it = mSubscribers.begin();
      Subscriptions::const_iterator endIt = mSubscribers.end();

      for (; it != endIt; ++it)
      {
        Listeners::const_iterator it2 = it->second.begin();
        Listeners::const_iterator endIt2 = it->second.end();

        for (; it2 != endIt2; ++it2)
          if (*it2 == listener)
            ++count;
      }

      return count;
    }

    //added 11/2/2011 Jonathan Fitzpatrick
    void EventHandler::processDelayedUnsubscribe( void )
    {

      DelayedMassUnsubList::iterator ait = m_delayedUnsubAll.begin();
      for(;ait!=m_delayedUnsubAll.end(); ++ait)
      {
        rmAllSubscriptions((*ait)->mHListener);
        mHResources.Release((*ait)->mHListener);
        (*ait)->mMyEventHandler = NULL;
      }
      m_delayedUnsubAll.clear();

      DelayedUnsubList::iterator it = m_delayedUnsub.begin();
      for(;it!=m_delayedUnsub.end(); ++it)
      {
        hstring const& message = it->first;
        IListener* subscriber = it->second;

        rmSubscription(message, subscriber->mHListener);

        if (numSubscriptions(subscriber->mHListener) == 0)
        {
          mHResources.Release(subscriber->mHListener);
          subscriber->mMyEventHandler = NULL;
        }
      }
      m_delayedUnsub.clear();

    }
    /////////// Message Prioritization ////////////

    u1 operator<(EventHandler::Msg const& lhs, EventHandler::Msg const& rhs)
    {
      MessageType lpr = GET_MSG_PRIORITY_BITS(lhs.type);
      MessageType rpr = GET_MSG_PRIORITY_BITS(rhs.type);

      if (lpr == rpr)
        return lhs.framestamp < rhs.framestamp;
      else
        return lpr < rpr;
    }


} // namespace Virtus
