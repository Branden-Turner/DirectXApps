/*! ====================================================================================================================
  @file ant_tweak.h

  Codename Grimelins 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Nov.4.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef GRIMELINS_ANT_TWEAK_H
#define GRIMELINS_ANT_TWEAK_H

#ifdef _DEBUG
  #define ANT_TWEAK_ENABLED //comment out to remove ANT TWEAK BAR
#endif

/// DO NOT MOVE ///
#ifndef ANT_TWEAK_ENABLED
#define ANT__RETVAL void
#else
#include "AntTweakBar.h"
#pragma comment(lib, "AntTweakBar.lib")
#define ANT__RETVAL void TW_CALL
#endif
/// END DO NOT MOVE ///


namespace Grimelins{
//Place ANT callback, getter and setter method prototypes here for global ANT callbacks.
//You can define and implement these locally in your .cpps if they are very type specific.
//use this file only for very general functionality callbacks.

  ANT__RETVAL ANT__Hide(void* bar);
  ANT__RETVAL ANT__Show(void* bar);
  ANT__RETVAL ANT__Toggle(void* bar);

  //These are not called directly, registered during ant init
  ANT__RETVAL ANT__CopyStdStringToClient(std::string& s1, const std::string&s2);
  ANT__RETVAL ANT__SetMyStdStringCB(const void *value, void * userdata);
  ANT__RETVAL ANT__GetMyStdStringCB(void *value, void * userdata);

}//namespace
#ifndef ANT_TWEAK_ENABLED
////////////////////////////////////////////////////////
// INTERFACE WHICH REPLACES ANT CALLS WHEN NOT IN USE //
////////////////////////////////////////////////////////
namespace Grimelins{

#define ANT__Initialize(a)            (void)(0)
#define ANT__WindowSize(a,b)          (void)(0)
#define ANT__Draw()                   (void)(0)
#define ANT__Event_Handle(a,b,c,d)    (void)(0)
#define ANT__KeyPressed(a,b)          (void)(0)
#define ANT__MouseButton(a,b)         (void)(0)
#define ANT__MouseMove(a,b)           (void)(0)
#define ANT__MouseWheel(a)            (void)(0)
#define ANT__Terminate()              (void)(0)
#define ANT__CVAR_DECL()              
#define ANT__CVAR                     (void)(0)

  typedef void ANT_Bar;
  typedef void (* ANT__SetCB)(const void*, void*);
  typedef void (* ANT__GetCB)(void*, void*);
  typedef void (* ANT__CB)(void*);

#define ANT__LISTEN(who)                          (void)(0)
#define ANT__MSG_HANDLE()                         return autoRetVal__;} if(0) {static const u1 autoRetVal__=false;
#define ANT__Define(b, d)                         (void)(0)
#define ANT__GetBar(b,from)                       NULL
#define ANT__GetBarP(bar, Pfrom)                  NULL
#define ANT__PBar(b)                              NULL
#define ANT__SetTop(b)                            (void)(0)
#define ANT__Bar(b)
#define ANT__Bar_Init(b, n, d)                    (void)(0)
#define ANT__Bar_Deinit(b)                        (void)(0)
#define ANT__Bar_Remove(b, n)                     (void)(0)
#define ANT__Bar_Show(b)                          (void)(0)
#define ANT__Bar_Hide(b)                          (void)(0)
#define ANT__Bar_Toggle(b)                        (void)(0)
#define ANT__Bar_AddButton(b, n, f, d)            (void)(0)
#define ANT__Bar_AddButton_P(b, n, f, d, p)       (void)(0)
#define ANT__Bar_AddVarCB(b, n, s, g, v, T)       (void)(0)
#define ANT__Bar_AddVarCB_P(b, n, S,G, v, T, p)   (void)(0)
#define ANT__Bar_AddVarRO(b, n, v, T)             (void)(0)
#define ANT__Bar_AddVarRO_P(b, n, v, T, p)        (void)(0)
#define ANT__Bar_AddVarRW(b, n, v, T)             (void)(0)
#define ANT__Bar_AddVarRW_P(b, n, v, T, p)        (void)(0)
#define ANT__Bar_AddVarRW_P_Oride(b, n, v, T, p)  (void)(0)
#define ANT__Bar_AddSeperator(b, n)               (void)(0)
#define ANT__Bar_AddSeperatorAny(b)               (void)(0)
#define ANT__Bar_AddSeperator_P(b, n, p)          (void)(0)
#define ANT__Bar_AddSeperatorAny_P(b, p)          (void)(0)



}//grimelins


#else
namespace Grimelins{


  /////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////                            ///////////////////////////////////////////
  ////////////////////// A N T    I N T E R F A C E ///////////////////////////////////////////
  //////////////////////                            ///////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////
  #define ANT__Initialize(d_)         TwInit(TW_DIRECT3D9, (void*)d_);                                  \
                                      TwCopyStdStringToClientFunc(ANT__CopyStdStringToClient);          \
                                      gCore->Events.Post(H("ANTReady"), MSG_DEFAULT|MSG_NOT_CONSUMABLE)

  #define ANT__WindowSize(w_,h_)      TwWindowSize(w_, h_)
  #define ANT__CVAR                   cvANT
  #define ANT__Draw()                 if(ANT__CVAR) TwDraw()
  #define ANT__Event_Handle(h,m,w,l)  if(TwEventWin(h,m,w,l))return 0;
  #define ANT__KeyPressed(k_,m_)      TwKeyPressed(k_, m_)
  #define ANT__MouseButton(a_,b_)     TwMouseButton(a_, b_)
  #define ANT__MouseMove(x_,y_)       TwMouseMotion(x_, y_)
  #define ANT__MouseWheel(p_)         TwMouseWheel(p_)
  #define ANT__Terminate()            TwTerminate()
  #define ANT__CVAR_DECL()            cvar ANT__CVAR("ant", "Toggle display of ANT tweak-bar interfaces.", true)

  /////////////////////////////////////////////////////////////////////////////////
  //                                                                             //
  //    BUTTON CALLBACKS   -OR-  VARIABLE ACCESS VIA CALLBACK GET/SET            //
  //                                                                             //
  /////////////////////////////////////////////////////////////////////////////////
  /* To create callback functions create a C-style functions as described below:
      For a Button: ANT__RETVAL YourFnName(void* variable);
      For a SetCB: ANT__RETVAL yourFnName(const void* fromANT, void* userdata);
      For a getCB: ANT__RETVAL yourFnName(void* toANT, void* userdata);

    NOTE: callbacks take both functions. For read-only specify NULL for the setter
  */
  typedef void (TW_CALL * ANT__CB)(void* userdata);
  typedef void (TW_CALL * ANT__SetCB)(const void* fromANT, void* userdata);
  typedef void (TW_CALL * ANT__GetCB)(void* toANT, void* userdata);

  /////////////////////////////////////////////////////////////////////////////////
  //                                                                             //
  //    Creating ANT tweak bar data members, defining, and toggling features     //
  //                                                                             //
  /////////////////////////////////////////////////////////////////////////////////
  /* 
    + CREATE A BAR: call ANT__Bar(x) in your class definition as though it was a
        class data-member. (x) is the variable name for the new bar. THIS DOES NOT
        BEHAVE LIKE A FUNCTION! It behaves like variable declaration.

    + SETUP A BAR: call ANT__Bar_Init(x,y) to give a bar a name. Called just like
        a function. It only has to be called once. (x) is same as in ANT_Bar(x), 
        (y) is the DISPLAY name

    + ADD A BUTTON: use ANT__Bar_AddButton
    + ADD A VARIABLE: Depending on your needs:
        Variables that require GET/SET methods: ANT__Bar_AddVarCB
        Variables that are direct access: ANT__Bar_AddVarRO or ANT__Bar_AddVarRW
    + ADD A SEPERATOR: use ANT__Bar_AddSeperator
    + REMOVE AN ELEMENT FROM A BAR: use ANT__Bar_Remove

    NOTE: There are _P variations on all ADD functions. These allow you to specify
    a C-String formatted as described at http://www.antisphere.com/Wiki/tools:anttweakbar:varparamsyntax
    look to the example arguments passed to TwDefine. REMEMBER! The bar name and
    element name are inputted for you (like "mybar/WindVel" in the URL aboves examples)
  */
