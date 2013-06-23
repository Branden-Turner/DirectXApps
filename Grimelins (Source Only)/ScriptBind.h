/*! ====================================================================================================================
  @file ScriptBind.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Oct.23.2011

  @brief This is the ONLY script file you need to include to bind a class to script, it will include all necessary libraries for you.

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#ifndef VIRTUS_IMPORTSQUIRREL_H
#define VIRTUS_IMPORTSQUIRREL_H

#include "scriptVM.h"
#include "scriptenum.h"

#define NSGLOBAL ::

    /*******************************************************************************************************************
     * GlobalEnum reserves the single letter g in script, binding a variable 
     * here called FOO would be retrieved in script with g.FOO syntax.
     * Caution: All of these variables must have unique names
     *
     * DO NOT CALL THIS MACRO, IT IS ALREADY SETUP IN SQVM_Bind.h
     \*****************************************************************************************************************/
    #define SCRIPT_GlobalEnum_Start() Grimelins::Engine::ScriptENUM globalEnumeration("g");{


    /*******************************************************************************************************************
     * Define the namespace for all global Enum values defined hereafter.
     * ns - The fully qualified namespace all future _Value calls will use.
     \*****************************************************************************************************************/
    #define SCRIPT_GlobalEnum_Using(ns) }{namespace globalLastUsed = ns

    /*******************************************************************************************************************
     * Takes value of n and exposes it in the g table by the same name.
     * n - Name of variable, and consequently the name in the g table.
     \*****************************************************************************************************************/
    #define SCRIPT_GlobalEnum_Value(n) globalEnumeration.newEnum(#n, globalLastUsed::n)

    /*******************************************************************************************************************
     * Takes value of 'n' and exposes it in the g table as the value of the 'as' parameter.
     * n - Name of variable
     * as - The C_String name the value will use in the g table
     \*****************************************************************************************************************/
    #define SCRIPT_GlobalEnum_ValueAs(n, as) globalEnumeration.newEnum(as, globalLastUsed::n)

    /*******************************************************************************************************************
     * Finalized the GlobalEnum call sequence.
     *
     * DO NOT CALL THIS MACRO, IT IS ALREADY SETUP IN SQVM_Bind.h
     \*****************************************************************************************************************/
    #define SCRIPT_GlobalEnum_End() }globalEnumeration.bind()


    /*******************************************************************************************************************
     * Begin Definition of an Enumeration which will be exposed to script.
     * IMPORTANT: Enumerations occupy the root table in script, and therefore
     * must each have a unique ScriptName 'n'.
     * n - Unique name to expose this enumeration set under in script.
     \*****************************************************************************************************************/
    #define SCRIPT_EnumStart(n)             \
      {                                     \
        Grimelins::ScriptENUM e(n)


    /*******************************************************************************************************************
     * Expose a c++ constant to script as an enumeration of the same name.
     * n - the name of the constant value. This must match the C++ data definition.
     \*****************************************************************************************************************/
    #define SCRIPT_EnumValue(n) e.newEnum(#n, n)

    /*******************************************************************************************************************
     * Expose a c++ constant to script as an enumeration of the specified name.
    * n - The name of the constant value. This must match the C++ data definition.
    * as - The C_String name to expose the value in script under.
    \*****************************************************************************************************************/
    #define SCRIPT_EnumValueAs(n, as) e.newEnum(as, n)


    /*******************************************************************************************************************
     * Finalize the Enumeration definition
     * (no args)
     \*****************************************************************************************************************/
    #define SCRIPT_EnumEnd()            \
      e.bind();                         \
        } do{}while(0)


    /* CLASS BINDING
      Define a C++ class for use in squirrel. Macros should be placed in a single block
      within the declaration in the class header. SCRIPT_RegisterGlobal is an exception
      which must instead be used in the CTOR of a singleton class.
      The final step not shown here is to use SCRIPT_BindType in SQVM_Bind.cpp

  */

    /*******************************************************************************************************************
     * Begin class definition for script binding.
     * The script class definition should in the .cpp file of your class.
     * IMPORTANT: Define this within the same namespace as your class!
     * c - Class name, which must be exactly the typename of the class 
     \*****************************************************************************************************************/
    #define SCRIPT_ClassStart(c)                      \
      void SVM_BindInterface_##c##(void)              \
      {                                               \
        static bool bound = false;                    \
        ErrorIf(bound, "An attempt to re-bind %s to script has been halted.\n", #c);\
        bound = true;                                 \
        /*printDebug("\n%s is Bound to script ", #c);   \
        */typedef c myType;                             \
        static const char myName [] =#c;              \
        HSQUIRRELVM svm = CoreInstance()->Scripter
 
    /*******************************************************************************************************************
     * Specifies that script is allowed to create objects of this type.
     * (no args)
     \*****************************************************************************************************************/
    #define SCRIPT_CTOR_Enabled()                 \
        /*printDebug("(CTOR enabled), Exposes:\n"); \
        */Sqrat::Class<myType> myInstance(svm)

 
    /*******************************************************************************************************************
     * Specifies that script is NOT allowed to create objects of this type.
     * Attempting to use a class of this type created in script will access
     * an unallocated class, and therefore be undefined behavior. In debug
     * this causes a crash. This is a good thing :)
     * (no args)
     \*****************************************************************************************************************/
    #define SCRIPT_CTOR_Disabled()                  \
        /*printDebug("(CTOR disabled), Exposes:\n");  \
        */Sqrat::Class<myType, Sqrat::NoConstructor> myInstance(svm)

    /*******************************************************************************************************************
     * Add METHOD to a class definition, enabling access to it from script.
     * IMPORTANT: only usable between SCRIPT_ClassStart and SCRIPT_ClassEnd

     * m - Name of the method, which must NOT be overloaded.
     \*****************************************************************************************************************/
    #define SCRIPT_Method(m)                      \
        /*printDebug("    %s [ Method ]\n", #m);    \
        */myInstance.Func(#m, &myType::##m)

    /*******************************************************************************************************************
     * Add OVERLOADED METHOD to a class definition, enabling access to it from script.
     * When an overloaded C++ method is exposed, each version you intend to expose must have a
     * unique name within script, as overloaded functions do not actually exist in script.
     * Use this macro to break your overloaded methods into methods by another name for script.
     * IMPORTANT: only usable between SCRIPT_ClassStart and SCRIPT_ClassEnd
     *
     * To implement operators, the following are allowed for 'as'
     * _add, _sub, _mul, _div, _unm, _modulo, _set, _get, _typeof, _nexti, _cmp, _call, _delslot,_tostring
     *
     * m - Name of the method, which must be overloaded.
     * as - C_string name to expose this version of the overloaded method to script as.
     * ret - the return type of the method overload.
     * args - the argument type list of the method enclosed in parenthesis. ex: (int, float)
     \*****************************************************************************************************************/
      #define SCRIPT_MethodOverload(m, as, ret, args )                          \
        /*printDebug("    %s [ Overload of %s %s %s ]\n", #as, #ret, #m, #args);  \
        */myInstance.Overload< ret (myType::*) args >( as, &myType::##m )


    /*******************************************************************************************************************
     * Add METHOD to a class definition, which enables access of this data from within squirrel.
     * IMPORTANT: only usable between SCRIPT_ClassStart and SCRIPT_ClassEnd
     *
     * d - Data member name, which MUST be a C++ data type (int, float etc)
     \*****************************************************************************************************************/
      #define SCRIPT_PlainData(d)                   \
        /*printDebug("    %s [ Plain Data ]\n", #d);  \
        */myInstance.Var(#d, &myType::##d)


    /*******************************************************************************************************************
     * Add METHOD to a class definition, which enables access of this data from within squirrel.
     * IMPORTANT: only usable between SCRIPT_ClassStart and SCRIPT_ClassEnd
     *
     * d - Data member name, which MUST be a user defined type (vec2, Quaternion, Factory etc)
     \*****************************************************************************************************************/
      #define SCRIPT_UserData(d)                    \
        /*printDebug("    %s [ User Data ]\n", #d);   \
        */myInstance.VarRef(#d, &myType::##d)


    /*******************************************************************************************************************
     * Add property to a class definition, which enables access of this data from within squirrel.
     * IMPORTANT: only usable between SCRIPT_ClassStart and SCRIPT_ClassEnd
     *
     * p - Property name to expose. This must be a property defined with the PROPERTY macros 
     \*****************************************************************************************************************/
      #define SCRIPT_Property(p)                      \
        /*printDebug("    %s [ Property ]\n", #p);      \
        */myInstance.Prop(#p, &myType::property_get__##p, &myType::property_set__##p)

    /*******************************************************************************************************************
     * Add READ ONLY PROPERTY to a class definition, which enables access of this
     * data from within squirrel.
     * IMPORTANT: only usable between SCRIPT_ClassStart and SCRIPT_ClassEnd
     *
     * p - Property name to expose. This must be a property defined with the PROPERTY _R macros 
     \*****************************************************************************************************************/
      #define SCRIPT_Property_R(p)                            \
        /*printDebug("    %s [ Read-only Property ]\n", #p);    \
        */myInstance.Prop(#p, &myType::property_get__##p)


    /*******************************************************************************************************************
     * Terminate the class definition (SCRIPT_ClassStart) and bind it to script.
     * The class is now able to be referenced in script!
     *
     * (no args)
     \*****************************************************************************************************************/
      #define SCRIPT_ClassEnd()                               \
        Sqrat::RootTable(svm).Bind(myName, myInstance);       \
        }

    /*******************************************************************************************************************
     * Register *this to the global table in script, accessible by the class name 
     * prefixed with g. gFactory for example.
     * Use of this implies SCRIPT_BindType(), therefore do not also call that!
     * Because it binds by class name, this must only be called in singleton CTORs! 
     * To bind per-instance globals use SCRIPT_ORIDE_RegisterGlobal
     *
     * className - The typename of the class.
     \*****************************************************************************************************************/
      #define SCRIPT_RegisterGlobal(className)          \
      {                                                 \
        SVM_BindInterface_##className();                \
        std::string g = "g";                            \
        g += #className;                                \
        Sqrat::RootTable(CoreInstance()->Scripter).SetInstance(g.c_str(), this);\
        /*printDebug("\n    Registered as a global '%s'\n", g.c_str());\
      */}do{}while(0)


    /*******************************************************************************************************************
     * Commit the binding definition for 'className'.
     * Use this in SQVM_Bind.cpp. #include your class header, and
     * call this below the other class bindings located there.
     * Note: This is not for classes using the SCRIPT_RegisterGlobal() macro.
     *
     * n - fully qualified namespace the class is defined in. If global use NSGLOBAL
     * c - the name of the class.
     \*****************************************************************************************************************/
      #define SCRIPT_RegisterType(c, n)                 \
      {                                                 \
        namespace c##_NS = n;                           \
        c##_NS::SVM_BindInterface_##c ();               \
      }do{}while(0)


    /*******************************************************************************************************************
     * Declares the intent to create a script enabled class. This must be
     * placed in the header file (.h) of the class, but can be before or after
     * the class definition itself.
     * IMPORTANT: Use this in the same namespace as your class!
     *
     * c - Class name, which must be exactly the typename of the class 
     \*****************************************************************************************************************/
      #define SCRIPT_HeaderDefine(c) void SVM_BindInterface_##c (void)


///////////////SPECIAL BINDINGS ///////////////////////////////////////

    /*******************************************************************************************************************
     * Register 'inst' to the global table in script, accessible by name.
     * This should only be used for non-singleton types, and care must be taken
     * to ensure you do not re-use a name that already exists on the global table
     * and that the instance does not go out of scope while script uses this global.
     *
     * inst - a pointer to the instance of the class to register.
     * as - C_String name to expose this instance on the global script table. Should be unique name!
     \*****************************************************************************************************************/
      #define SCRIPT_GlobalInstanceOfBoundType(inst, as) \
        ErrorIf(as == "self" || as == "on" || as == "menu", "Attempt to bind global instance (%X) using the restricted value '%s' in %s Line %s.\n", inst, as, __FILE__, __LINE__);\
        Sqrat::RootTable(CoreInstance()->Scripter).SetInstance(as, inst)


    /*******************************************************************************************************************
     * ******** WARNING: YOU DON'T MEAN TO USE THIS MACRO, PUT IT DOWN! ************
     *
     * Pushes the specified game object into the "self" table in the script virtual machine for use
     * by context sensitive, game object owned, behavior scripts.
     *
     * which - instance of a game object, which will run behavior scripts
     \*****************************************************************************************************************/
//       #define SCRIPT_SetActiveObject(which) \
//       Sqrat::RootTable(CoreInstance()->Scripter).SetInstance("self", which)



namespace Grimelins
{
  //Called by core to initialize all script bound elements
  void RunCoreBinding(void);
}


#endif //VIRTUS_IMPORTSQUIRREL_H