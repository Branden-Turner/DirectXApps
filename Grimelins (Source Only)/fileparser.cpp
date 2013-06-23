/*! ====================================================================================================================
  @file fileparser.cpp

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Jul.2.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "fileparser.h"
namespace Grimelins
{
#define FILEPARSER_BUILD_SCOPE_DEBUG_TEXT 1

#if (VIRTUS_CONFIG_DEBUG_MODE || VIRTUS_CONFIG_PROFILE_MODE) && FILEPARSER_BUILD_SCOPE_DEBUG_TEXT
#define moveTabStopIn() tabStop += "  ";
#define moveTabStopOut() tabStop = tabStop.substr(2);
#define recordSerialization(str) debugInfo += str;
  Text tabStop;
#else
#define moveTabStopIn() void(0)
#define moveTabStopOut() void(0)
#define recordSerialization(str) void(0)

#endif

  TypeParser::TypeParser( void )
  {
  }

  TypeParser::~TypeParser( void )
  {

  }

  bool TypeParser::parseFile( Text const& file )
  {
    m_stream.newStream(file);
    if(!m_stream.hasData()) return false;

    fileScope.name = file;
    recordSerialization(fileScope.name + ":\n");
    while(m_stream.hasData())
    {
      parseScope(fileScope);
    }
    return true;
  }

  /*! --------------------------------------------------------------------------------------------------------------------
    @brief    RECURSIVE!!  Serialize container: "As long as there is data in the current container,
                                                 see if the data is a variable or another container...
                                                 serialize variables, serialize containers..."
  
    @param parent  The data Scope that encapsulates the current recursion level.
    
    @return   returns when a variable or sub-scope is serialized, or when the data stream is empty.
     
    @date 27.10.2011
  --------------------------------------------------------------------------------------------------------------------- */
  void TypeParser::parseScope( Scope& parent )
  {
    moveTabStopIn();
    m_stream.cleanStream();
    if(!m_stream.hasData()) return;

    //variable
    if(m_stream.peekLineFor(':'))
    {
      parseScopeLine(parent);
    }
    //list
    else if(m_stream.peekLineFor('|'))
    {
      parseScopeList(parent);
      recordSerialization(tabStop + "}\n");
    }
    //Scope
    else
    {
      Scope* s = new Scope();
      s->name =  m_stream.read<Text>();
      s->parent = &parent;
      recordSerialization(tabStop + s->name + "\n" + tabStop + "{\n");
      parent.children.push_back(s);

      //recurse
      while(m_stream.inScope())
        parseScope(*s);
      recordSerialization(tabStop + "}\n");
    }
    moveTabStopOut();
  }

#define loc_parseStart() if(0){
#define loc_parseIfType(typ) return;} if(typeName == #typ){
#define loc_parsePushType(typ, expr) typ t_ = expr; recordSerialization("=" + toText<typ>(t_) + "\n"); self.typ##_.push_back(Variable<typ>(t_, propertyName))
  //parseIfTypeStd combines the two macros above in one for types that can be directly serialized.
#define loc_parseIfTypeStd(typ) return;} if(typeName == #typ){typ t_ = m_stream.read<typ>(); recordSerialization("=(value)\n"); self.typ##_.push_back(Variable<typ>(t_, propertyName))
#define loc_parseComplete()  return;}ErrorIf(true, "Unhandled type '%s' during serialization.\n%s", typeName.c_str(), (self.buildTree() + "::<" + typeName + ">" + propertyName).c_str())
#define loc_parseColor(clr) if(val == #clr) \
  self.Color_.push_back(Variable<Color>(clr, propertyName))
#define loc_parseShape(shp) if(val == #shp) \
  self.ShapeType_.push_back(Variable<ShapeType>(shp, propertyName))
#define loc_parseNarrowType(typ) if(val == #typ) \
  self.NarrowPhaseType_.push_back(Variable<NarrowPhaseType>(typ, propertyName))
