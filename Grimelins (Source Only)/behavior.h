/*! ====================================================================================================================
  @file behavior.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.16.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#ifndef VIRTUS_BEHAVIOR_H
#define VIRTUS_BEHAVIOR_H


namespace Grimelins
{
  typedef Text BehaviorID;
  typedef Text BehaviorFnName;
  typedef Sqrat::Function BehaviorFn;

    class Behavior
    {
      friend class Factory;
    public://ANT section
      ANT__Bar(bar);

    public:
      Behavior(Text const& file);
      ~Behavior(void);
      PROPERTY_BASIC_R(BehaviorID, id, m_id);
      u1 defines(BehaviorFnName const& fnName);
      BehaviorFn& call(BehaviorFnName const& fnName);
      BehaviorFn& call(hstring const& msgName);

      typedef std::map<hstring, BehaviorFn*> BehaviorFnMap;
      BehaviorFnMap const& getDefinedFn(void) const;

    private:
      typedef Sqrat::Script ByteCode;
      typedef std::map<BehaviorFnName, BehaviorFn*> BehaviorNameFnMap;
      void setup(void);

      //WARNING! all objects using current behaviors
      // will have their behavior pointers invaldiated!
      void recompile(void);

      Text m_script;
      ByteCode* m_bytecode;
      BehaviorID m_id;
      BehaviorFnMap m_behaviors;
      BehaviorNameFnMap m_behaviorsByName;

    };
};//Virtus namespace
#endif //VIRTUS_BEHAVIOR_H