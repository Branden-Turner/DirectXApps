/*! ====================================================================================================================
  @file helpers.inl

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Oct.14.2011

  @brief 
    Small helper macros and functions

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_HELPERS_INL
#define VIRTUS_HELPERS_INL

namespace Grimelins
{
  
  // Couple useful templates borrowed from Chris Peters

  template< typename T >
  void SafeDeleteArray( T& interfacePtr )
  {
    if( interfacePtr) delete[] interfacePtr;
    interfacePtr = NULL;
  }

  template< typename T >
  void SafeRelease( T& interfacePtr ) {
    if( interfacePtr ) interfacePtr->Release();
    interfacePtr = NULL;
  }

  template< typename T >
  void SafeDelete( T& objectPtr ) {
    if( objectPtr ) delete objectPtr;
    objectPtr = NULL;
  }

  // More useful templates borrowed from Cody Pritchard

  template <class CTR, class T>
  bool Contains(const CTR& container, const T& Tect) {
    return (std::find(container.begin(), container.end(), Tect) != container.end());
  }

  template <class CTR, class T>
  bool MapContains(const CTR& container, const T& Tect) {
    return (container.find(Tect) != container.end());
  }

  template <class CTR, class T>
  void Remove(CTR& container, const T& Tect) {
    CTR::iterator location = std::find(container.begin(), container.end(), Tect);
    if (location != container.end()) { container.erase(location); }
  }

  template <class CTR>
  void ReleaseContainer(CTR& container) {
    for (CTR::iterator I = container.begin() ; I != container.end() ; ++I) { SafeRelease(*I); }
  }

  template <class CTR>
  void DeleteContainer(CTR& container) {
    for (CTR::iterator I = container.begin() ; I != container.end() ; ++I) { SafeDelete(*I); }
  }

  template <class CTR>
  void DeleteAndClear(CTR& container) {
    for (CTR::iterator I = container.begin() ; I != container.end() ; ++I) { SafeDelete(*I); }
    container.clear();
  }

  template <class CTR>
  void ReleaseAndClear(CTR& container) {
    for (CTR::iterator I = container.begin() ; I != container.end() ; ++I) { SafeRelease(*I); }
    container.clear();
  }

  template <class CTR>
  void MapDeleteAndClear(CTR& container) {
    for (CTR::iterator I = container.begin() ; I != container.end() ; ++I) { SafeDelete(I->second); }
    container.clear();
  }

  template <class CTR>
  void MapReleaseAndClear(CTR& container) {
    for (CTR::iterator I = container.begin() ; I != container.end() ; ++I) { SafeRelease(I->second); }
    container.clear();
  }

  template <typename TYPE, typename DATA>
  TYPE checked_cast(DATA p)
  {
#if VIRTUS_CONFIG_DEBUG_MODE
    TYPE a = dynamic_cast<TYPE>(p);
    ErrorIf(!a, "Invalid cast!");
    return a;
#else
    return static_cast<TYPE>(p);
#endif
  }


} // namespace Virtus


#endif // Include Guard