#define ANT__LISTEN(who)                          gCore->Events.Subscribe( H("ANTReady"), who );
#define ANT__MSG_HANDLE()                         return autoRetVal__;} if(message == H("ANTReady")) {static const u1 autoRetVal__=false;
#define ANT__GetBar(b, f)                         f.b
#define ANT__GetBarP(b, pf)                       pf->b
#define ANT__PBar(b)                              &b
#define ANT__SetTop(b)                            b.setTop()
#define ANT__Define(b, d)                         b.define(d)
  /*************************************************************************************************************
  | ** DECLARE A BAR IN YOUR CLASS **
  | Description: Declares bar b that will exist with your class instances. This must be unique within your class.
  | The bar must be initialized (once) in your cpp, use ANT__Bar_Init for this.
  | b - bar; Unique name among other bars in the parent class.
  \*************************************************************************************************************/
  #define ANT__Bar(b)                               friend class ANT_Bar; mutable ANT_Bar b

  /*************************************************************************************************************
  | ** INITIALIZE A BAR **
  | Description: Actually creates the bar, giving it name n.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String name for the bar to display as.
  | d - default visible; Boolean value of wether this bar shows(true) by default, or hides (false)
  \*************************************************************************************************************/
  #define ANT__Bar_Init(b, n, d)                    b.initialize(n,d)
  #define ANT__Bar_Deinit(b)                        b.deinitialize()
  /*************************************************************************************************************
  | ** REMOVE AN ELEMENT FROM A BAR **
  | Description: Deletes an element n from bar b.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String name of the element to delete.
  \*************************************************************************************************************/
  #define ANT__Bar_Remove(b, n)                     b.remove(n)

  /*************************************************************************************************************
  | ** SHOW BAR **
  | Description: Causes a bar to display
  | b - bar; Unique name among other bars in the parent class.
  \*************************************************************************************************************/
  #define ANT__Bar_Show(b)                          b.show()

  /*************************************************************************************************************
  | ** HIDE BAR **
  | Description: Causes a bar to hide (not display).
  | b - bar; Unique name among other bars in the parent class.
  \*************************************************************************************************************/
  #define ANT__Bar_Hide(b)                          b.hide()

  /*************************************************************************************************************
  | ** SHOW or HIDE BAR **
  | Description: Causes a bar to toggle visibility. (hidden or shown).
  | b - bar; Unique name among other bars in the parent class.
  \*************************************************************************************************************/
  #define ANT__Bar_Toggle(b)                        b.toggle()


  /*************************************************************************************************************
  | ** ADD SEPERATOR TO BAR **
  | Description: Creates a separation "bar" across bar b named n.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String unique element name among elements on bar b for this seperator.
  \*************************************************************************************************************/
