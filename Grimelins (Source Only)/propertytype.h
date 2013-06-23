/*! ====================================================================================================================
  @file property_type.h

  Codename Virtus 

  @author Jonathan Fitzpatrick <jonathan.fitzpatrick@digipen.edu>
  @date  Aug.24.2011

  @brief Defines properties of a class. A property is a data member that has either an accessor, mutator, or both.
    The special significance of a property using these macros, is that the get and set methods are exposed under a name
    that appears to end-users as a member variable. That is, you could expose both the GET and SET of a private data
    member named 'm_foo' as 'foo' which would enable access to the GET and SET method with ordinary struct-like access
    where the end-user types class.foo and actually is routed to the GET or SET method defined by the class.

    The limiting factor with properties is that GET must adhere to the signature "type get(void)" and set must adhere
    to the signature "void set(const type& value)" where 'type' is the same for both GET and SET. This does not allow
    for special overloads taking different types unless additional properties are created using a different name.
    An example of this exact situation is below.

    It is important to note that properties do not actually increase the data footprint of a class, and are not adding
    hidden data members! This is a Microsoft Visual Studio feature (read: hack or trick) and is not ANSI Standard C++.
    I wrote these macros, they are not copy+paste off some help website.

    Declaring a property:
          a) Use a PROPERTY(t,n) macro to declare the property in the header file, where t is the 'type' and n is the
             name by which end-users can access this property.

          b) Define the GET and SET method in the .cpp file. These must be in a proper namespace such that you could
             use the CLASSNAME::METHODNAME() syntax as if you were defining any other method of that class.
             You may omit the GET or SET implementations if you wish to prevent their use (read or write only).
             Pass the name of the class, and the name of the property (same as name given to PROPERTY macro).

          ** OR **

          a) The BASIC extensions replace the above, they declare AND implement a trivial accessor/mutator. The v 
             Variable is the name of the data member you wish to expose through the GET/SET. Optionally you can append 
             _R or _W to the BASIC extension to make the property read or write only, respectively. EX: PROPERTY_BASIC_R 
             is read-only.

   EXAMPLE:
   ---------------------------------------
   ---- foo.h ----
   namespace bar{

   class foo
   {
   public:
     PROPERTY(float, x);
     PROPERTY_BASIC(float, y, m_position.y);
     PROPERTY_BASIC(vec2, pos, m_position);
     PROPERTY(vec3, posIn3D);
     PROPERTY(float, length);

   private:
     vec2 m_position;
   }

   }//namespace bar
   ---------------------------------------
   ---- foo.cpp ----


   #include "foo.h"

   namespace bar{

   GET(foo, x){
     return m_position.x;
   }
   SET(foo, x){
   m_position.x = value;
   }

   GET(foo, posIn3D){
     return vec3(m_position.x, m_position.y, 0);
   }

   GET(foo, length){
     return m_position.length();
   }

   }//namespace bar
   ---------------------------------------
   ---- main.cpp ----
   #include "foo.h"

   int main(void)
   {
     foo myFoo;
     float a = myFoo.x;
     myFoo.x = 5.0f;         
     float b = myFoo.y;
     vec2  c = myFoo.pos;
     float d = myFoo.length;
     myFoo.length = 5; // Error, read only
     c.x = 5;
     c.y = 9;
     myFoo.pos = c;
   }
   ---------------------------------------


  * Copyright 2010, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once
#ifndef VIRTUS_PROPERTY_TYPE_H
#define VIRTUS_PROPERTY_TYPE_H

#define PROPERTY(t,n) __declspec( property ( put = property_set__##n, get = property_get__##n ) ) t n; typedef t property__typeof__##n; void property_set__##n(const property__typeof__##n& value); property__typeof__##n property_get__##n(void) const;

#define PROPERTY_BASIC(t,n,v)   __declspec( property ( put = property_set__##n, get = property_get__##n ) ) t n; void property_set__##n(const t& value){v=value;}; t property_get__##n(void) const{return v;};
#define PROPERTY_BASIC_R(t,n,v) __declspec( property ( put = property_set__##n, get = property_get__##n ) ) t n; void property_set__##n(const t& value); t property_get__##n(void) const{return v;};
#define PROPERTY_BASIC_W(t,n,v) __declspec( property ( put = property_set__##n, get = property_get__##n ) ) t n; void property_set__##n(const t& value){v=value;}; t property_get__##n(void) const;

#define SET(c, n) void c##::property_set__##n(const property__typeof__##n& value)
#define GET(c, n) c##::property__typeof__##n c##::property_get__##n() const


#endif //VIRTUS_PROPERTY_TYPE_H