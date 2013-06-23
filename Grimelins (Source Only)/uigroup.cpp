/*! ====================================================================================================================
  @file uigroup.cpp

  Codename Grimelins 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Nov.23.2011

  @brief Specifies a UI Grouping. That is, a set of related buttons and images, aka a menu.

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "uigroup.h"
#include "uisystem.h"
#include "graphics.h"
#include "dx9vertextype.h"
#include "dx9device.h"
#include "dx9buffers.h"
#include "dx9shaders.h"
#include "dx9resourcemanager.h"
#include "syssound.h"

 namespace Grimelins
 {
//---------------------------------------------
// UIG Script
//---------------------------------------------

   UIGScript::UIGScript( void )
     : script(NULL), file(""), tabStart("")
   {
   }

  UIGScript::~UIGScript( void )
   {
     if(script) delete script;
     menuFunctionMap::iterator it = functions.begin();
     for(;it!=functions.end(); ++it)
       delete it->second;
   }

   void UIGScript::serialize( Text const& file )
   {
     script = new Sqrat::Script(gCore->Scripter);
     try { script->CompileFile(file); }
     catch (Sqrat::Exception e) { printDebug("Error compiling file '%s' in squirrel virtual machine.\n", file.c_str()); return;}

     ErrorIf(!script->IsCompiled(), "Script Failed to compile '%s'\n\nCheck the filepath and filename.\n If you are sure it's right, check Squirrel syntax in the file.\n", file.c_str());    
     if(!script->IsCompiled())
     {
       delete script;
       script = NULL;
       return;
     }
     
     addFunction("setup", false);
     addFunction("gainFocus", false);
     addFunction("loseFocus", false);
     addFunction("priority", false);
     addFunction("open", false);
     addFunction("close", false);


   }

   void UIGScript::addFunction( CText fName, bool tabStopped)
   {
     //already exists
     if(functions.find(fName) != functions.end())
       return;

     //attempt to obtain function pointer
     try { script->Run(); }
     catch (Sqrat::Exception e) { printDebug("Error in squirrel virtual machine running script '%s'.\n", file.c_str()); return;}
     Sqrat::Function* fn = getFunction(fName);
     gCore->Scripter.runString("menu.clear();");

     //function not found
     if(fn == NULL)
       return;

     functions[fName] = fn;

      if(tabStopped)
      {
        if(tabbedFunctions.empty())
          tabStart = fName;
        tabbedFunctions.push_back(fName);
      }
  }

   Sqrat::Function* UIGScript::getFunction( CText fName )
   {
     Sqrat::Function* fn = new Sqrat::Function(Sqrat::RootTable(gCore->Scripter).GetSlot("menu"), fName);
     ErrorIf(!fn, "Script Object failed to allocate heap memory for a function");
     if(fn->IsNull())
     {
       delete fn;
       fn = NULL;
     }
     return fn;
   }

//---------------------------------------------
// UI Group
//---------------------------------------------

   SCRIPT_ClassStart(UIGroup);
   SCRIPT_CTOR_Disabled();
   SCRIPT_Method(addDynamic);
   SCRIPT_Method(addStatic);
   SCRIPT_Method(addTimer);
   SCRIPT_Method(addOrderedStatic);
   SCRIPT_Method(nextOrderedStatic);
   SCRIPT_ClassEnd();

  UIGroup::UIGroup( Text const& name )
  :m_name(name), m_visible(false), m_script(), m_curElement(""), m_isSetup(false)
  {
    m_script.serialize(CONFIG_PATH_SCRIPT + name + ".nut");
  }

  UIGroup::~UIGroup( void )
  {

  }

  void UIGroup::update( void )
  {
    if(m_visible)
    {
      static const size_t EMPTY_ELEMENT_LIST = -1;
      size_t iActive=EMPTY_ELEMENT_LIST;
      for(size_t i=0; i<m_elements.size(); ++i)
      {
        if(m_curElement==m_elements[i]->fnName)
        {
          m_elements[i]->tex = m_elements[i]->tex2;
          iActive=i;
        }
        else
        {
          m_elements[i]->tex = m_elements[i]->tex1;
          //draw inactive items first
          GRAPHICS->mUISystem->AddUIElement(*m_elements[i]);
        }

      }
      //then draw active element
      if(iActive!=EMPTY_ELEMENT_LIST) 
        GRAPHICS->mUISystem->AddUIElement(*m_elements[iActive]);


      //update timers
      size_t countTo = m_timers.size();
      for(size_t i = 0; i < m_timers.size(); ++i)
      {
        if( (m_timers[i].start + m_timers[i].time.Get()) > m_timers[i].duration )
        {
          m_script.functions[m_timers[i].func]->Execute();
          m_timers.erase(m_timers.begin() + i);
          --i;
          if(countTo != m_timers.size())
            break;
        }
      }
    }
  }

  void UIGroup::setup( void )
  {
    if(!m_isSetup)
    {
      m_script.functions["setup"]->Execute(this);
      m_isSetup = true;
      m_curElement = m_script.tabStart;
    }
  }

  void UIGroup::Active( void )
  {
    ErrorIf(!m_visible, "Activate function called an element which is on a menu marked hidden.");
    call(m_curElement);

    // WTF, why twice? [2/17/2012 Maximilian Manndorff]
    SOUND->PostGlobalEvent("play_menu_active");
    SOUND->PostGlobalEvent("play_menu_active");
  }

  void UIGroup::Next( void )
  {
    ErrorIf(!m_visible, "Next called on a hidden menu group.");

    if(m_script.tabbedFunctions.empty())
      return;
    ErrorIf(m_curElement=="", "cant call next element when no current element is set.");

    UIGScript::FuncNames::iterator it = m_script.tabbedFunctions.begin();
    while(*it != m_curElement)
    {
      ++it;
      ErrorIf(it==m_script.tabbedFunctions.end(), "next reached end of elements without finding the current element.");
    }
    ++it;
    if(it==m_script.tabbedFunctions.end())
      it = m_script.tabbedFunctions.begin();

    m_curElement = *it;

    SOUND->PostGlobalEvent("play_menu_active");
  }

  void UIGroup::Prev( void )
  {
    ErrorIf(!m_visible, "Prev called on a hidden menu group.");

    if(m_script.tabbedFunctions.empty())
      return;
    ErrorIf(m_curElement=="", "cant call prev element when no current element is set.");

    UIGScript::FuncNames::reverse_iterator it = m_script.tabbedFunctions.rbegin();
    while(*it != m_curElement)
    {
      ++it;
      ErrorIf(it==m_script.tabbedFunctions.rend(), "next reached end of elements without finding the current element.");
    }
    ++it;
    if(it==m_script.tabbedFunctions.rend())
      it = m_script.tabbedFunctions.rbegin();

    m_curElement = *it;

    SOUND->PostGlobalEvent("play_menu_active");
  }

  void UIGroup::Show( void )
  {
    m_visible = true;
  }

  void UIGroup::Hide( void )
  {
    m_visible = false;
  }

  void UIGroup::gainFocus( void )
  {
    call("gainFocus");
    Show();
  }

  void UIGroup::loseFocus( void )
  {
//     Hide();
    call("loseFocus");
  }

  void UIGroup::open( void )
  {
    setup();
    call("open");
    gainFocus();
  }

  void UIGroup::close( void )
  {
    m_curElement = m_script.tabStart;
    Hide();
    loseFocus();
    call("close");
  }



  u32 UIGroup::Priority( void )
  {
    return m_script.functions["priority"]->Evaluate<u32>();
  }

  void UIGroup::addStatic( Text const& image, f32 pX, f32 pY, f32 sX, f32 sY, f32 Alpha )
  {
    UIElement* uie = new UIElement();
    uie->fnName = "none";
    uie->alpha = Alpha;
    uie->dim.x = sX;
    uie->dim.y = sY;
    uie->pos.x = pX;
    uie->pos.y = pY;
    uie->tex = uie->tex1 = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE + image);
    uie->tex2 = NULL;
    m_elements.push_back(uie);
  }

  void UIGroup::addDynamic( Text const& image, Text const& image2,  Text const& fnName, f32 pX, f32 pY, f32 sX, f32 sY, f32 Alpha )
  {
    ErrorIf(fnName=="", "Can't have a dynamic menu element with no assigned function");
    UIElement* uie = new UIElement();
    uie->fnName = fnName;
    uie->alpha = Alpha;
    uie->dim.x = sX;
    uie->dim.y = sY;
    uie->pos.x = pX;
    uie->pos.y = pY;
    uie->tex = uie->tex1 = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE + image);
    if(image2!="")
      uie->tex2 = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE + image2);

    m_elements.push_back(uie);
    m_script.addFunction(fnName.c_str());
  }

  void UIGroup::addTimer( f32 duration, Text const& fnName )
  {
    m_script.addFunction(fnName.c_str(), false);
    m_timers.push_back(TimePackage(duration, fnName));
  }

  void UIGroup::addOrderedStatic( Text const& image, f32 pX, f32 pY, f32 sX, f32 sY, f32 Alpha )
  {
    UIElement* uie = new UIElement();
    uie->fnName = "none";
    uie->alpha = Alpha;
    uie->dim.x = sX;
    uie->dim.y = sY;
    uie->pos.x = pX;
    uie->pos.y = pY;
    uie->tex = uie->tex1 = GRAPHICS->mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE + image);
    uie->tex2 = NULL;
    //first ordered static?
    if(m_orderedStatics.empty())
    {
      m_elementIndex = m_elements.size();
      m_orderedIndex = 0;
      m_elements.push_back(uie);
    }
      m_orderedStatics.push_back(uie);
  }

  void UIGroup::nextOrderedStatic( void )
  {
    ErrorIf(m_orderedStatics.empty(), "No ordered static elements exist to cycle");
    ++m_orderedIndex;
    if(m_orderedIndex == m_orderedStatics.size())
      m_orderedIndex = 0;
    m_elements[m_elementIndex] = m_orderedStatics[m_orderedIndex];
  }

  void UIGroup::prevOrderedStatic( void )
  {
    ErrorIf(m_orderedStatics.empty(), "No ordered static elements exist to cycle");
    if(m_orderedIndex == 0)
      m_orderedIndex = m_orderedStatics.size()-1;
    else
      --m_orderedIndex;
    m_elements[m_elementIndex] = m_orderedStatics[m_orderedIndex];
  }

  void UIGroup::call( Text const& fName )
  {
    UIGScript::menuFunctionMap::iterator it = m_script.functions.find(fName);
    if(it!=m_script.functions.end())
      it->second->Execute();
  }

 }//namespace
 
