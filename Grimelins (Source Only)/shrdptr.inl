/*! ====================================================================================================================
  @file shrdptr.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.21.2011

  @brief 
    Shared pointer container.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

namespace Grimelins
{

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Autos

  template <typename T, typename DelFunc>
  shrdptr<T, DelFunc>::shrdptr(void) : mResource(0), 
                                       mEffectPtr(0), 
                                       mDealloc(DelFunc())
  {
    
  }

  template <typename T, typename DelFunc>
  shrdptr<T, DelFunc>::shrdptr(shrdptr const& rhs) : mResource(rhs.mResource), 
                                                     mEffectPtr(rhs.mEffectPtr), 
                                                     mDealloc(DelFunc())
  {
    ++mResource->numRef;
  }

  template <typename T, typename DelFunc>
  shrdptr<T, DelFunc>::shrdptr(T* res) : mResource(0), 
                                         mEffectPtr(0)
  {
    allocResource(res);
  }

  template <typename T, typename DelFunc>
  shrdptr<T, DelFunc>::~shrdptr(void)
  {
    disbandResource();
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Operators


  /////////// Write ////////////

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc>& shrdptr<T, DelFunc>::operator=(shrdptr const& rhs)
  {
    if (this != &rhs)
    {
      disbandResource();
      mResource = rhs.mResource;
      ++mResource->numRef;
      mEffectPtr = rhs.mEffectPtr;
    }

    return *this;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc>& shrdptr<T, DelFunc>::operator=(T* rhs)
  {
    if (!mResource || mResource->data != rhs)
    {
      disbandResource();
      allocResource(rhs);
    }

    return *this;
  }


  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc> shrdptr<T, DelFunc>::operator++(s32)
  {
    shrdptr ret(*this);
    ++ret.mEffectPtr;
    return ret;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc>& shrdptr<T, DelFunc>::operator++(void)
  {
    ++mEffectPtr;
    return *this;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc> shrdptr<T, DelFunc>::operator--(s32)
  {
    shrdptr ret(*this);
    --ret.mEffectPtr;
    return ret;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc>& shrdptr<T, DelFunc>::operator--(void)
  {
    --mEffectPtr;
    return *this;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc>& shrdptr<T, DelFunc>::operator+=(s32 i)
  {
    mEffectPtr += i;
    return *this;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc>& shrdptr<T, DelFunc>::operator-=(s32 i)
  {
    mEffectPtr -= i;
    return *this;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc> shrdptr<T, DelFunc>::operator+(s32 i)
  {
    shrdptr ret(*this);
    ret.mEffectPtr += i;
    return ret;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc> shrdptr<T, DelFunc>::operator-(s32 i)
  {
    shrdptr ret(*this);
    ret.mEffectPtr -= i;
    return ret;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc> shrdptr<T, DelFunc>::operator*(s32 i)
  {
    shrdptr ret(*this);
    ret.mEffectPtr *= i;
    return ret;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc> shrdptr<T, DelFunc>::operator/(s32 i)
  {
    shrdptr ret(*this);
    ret.mEffectPtr /= i;
    return ret;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc>& shrdptr<T, DelFunc>::operator*=(s32 i)
  {
    mEffectPtr *= i;
    return *this;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc>& shrdptr<T, DelFunc>::operator/=(s32 i)
  {
    mEffectPtr /= i;
    return this;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc> shrdptr<T, DelFunc>::operator%(s32 i)
  {
    shrdptr ret(*this);
    ret.mEffectPtr %= i;
    return ret;
  }

  template <typename T, typename DelFunc>
  typename shrdptr<T, DelFunc> shrdptr<T, DelFunc>::operator%=(s32 i)
  {
    mEffectPtr %= i;
    return this;
  }


  /////////// Read / Write ////////////

  template <typename T, typename DelFunc>
  T const& shrdptr<T, DelFunc>::operator[](u32 i) const
  {
    return mEffectPtr[i];
  }

  template <typename T, typename DelFunc>
  T& shrdptr<T, DelFunc>::operator[](u32 i)
  {
    return mEffectPtr[i];
  }

  template <typename T, typename DelFunc>
  T const& shrdptr<T, DelFunc>::operator*(void) const
  {
    return *mEffectPtr;
  }

  template <typename T, typename DelFunc>
  T const* shrdptr<T, DelFunc>::operator->(void) const
  {
    return mEffectPtr;
  }

  template <typename T, typename DelFunc>
  T& shrdptr<T, DelFunc>::operator*(void)
  {
    return *mEffectPtr;
  }

  template <typename T, typename DelFunc>
  T* shrdptr<T, DelFunc>::operator->(void)
  {
    return mEffectPtr;
  }

  template <typename T, typename DelFunc>
  T* Grimelins::shrdptr<T, DelFunc>::GetPtr( void )
  {
    return mEffectPtr;
  }


  /////////// Comp ////////////

  template <typename T, typename DelFunc>
  u1 shrdptr<T, DelFunc>::operator<(shrdptr const& rhs) const
  {
    return (mEffectPtr < rhs.mEffectPtr);
  }

  template <typename T, typename DelFunc>
  u1 shrdptr<T, DelFunc>::operator>(shrdptr const& rhs) const
  {
    return (mEffectPtr > rhs.mEffectPtr);
  }

  template <typename T, typename DelFunc>
  u1 shrdptr<T, DelFunc>::operator==(shrdptr const& rhs) const
  {
    return (mEffectPtr == rhs.mEffectPtr);
  }

  template <typename T, typename DelFunc>
  u1 shrdptr<T, DelFunc>::operator!=(shrdptr const& rhs) const
  {
    return (mEffectPtr != rhs.mEffectPtr);
  }

  template <typename T, typename DelFunc>
  shrdptr<T, DelFunc>::operator bool(void) const
  {
    return mResource != NULL;
  }


  /////////// Helpers ////////////
  template <typename T, typename DelFunc>
  void shrdptr<T, DelFunc>::disbandResource(void)
  {
    if (!mResource)
      return;

    if (--mResource->numRef == 0)
    {
      if (mResource->data)
        mDealloc(mResource->data);    
      delete mResource;
    }

    mResource = 0;
    mEffectPtr = 0;
  }

  template <typename T, typename DelFunc>
  void shrdptr<T, DelFunc>::allocResource(T* Resource)
  {
    if (Resource)
    {
      Assert(mResource == 0, "Allocated new resource before old one was gone.");
      mResource = new resource;
      mResource->numRef = 1;
      mResource->data = Resource;
      mEffectPtr = Resource;
    }
  }



} // namespace Virtus