/*! ====================================================================================================================
  @file aabb.h

  Codename Grimelins

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date Nov.15.2011

  @brief 
    Level data storage class. When a level is loaded the files and scripts that it needs are invoked through this.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIMELINS_LEVEL_H
#define GRIMELINS_LEVEL_H

namespace Grimelins
{
  class Level
  {
  public:
    Level(Text const& name);
    ~Level(void);
    void load(void);
    void unload(void);

  private:
    void loadScript(void);
    void loadObjects(void);

  public:
    Text m_name;
    Text m_pathfile;
  private:
    Sqrat::Script* m_script;
    Sqrat::Function* m_load;
    Sqrat::Function* m_unload;
    u1 m_loaded;

  };

}//namespace
#endif // GRIMELINS_LEVEL_H