/*! ====================================================================================================================
  @file messagerouter.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.02.2011

  @brief 
    Responsible for management of messages.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_COM_H
#define VIRTUS_COM_H

#include "rawstore.h"

namespace Grimelins
{


    /////////// Decls ////////////
    class IListener;
    class EventHandler;

    typedef u8 MessageType;

    MessageType const MSG_DEFAULT          = 0;
    MessageType const MSG_TYPE_NETWORK     = (1 << 0);
    MessageType const MSG_PRIORITY_LOW     = (1 << 1);
    MessageType const MSG_PRIORITY_MEDIUM  = (1 << 2);
    MessageType const MSG_PRIORITY_HIGH    = (1 << 3);
    MessageType const MSG_PRIORITY_TOP     = (1 << 4);
    MessageType const MSG_NOT_CONSUMABLE   = (1 << 5);
    

    #define IS_MSG_NETWORKED(type) type & MSG_TYPE_NETWORK
    #define IS_MSG_TOP_PRIORITY(type) type & MSG_PRIORITY_TOP
    #define IS_MSG_HIGH_PRIORITY(type) type & MSG_PRIORITY_HIGH
    #define IS_MSG_MEDIUM_PRIORITY(type) type & MSG_PRIORITY_MEDIUM
    #define IS_MSG_LOW_PRIORITY(type) type & MSG_PRIORITY_LOW
    #define GET_MSG_PRIORITY_BITS(type) (type >> 1) & 0xF;

    static const hstring AllMessages(H("__SUBSCRIBE_TO_ALL_MESSAGES__"));


    #define OnMessage(msg)  if (message == H(#msg)) {
    #define EndMessage return false; }
    #define ConsumeMessage return true; }

    #define MSG_StartHandling() {if(0){static const u1 autoRetVal__=false;do{}while(0)
    #define MSG_ConsumeIf(m) return autoRetVal__;} if(message == H(m)) {static const u1 autoRetVal__=true;
    #define MSG_PeekIf(m) return autoRetVal__;} if(message == H(m)) {static const u1 autoRetVal__=false;
    #define MSG_DoneHandling() return autoRetVal__;}} return false

    #define REQ_StartHandling() if(0){do{}while(0)
    #define REQ_HandleIf(r) return true;} if(request == H(r)) {
    #define REQ_DoneHandling() return true;} return false


    /////////// Event Subscription ////////////

    /// Subscription class extension: derive (virtually preferred) to receive messages.
    class IListener
    {
      friend EventHandler;
      EventHandler* mMyEventHandler;
      Handle        mHListener;

    public://Fitz changed this to public, need to access the handle of IListener objects from outside their scope
      Handle GetMyListenerHandle(void) const { return mHListener; }

    public: // Autos
      IListener(void);
      virtual ~IListener(void);
  
    public: // Interface
      virtual u1 handleEvent(hstring const& message, RawStore const& arguments, MessageType type, f32 timestamp) = 0;
      virtual u1 parseCommand(hstring const& command, std::vector<std::string> const& commandLine) { return false; }
      virtual u1 handleRequest(hstring const& request, RawStore& data) { return false; }
    };

    /////////// Message Handling / Routing ////////////
    class EventHandler
    {
      friend class Core;

    private: // Denied
      EventHandler(const EventHandler& rhs);
      EventHandler& operator=(const EventHandler& rhs);

    public: // Autos
      EventHandler(void);
      virtual ~EventHandler(void);

    public: // Methods
      //General Posting (With and Without RawStore)
      void Post(hstring const& message, RawStore const& arguments,         MessageType type = MSG_DEFAULT);
      void Post(hstring const& message,                                   MessageType type = MSG_DEFAULT);

      void PostNow(hstring const& message, RawStore const& arguments,      MessageType type = MSG_DEFAULT);
      void PostNow(hstring const& message,                                MessageType type = MSG_DEFAULT);

      void PostDelayed(hstring const& message, RawStore const& arguments,  float wait = 1.0f, bool lateSubscribersAllowed = true, MessageType type = MSG_DEFAULT);
      void PostDelayed(hstring const& message,                            float wait = 1.0f, bool lateSubscribersAllowed = true, MessageType type = MSG_DEFAULT);


      //Post To listener (With and Without RawStore)
      void PostTo(Handle listener, hstring const& message, RawStore const& arguments,        MessageType type = MSG_DEFAULT);
      void PostTo(Handle listener, hstring const& message,                                  MessageType type = MSG_DEFAULT);

      void PostToNow(Handle listener, hstring const& message, RawStore const& arguments,     MessageType type = MSG_DEFAULT);
      void PostToNow(Handle listener, hstring const& message,                               MessageType type = MSG_DEFAULT);

      void PostToDelayed(Handle listener, hstring const& message, RawStore const& arguments, float wait = 1.0f, bool lateSubscribersAllowed = true, MessageType type = MSG_DEFAULT);
      void PostToDelayed(Handle listener, hstring const& message,                           float wait = 1.0f, bool lateSubscribersAllowed = true, MessageType type = MSG_DEFAULT);

      u1 Request(hstring const& request, RawStore& data);
      void Execute(std::string command);

      void Subscribe(hstring const& message, IListener* subscriber);
      void Unsubscribe(hstring const& message, IListener* subscriber);
      void UnsubscribeNow(hstring const& message, IListener* subscriber);//CAUTION! DO NOT INVOKE DURING MESSAGE HANDLING
      void UnsubscribeFromAll(IListener* subscriber);
      void UnsubscribeFromAllNow(IListener* subscriber);
      void UnsubscribeForever(IListener* subscriber);

      u1 IsListener(Handle hand) const;
     
    protected: // For main service side (friend or child)
      void initialize(void);
      void uninitialize(void);
      void processQueue(void);
      void clearQueue(void);

    private: // Declarations.

      struct Msg
      {
        hstring message;
        MessageType type;
        RawStore data;
        Handle recipient;
        unsigned framestamp;
        unsigned sendFrame;
        bool lateSubsOK;

        Msg(hstring _message, MessageType _type, RawStore _data = RawStore(), Handle _recipient = Handle(), f32 _wait = 0.0f, bool _lateSubOK = true);
      };

      friend u1 operator<(EventHandler::Msg const& lhs, EventHandler::Msg const& rhs);
  
      typedef std::priority_queue<Msg> MessageList;
      typedef std::list<Handle> Listeners;
      typedef std::map<hstring, Listeners> Subscriptions;
      typedef std::list<std::string> CommandList;
      typedef std::list<IListener*> DelayedMassUnsubList;
      typedef std::list<std::pair<hstring, IListener*>> DelayedUnsubList;

    private: // Helpers.
      void sendToSubscribers(Msg& msg);
      void addSubscription(hstring const& message, Handle subscriber);
      void rmSubscription(hstring const& message, Handle subscriber);
      void rmAllSubscriptions(Handle subscriber);
      u32  numSubscriptions(Handle listener) const;
      void processDelayedUnsubscribe(void);

    private: // Data
      ResourceManager<IListener*> mHResources;
      MessageList                 mOutbound;
      Subscriptions               mSubscribers;
      Listeners                   mListeners;
      CommandList                 mCommands;
      DelayedMassUnsubList        m_delayedUnsubAll;
      DelayedUnsubList            m_delayedUnsub;
    };

    /////////// Message Prioritization ////////////

} // namespace Virtus

#endif

