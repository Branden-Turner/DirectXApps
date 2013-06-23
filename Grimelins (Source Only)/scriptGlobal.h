/*! ====================================================================================================================
  @file scriptGlobal.h

  Grimelins 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jan.31.2012

  @brief This is a class for global values that should be available to all scripts.
  For example, the global pointer to the player, or the boss.

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIMELINS_SCRIPTGLOBAL_H
#define GRIMELINS_SCRIPTGLOBAL_H

namespace Grimelins
{
  //fwd dec
  class Object;


  SCRIPT_HeaderDefine(ScriptGlobal);

  class ScriptGlobal : public Singleton<ScriptGlobal>
  {
  public://CTOR DTOR
    ScriptGlobal(void);
    ~ScriptGlobal(void);

  public:

    void ERR_IF(bool expr, Text const& message);

    Object* getPlayer1(void);
    void setPlayer1(Object& obj);
    void clearPlayer1(void);

    Object* getBoss(void);
    void setBoss(Object& obj);
    void clearBoss(void);
  
    bool isGamePaused(void);

    Object* getGlobal(void);
    void setGlobal(Object& g);
    void clearGlobal(void);


    float acos(float val) const;
    float cos(float val) const;
    float sin(float val) const;
    float pi(void) const;
    float randf(void) const;
    int   randi(void) const;
    float randfr(float a, float b) const;
    int   randir(int a, int b) const;

  private:
    unsigned m_boss;
    unsigned m_player1;
    unsigned m_global;

  };
}//namespace

#endif