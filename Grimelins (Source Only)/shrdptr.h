/*! ====================================================================================================================
  @file shrdptr.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.21.2011

  @brief 
    Shared pointer to Resources->

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_SHRDPTR_H
#define VIRTUS_SHRDPTR_H


namespace Grimelins
{

  //TODO: Change from templated delete methods to function ptrs
  template <typename T>
  class SingleDelete
  {
  public:
    void operator()(T* dat) { delete dat; }
  };

  template <typename T>
  class ArrayDelete
  {
  public:
    void operator()(T* dat) { delete [] dat; }
  };

  class NoDelete
  {
  public:
    void operator()(void* dat) { /*NOP*/ }
  };

  template <typename T, typename DelFunc = SingleDelete<T> >
  class shrdptr
  {
  private: // aggr
    struct resource
    {
      T* data;
      u32 numRef;

      resource(void) : data(0), numRef(0) {}
    };

  public: // autos
    shrdptr(void);
    shrdptr(shrdptr const& rhs);
    shrdptr(T* res);
    ~shrdptr(void);

  public: // operators

    T* GetPtr(void);

    // write
    shrdptr& operator=(shrdptr const& rhs);
    shrdptr& operator=(T* rhs);
    shrdptr operator++(s32);
    shrdptr& operator++(void);
    shrdptr operator--(s32);
    shrdptr& operator--(void);
    shrdptr& operator+=(s32 i);
    shrdptr& operator-=(s32 i);
    shrdptr operator+(s32 i);
    shrdptr operator-(s32 i);
    shrdptr operator*(s32 i);
    shrdptr operator/(s32 i);
    shrdptr& operator*=(s32 i);
    shrdptr& operator/=(s32 i);
    shrdptr operator%(s32 i);
    shrdptr operator%=(s32 i);

    // read / write
    T const& operator*(void) const;
    T const* operator->(void) const;
    T& operator*(void);
    T* operator->(void);
    T const& operator[](u32 i) const;
    T& operator[](u32 i);

    // comp
    u1 operator<(shrdptr const& rhs) const;
    u1 operator>(shrdptr const& rhs) const;
    u1 operator==(shrdptr const& rhs) const;
    u1 operator!=(shrdptr const& rhs) const;
    operator bool(void) const;

  private: // methods
    void disbandResource(void);
    void allocResource(T* res);

  private: // data
    resource* mResource;  /// The reference counted base pointer (beginning of block)
    T*        mEffectPtr; /// The currently effective pointer (manipulated by ++, --, +=, ...)
    DelFunc   mDealloc;   /// The deallocation functor
  };



} // namespace Virtus

#include "shrdptr.inl"

#endif // Include Guard