#define ANT__Bar_AddSeperator(b, n)               b.addSeperator(n)
#define ANT__Bar_AddSeperatorAny(b)               b.addSeperator("")

  /*************************************************************************************************************
  | ** ADD SEPERATOR TO BAR, WITH PROPERTIES **
  | Description: Creates a separation "bar" across bar b named n. Seperator further defined by p.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String unique element name among elements on bar b for this seperator.
  | p - properties; C string formatted according to ANT documentation for properties. Note b and n are supplied.
  \*************************************************************************************************************/
  #define ANT__Bar_AddSeperator_P(b, n, p)             b.addSeperator(n,p)
  #define ANT__Bar_AddSeperatorAny_P(b, p)             b.addSeperator("",p)

  /*************************************************************************************************************
  | ** ADD BUTTON TO BAR **
  | Description: Creates n that calls f & passes it a when clicked.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String unique element name among elements on bar b for this button.
  | f - function; An ANT__CB type function pointer to call when the button is pressed.
  \*************************************************************************************************************/
  #define ANT__Bar_AddButton(b, n, f, a)            b.addButton(n, f, a)

  /*************************************************************************************************************
  | ** ADD BUTTON TO BAR, WITH PROPERTIES **
  | Description: Creates n that calls f & passes it a when clicked. Button is further defined by p.
  | b - bar; C String unique name among other bars in the parent class.
  | n - name; Unique element name among elements on bar b for this button.
  | f - function; An ANT__CB type function pointer to call when the button is pressed.
  | p - properties; C string formatted according to ANT documentation for properties. Note b and n are supplied.
  \*************************************************************************************************************/
  #define ANT__Bar_AddButton_P(b, n, f, a, p)       b.addButton(n, f, a, p)

  /*************************************************************************************************************
  | ** ADD VARIABLE TO BAR, ACCESSED WITH GET/SET FUNCTION **
  | Description: Creates n that calls g to get, s to set variable of type T. 'a' is passed to these functions.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String unique element name among elements on bar b for this callback accessed variable.
  | s - set; An ANT__SetCB type function pointer to call when v is modifed by ANT. (NULL indicates read-only)
  | g - get; An ANT__GetCB type function pointer to call when v is read by ANT.
  | a - address; A void* to some data you wish to pass as userdata to the get and set method.
  | T - type; The datatype of variable ANT will expect to pass, and retrieve from this get/set function.
  \*************************************************************************************************************/
  #define ANT__Bar_AddVarCB(b, n, s, g, a, T)       b.addVarCB<T>(n, s, g, a)

  /*************************************************************************************************************
  | ** ADD VARIABLE TO BAR, ACCESSED WITH GET/SET FUNCTION, WITH PROPERTIES **
  | Description: Creates n that calls g to get, s to set variable of type T. 'a' is passed to these functions.
  | further defined by p.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String unique element name among elements on bar b for this callback accessed variable.
  | s - set; An ANT__SetCB type function pointer to call when v is modifed by ANT. (NULL indicates read-only)
  | g - get; An ANT__GetCB type function pointer to call when v is read by ANT.
  | a - address; A void* to some data you wish to pass as userdata to the get and set method.
  | T - type; The datatype of variable ANT will expect to pass, and retrieve from this get/set function.
  | p - properties; C string formatted according to ANT documentation for properties. Note b and n are supplied.
  \*************************************************************************************************************/
  #define ANT__Bar_AddVarCB_P(b, n, s, g, a, T, p)  b.addVarCB<T>(n, s, g, a, p)

  /*************************************************************************************************************
  | ** ADD READ-ONLY VARIABLE TO BAR **
  | Description: Creates n that reads v of type T.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String unique element name among elements on bar b for this callback accessed variable.
  | v - variable; The variable to be accessed.
  | T - type; The datatype of variable ANT will expect v to be.
  \*************************************************************************************************************/
  #define ANT__Bar_AddVarRO(b, n, v, T)             b.addVarRO<T>(n, v)

  /*************************************************************************************************************
  | ** ADD READ-ONLY VARIABLE TO BAR, WITH PROPERTIES **
  | Description: Creates n that reads v of type T. Further defined by p.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String unique element name among elements on bar b for this callback accessed variable.
  | v - variable; The variable to be accessed.
  | T - type; The datatype of variable ANT will expect v to be.
  | p - properties; C string formatted according to ANT documentation for properties. Note b and n are supplied.
  \*************************************************************************************************************/
  #define ANT__Bar_AddVarRO_P(b, n, v, T, p)        b.addVarRO<T>(n, v, p)

  /*************************************************************************************************************
  | ** ADD READ-WRITE VARIABLE TO BAR **
  | Description: Creates n that reads/writes v of type T.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String unique element name among elements on bar b for this callback accessed variable.
  | v - variable; The variable to be accessed/mutated.
  | T - type; The datatype of variable ANT will expect v to be.
  \*************************************************************************************************************/
  #define ANT__Bar_AddVarRW(b, n, v, T)             b.addVarRW<T>(n, v)

  /*************************************************************************************************************
  | ** ADD READ-WRITE VARIABLE TO BAR **
  | Description: Creates n that reads/writes v of type T.
  | b - bar; Unique name among other bars in the parent class.
  | n - name; C String unique element name among elements on bar b for this callback accessed variable.
  | v - variable; The variable to be accessed/mutated.
  | T - type; The datatype of variable ANT will expect v to be.
  | p - properties; C string formatted according to ANT documentation for properties. Note b and n are supplied.
  \*************************************************************************************************************/
  #define ANT__Bar_AddVarRW_P(b, n, v, T, p)        b.addVarRW<T>(n, v, p)
  #define ANT__Bar_AddVarRW_P_Oride(b, n, v, T, p)        b.addVarRW<signed short>(n, v, p, T)




  /////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////                            ///////////////////////////////////////////
  //////////////////////  BEHIND THE CURTAINS...    ///////////////////////////////////////////
  //////////////////////                            ///////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////
  class ANT_Bar
  {
  public:
    //CTOR/DTOR
    ANT_Bar(void): bar(NULL), initializedBefore(false){};
    ~ANT_Bar(void) {if(bar) TwDeleteBar(bar);  };

    void deinitialize(void)  
    { 
      if(bar) 
      {
        TwGetParam(bar, NULL, "position", TW_PARAM_INT32, 2, pos);
        TwDeleteBar(bar); 
        bar = NULL; 
      } 
    };

    //CTOR as a Method (initializer)
    void initialize(const char* name, bool show=false)  
    { 
      memset(barName,NULL,40);
      strcpy(barName, name);

      bar = TwNewBar(barName);
//       {const char* err = TwGetLastError(); assert(err =d= NULL);}

      if(!bar) return;
      std::stringstream ss;
      ss << " '" << barName << "' color='60 60 60' alpha=180 text=light valueswidth=100 size='325 500' movable=true resizable=true fontresizable=true alwaystop=false alwaysbottom=false contained=true";
      TwDefine(ss.str().c_str());
      //{const char* err = TwGetLastError(); assert(err == NULL);}

      std::string st = " '";
      st += barName;
      st += "' visible=true";
      st = st.substr(0,100);

      std::string sf = " '";
      sf += barName;
      sf += "' visible=false";
      sf = sf.substr(0,100);

      memset(visibleTrue,NULL,100);
      memset(visibleFalse,NULL,100);
      strcpy(visibleTrue, st.c_str());
      strcpy(visibleFalse, sf.c_str());
      shown = (initializedBefore? shown:show);
      if(!shown) hide();
      if(initializedBefore)
      {
        std::stringstream s;
        s << " '" << barName;
        s << "' position='" << pos[0] << " " << pos[1] << "'";
        TwDefine(s.str().c_str());
      }
      initializedBefore = true;
    };

    const char* composeProperties(const char* name, const char* properties) 
    { 
      //default property scheme
      if(*properties == '\0')
        return NULL;

      //custom property scheme
      std::string p = " ";
      p += barName;
      p += "/";
      p += name;
      p += " ";
      p += properties;
      return p.c_str();
    };

    void show(void) 
    { if(bar) {TwDefine(visibleTrue); shown=true;}  };

    void setTop(void)
    {
      if(bar && shown) TwSetTopBar(bar);
    }

    void define(const char* def)
    {
      if(bar) 
      {
        std::string p = " '";
        p += barName;
        p += "'";
        p += def;
        TwDefine(p.c_str());
        //{const char* err = TwGetLastError(); assert(err == NULL);}

      }
    }

    void hide(void) 
      { if(bar) {TwDefine(visibleFalse); shown=false;} };

    void toggle(void) 
    { if(bar) if(shown){TwDefine(visibleFalse); shown=!shown;}else{TwDefine(visibleTrue);shown=!shown;}  };

    void remove(const char* name) 
      { if(bar) TwRemoveVar(bar, name); };

    void addButton(const char* name, ANT__CB callback, void* data=NULL, const char* properties="") 
    { if(bar) TwAddButton(bar, name, callback, data, properties );}; //{const char* err = TwGetLastError(); assert(err == NULL);} };

    void addSeperator(const char* name, const char* properties="") 
    { if(bar) TwAddSeparator(bar, (name==""?getUniqueName().c_str():name), properties );}; //{const char* err = TwGetLastError(); assert(err == NULL);} };

    //addVarCB addVarRO addVarRW:
    template<typename T>
    void addVarCB(const char* name, ANT__SetCB setcall=NULL, ANT__GetCB getcall=NULL, void* data=NULL, const char* properties="", TwType t=TW_TYPE_INT32)
    {
      if(bar) TwAddVarCB(bar, name, t, setcall, getcall, data, properties );
      //{const char* err = TwGetLastError(); assert(err == NULL);}
    };
    template<typename T>
    void addVarRO(const char* name, const void* data=NULL, const char* properties="", TwType t=TW_TYPE_INT32)
    {
      if(bar) TwAddVarRO(bar, name, t, data, properties );
      //{const char* err = TwGetLastError(); assert(err == NULL);}
    };
    template<typename T>
    void addVarRW(const char* name, void* data=NULL, const char* properties="", TwType t=TW_TYPE_INT32)
    {
      if(bar) TwAddVarRW(bar, name, t, data, properties );
      //{const char* err = TwGetLastError(); assert(err == NULL);}
    };

    //Implements the addVarCB, addVarRO, addVarRW methods based on type (pairing c++ type with ant enum)
#define ANT_INTERNAL_USE_(typ, twt)                                                                                   \
    template<>                                                                                                        \
    void addVarCB<typ>(const char*name, ANT__SetCB setcall, ANT__GetCB getcall, void*data, const char*properties, TwType t)\
    {                                                                                                                 \
       if(bar) TwAddVarCB(bar, name, twt, setcall, getcall, data, properties );                                       \
       /*{const char* err = TwGetLastError(); assert(err == NULL);}*/                                                                  \
    };                                                                                                                \
    template<>                                                                                                        \
    void addVarRO<typ>(const char* name, const void* data, const char* properties, TwType t)                          \
    {                                                                                                                 \
      if(bar) TwAddVarRO(bar, name, twt, data, properties );                                                          \
       /*{const char* err = TwGetLastError(); assert(err == NULL);} */                                                                   \
    };                                                                                                                \
    template<>                                                                                                        \
    void addVarRW<typ>(const char* name, void* data, const char* properties, TwType t)                                \
    {                                                                                                                 \
      if(bar) TwAddVarRW(bar, name, twt, data, properties );                                                          \
       /*{const char* err = TwGetLastError(); assert(err == NULL);} */                                                                  \
    };
    ANT_INTERNAL_USE_(int, TW_TYPE_INT32);
    ANT_INTERNAL_USE_(unsigned, TW_TYPE_UINT32);
    ANT_INTERNAL_USE_(float, TW_TYPE_FLOAT);
    ANT_INTERNAL_USE_(double, TW_TYPE_DOUBLE);
    ANT_INTERNAL_USE_(bool, TW_TYPE_BOOLCPP);
    ANT_INTERNAL_USE_(char, TW_TYPE_CHAR);
    ANT_INTERNAL_USE_(float[4], TW_TYPE_COLOR4F);
    ANT_INTERNAL_USE_(vec3, TW_TYPE_DIR3F);
    ANT_INTERNAL_USE_(Quat, TW_TYPE_QUAT4F);
#undef ANT_INTERNAL_USE_

    std::string getUniqueName( void )
    {
      ++uniqueNameID;
      std::stringstream s;
      s << "unnamed";
      s << uniqueNameID;
      return s.str();
    }

    char barName[40];
    char visibleFalse[100];
    char visibleTrue[100];
    TwBar * bar;
    bool shown;
    static unsigned uniqueNameID;
    bool initializedBefore;
    int pos[2];
  };
  }//namespace Grimelins

#endif

#endif //GRIMELINS_ANT_TWEAK_H