#define loc_parseCollisionType(typ) if(val == #typ) \
  self.CollisionType_.push_back(Variable<CollisionType>(typ, propertyName))

  /*! --------------------------------------------------------------------------------------------------------------------
    @brief    This method assumes that the next line in m_stream is a line of data, and serializes it into the Scope
              'self' according to its type.
  
    @param self   the scope who owns the variable being serialized
     
    @date 27.10.2011
  --------------------------------------------------------------------------------------------------------------------- */
  void TypeParser::parseScopeLine( Scope& self)
  {
    Text typeName = m_stream.readSubstring(':');
    Text propertyName = m_stream.readSubstring('=');
    recordSerialization(tabStop + typeName + ":" + propertyName);

    loc_parseStart();


    //these macros essentially perform the following: (take u32 for example)
    //  if(typeName == "u32")
    //  {
    //    u32 data = m_stream.read<u32>();
    //    self.u32_.push_back( Variable<u32>(data,propertyName) );
    //  }
    // !!!!    Notice that the type must be able to be serialized as defined in filestream.h    !!!!!!!

    loc_parseIfType(u1)
      Text val = m_stream.read<Text>();
      loc_parsePushType(u1, val!="0" && val!="false");
    loc_parseIfType(ch) loc_parsePushType(ch, m_stream.read<Text>()[0]);
    loc_parseIfType(u8) loc_parsePushType(u8, m_stream.read<Text>()[0]);
    loc_parseIfType(s8) loc_parsePushType(s8, m_stream.read<Text>()[0]);
    loc_parseIfTypeStd(u16);
    loc_parseIfTypeStd(s16);
    loc_parseIfTypeStd(u32);
    loc_parseIfTypeStd(s32);
    loc_parseIfTypeStd(u64);
    loc_parseIfTypeStd(s64);
    loc_parseIfTypeStd(f32);
    loc_parseIfTypeStd(f64);
    loc_parseIfTypeStd(vec2);
    loc_parseIfTypeStd(vec3);
    loc_parseIfTypeStd(vec4);
    loc_parseIfTypeStd(Text);
    loc_parseIfTypeStd(Quat);
    loc_parseIfType(Color)
      Text val = m_stream.read<Text>();
      loc_parseColor(Red);
      else loc_parseColor(Green);
      else loc_parseColor(Blue);
      else loc_parseColor(Cyan);
      else loc_parseColor(Yellow);
      else loc_parseColor(Magenta);
      else loc_parseColor(Black);
      else loc_parseColor(White);
      else loc_parseColor(Gray);
      else loc_parseColor(LightGray);
      else loc_parseColor(DarkGray);
    loc_parseIfType(ShapeType)
      Text val = m_stream.read<Text>();
      loc_parseShape(ShapePlane);
      else loc_parseShape(ShapeCube);
      else loc_parseShape(ShapeSphere);
      else loc_parseShape(ShapeCylinder);
      else loc_parseShape(ShapeCone);
      else loc_parseShape(ShapeTorus);
      else loc_parseShape(ShapeQuad2D);
      else loc_parseShape(ShapeQuad3D);
      else loc_parseShape(ShapeSkybox);
    loc_parseIfType(NarrowPhaseType)
      Text val = m_stream.read<Text>();
      loc_parseNarrowType(Obb);
      else loc_parseNarrowType(BoundingSphere);
    loc_parseIfType(CollisionType)
      Text val = m_stream.read<Text>();
      loc_parseCollisionType(Player1);
      else loc_parseCollisionType(Boss);
      else loc_parseCollisionType(DamageSource);
      else loc_parseCollisionType(Environment);
    loc_parseComplete();
  }


#define loc_parseListStart() if(0){
#define loc_parseListIfType(typ) return;} if(listType == #typ){       \
  std::list<typ> t;                                                   \
  moveTabStopIn();                                                    \
  while(m_stream.inScope())                                           \
  {                                                                   \
    typ t_ = m_stream.read<typ>();                                    \
    t.push_back(t_);                                                  \
    recordSerialization(tabStop + "(value)\n");                       \
  }                                                                   \
  moveTabStopOut();                                                   \
  self.##typ##List_.push_back(Variable<std::list<typ>>(t, listName))

