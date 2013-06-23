 /*! ====================================================================================================================
   @file uigroup.h
 
   Codename Grimelins 
 
   @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
   @date  Nov.23.2011
 
   @brief Specifies a UI Grouping. That is, a set of related buttons and images, aka a menu.
  
   * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
 ===================================================================================================================== */
 #ifndef GRIMELINS_UIGROUP_H
 #define GRIMELINS_UIGROUP_H
 
 namespace Grimelins
 {
  struct UIElement;

  class UIGScript
  {
  public:
    UIGScript(void);
    ~UIGScript(void);
    void serialize(Text const& file);
    void addFunction(CText fName, bool tabStopped = true);
  private:
    friend class UIGroup;
    Sqrat::Function* getFunction(CText fName);
  private:
    typedef std::map<Text, Sqrat::Function*> menuFunctionMap;
    typedef std::vector<Text> FuncNames;
  private:
    menuFunctionMap functions;
    FuncNames tabbedFunctions;
    Sqrat::Script* script;
    Text file;
    Text tabStart;
  };

   SCRIPT_HeaderDefine(UIGroup);
   class UIGroup
   {
   public:
     UIGroup(Text const& name);
     ~UIGroup(void);

     void update(void);
     void setup(void);

     //navigation actions
     void Active(void);
     void Next(void);
     void Prev(void);

     //script invocation methods
     void Show(void);
     void Hide(void);
     void gainFocus(void);
     void loseFocus(void);
     void open(void);
     void close(void);

     void addStatic(Text const& image, f32 pX, f32 pY, f32 sX, f32 sY, f32 Alpha);
     void addDynamic(Text const& image, Text const& image2, Text const& fnName, f32 pX, f32 pY, f32 sX, f32 sY, f32 Alpha);
     void addOrderedStatic(Text const& image, f32 pX, f32 pY, f32 sX, f32 sY, f32 Alpha);
     void nextOrderedStatic(void);
     void prevOrderedStatic(void);

     void addTimer(f32 duration, Text const& fnName);



     u32 Priority(void);
   private:
     //helper
     void call(Text const& fName);

   private:
     friend class UserInterface;
     Text m_name;
     u1 m_visible;
     UIGScript m_script;
     Text m_curElement;
     typedef std::vector<UIElement*> ElementList;
     ElementList m_elements;
     Text m_parent;
     u1 m_isSetup;

     //timer support
     struct TimePackage
     {
       TimePackage(f32 d, Text const& fn) : duration(d), func(fn)
       {
         time.StartTimer();
         start = time.Get();
       }
       f32 start;
       f32 duration;
       FrameTimer time;
       Text func;
     };
     std::vector<TimePackage> m_timers;

     //ordered static elements insert themselves one at a time into m_elements
     std::vector<UIElement*> m_orderedStatics;
     u32 m_elementIndex;
     u32 m_orderedIndex;

   };
 }//namespace
 
 #endif
