/*! ====================================================================================================================
  @file fileparser.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Nov.2.2011

  @brief 

  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_FILEPARSER_H
#define VIRTUS_FILEPARSER_H

#include "filestream.h"
#include "shapes.h"
#include "color.h"
#include "boundingtypes.h"
#define DeclScopeType(nam)                                          \
  typedef std::list<Variable<nam>> nam##List;                       \
  typedef nam##List::iterator nam##It;                              \
  nam##List nam##_;                                                 \
  typedef std::list< Variable<std::list<nam>>> nam##ListList;       \
  typedef nam##ListList::iterator nam##ListIt;                      \
  nam##ListList nam##List_;

//Each type gets two variables, below is the example of how the macro expands for 'Text'
/*
  typedef std::list< Variable<Text> > TextList;
  typedef TextList::iterator TextIt;
  TextList Text_;

  typedef std::list< Variable< std::list<Text> > > TextListList;
  typedef TextListList::iterator TextListIt;
  TextListList TextList_;
*/

namespace Grimelins
{

  template <typename T>
  struct Variable
  {
    T data;
    Text name;
    Variable(T const& d, Text const& n):data(d), name(n){};
  };
  struct Scope
  {
    typedef std::list<Scope*> ScopeList;
    Scope(void) :parent(NULL){};
    ~Scope(void);

    Text name;
    ScopeList children;

    DeclScopeType(ch);
    DeclScopeType(Text);
    DeclScopeType(u1);
    DeclScopeType(u8);
    DeclScopeType(s8);
    DeclScopeType(s16);
    DeclScopeType(u16);
    DeclScopeType(s32);
    DeclScopeType(u32);
    DeclScopeType(s64);
    DeclScopeType(u64);
    DeclScopeType(f32);
    DeclScopeType(f64);
    DeclScopeType(vec2);
    DeclScopeType(vec3);
    DeclScopeType(vec4);
    DeclScopeType(Quat);
    DeclScopeType(ShapeType);
    DeclScopeType(Color);
    DeclScopeType(NarrowPhaseType);
    DeclScopeType(CollisionType);
    Text buildTree(void)
    {
      if(parent)
        return parent->buildTree() + "::" + name;
      else
        return name;
    }
    Scope* parent;
  };

  class TypeParser
  {
  public:
    TypeParser(void);
    ~TypeParser(void);
    bool parseFile(Text const& file);


#if VIRTUS_CONFIG_DEBUG_MODE || VIRTUS_CONFIG_PROFILE_MODE
    Text debugInfo;
#endif

    Scope fileScope;

  private:
    void parseScope( Scope& parent);
    void parseScopeLine( Scope& self);
    void parseScopeList( Scope& self);
    FileStream m_stream;
  };

  class FileParser
  {
  public:
    struct DataPair
    {
      std::string propertyName;
      std::string typeName;
      void* data;
      DataPair(void) :data(NULL){ };
      ~DataPair(void) { delete data; };
    };
    struct ScopeData
    {
      std::string scopeName;
      std::list<DataPair*> properties;
      ScopeData(void) { };
      ~ScopeData(void) { 
        while(properties.size()){delete properties.front(); properties.pop_front();}};

    };
    struct FileData
    {
      std::string name;
      std::list<ScopeData*> scopes;
      FileData(void) { };
      ~FileData(void) { 
        while(scopes.size()){delete scopes.front(); scopes.pop_front();}};

    };
    typedef std::list<FileData*>::const_iterator parentIterator;
    typedef std::list<DataPair*>::const_iterator propertiesIterator;
    typedef std::list<ScopeData*>::const_iterator scopeIterator;

    FileParser(std::string const& file="");
    ~FileParser(void);

    void parseFile(std::string const& file);
    void pop_front(void);

    std::list<FileData*> parent;

  private:
    FileStream m_stream;
  };
};//namespace
#endif