#define loc_parseListComplete()  return;}ErrorIf(true, "Unhandled List '%s' of type '%s' during serialization.\n%s", listName.c_str(), listType.c_str())

  void TypeParser::parseScopeList( Scope& self )
  {
    Text listName = m_stream.readSubstring('|');
    Text listType = m_stream.read<Text>();
    recordSerialization(tabStop + listName + "|" + listType + "\n" + tabStop + "{\n");
    loc_parseListStart();
    loc_parseListIfType(Text);
    loc_parseListIfType(u16);
    loc_parseListIfType(s16);
    loc_parseListIfType(u32);
    loc_parseListIfType(s32);
    loc_parseListIfType(u64);
    loc_parseListIfType(s64);
    loc_parseListIfType(f32);
    loc_parseListIfType(f64);
    loc_parseListIfType(vec2);
    loc_parseListIfType(vec3);
    loc_parseListIfType(vec4);
    loc_parseListIfType(Text);
    loc_parseListIfType(Quat);
    loc_parseListComplete();


//     std::list<Text> t;
//     while(m_stream.inScope())
//       t.push_back(m_stream.read<Text>());
//     self.TextList_.push_back(Variable<std::list<Text>>(t, listName));

  }











  FileParser::FileParser( std::string const& file )
  {
    if(file!="")
      parseFile(file);
  }

  FileParser::~FileParser( void )
  {
    while(parent.size())
    {
      delete parent.front();
      parent.pop_front();
    }
  }

  void FileParser::parseFile( std::string const& file )
  {
    m_stream.newStream(file);
    while(m_stream.hasData())
    {
      FileData* fd = new FileData();
      fd->name = m_stream.read<std::string>();
      while(m_stream.inScope())
      {
        ScopeData* sd = new ScopeData();
        sd->scopeName = m_stream.read<std::string>();
        while(m_stream.inScope())
        {
          DataPair* dp = new DataPair();
          dp->typeName = m_stream.readSubstring(':');
          dp->propertyName = m_stream.readSubstring('=');
          if(dp->typeName == "ch")
          {
            std::string val = m_stream.read<std::string>();
            dp->data = new ch(val[0]);
          }
          else if(dp->typeName == "u1")
          {
            std::string val = m_stream.read<std::string>();
            dp->data = new u1(val=="1" || val=="true");
          }
          else if(dp->typeName == "u8")
          {
            std::string val = m_stream.read<std::string>();
            dp->data = new u8(val[0]);
          }
          else if(dp->typeName == "s8")
          {
            std::string val = m_stream.read<std::string>();
            dp->data = new s8(val[0]);
          }
          else if(dp->typeName == "u16")
            dp->data = new u16(m_stream.read<u16>());
          else if(dp->typeName == "s16")
            dp->data = new s16(m_stream.read<s16>());
          else if(dp->typeName == "u32")
            dp->data = new u32(m_stream.read<u32>());
          else if(dp->typeName == "s32")
            dp->data = new s32(m_stream.read<s32>());
          else if(dp->typeName == "u64")
            dp->data = new u64(m_stream.read<u64>());
          else if(dp->typeName == "s64")
            dp->data = new s64(m_stream.read<s64>());
          else if(dp->typeName == "f32")
            dp->data = new f32(m_stream.read<f32>());
          else if(dp->typeName == "f64")
            dp->data = new f64(m_stream.read<f64>());
          else if(dp->typeName == "vec2")
            dp->data = new vec2(m_stream.read<vec2>());
          else if(dp->typeName == "vec3")
            dp->data = new vec3(m_stream.read<vec3>());
          else if(dp->typeName == "vec4")
            dp->data = new vec4(m_stream.read<vec4>());

          sd->properties.push_back(dp);
        }
        fd->scopes.push_back(sd);
      }
      parent.push_back(fd);
    }
  }

  void FileParser::pop_front( void )
  {
    delete parent.front();
    parent.pop_front();
  }

  Scope::~Scope( void )
  {
    while(children.size())
    {
      delete children.front();
      children.pop_front();
    }

  }
};//namespace