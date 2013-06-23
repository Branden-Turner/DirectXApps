/*! ====================================================================================================================
  @file optional.inl

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.14.2011

  @brief 
    Data structure that allows optional variables. This might be useful for functions that return either a value or
    a failure code.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

namespace Grimelins
{

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // BOptional

  template <typename unsigned long Tsize>
  BOptional<Tsize>::BOptional(void) : m_bValid(false)
  {
    
  }

  template <typename unsigned long Tsize>
  BOptional<Tsize>::BOptional(BOptional const& orig) : m_bValid(orig.m_bValid)
  {

  }

  template <typename unsigned long Tsize>
  typename BOptional<Tsize>& BOptional<Tsize>::operator=(BOptional const& rhs)
  {
    m_bValid = rhs.m_bValid;
    return *this;
  }

  template <typename unsigned long Tsize>
  bool BOptional<Tsize>::IsValid(void) const
  {
    return m_bValid;
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Optional

  template <typename T>
  Optional<T>::Optional(void)
  {
    
  }

  template <typename T>
  Optional<T>::Optional(T const& dat)
  {
    construct(dat);
    m_bValid = true;
  }

  template <typename T>
  Optional<T>::Optional(OptionalEmpty const&)
  {

  }

  template <typename T>
  Optional<T>::Optional(Optional const& orig)
  {
    if (orig.m_bValid)
    {
      construct(*orig);
      m_bValid = true;
    }
  }

  template <typename T>
  Optional<T>::~Optional(void)
  {
    if (m_bValid)
      destroy();
  }

  template <typename T>
  typename Optional<T>& Optional<T>::operator=(T const& dat)
  {
    if (!m_bValid)
      throw OptionalException(OptionalException::INVALID_ACCESS);

    *GetT() = dat;

    return *this;
  }

  template <typename T>
  typename Optional<T>& Optional<T>::operator=(Optional const& orig)
  {
    if (this != &orig)
    {
      if (m_bValid)
      {
        // Destroy before copying (exception safety)
        m_bValid = false;
        destroy();
      }

      if (orig.m_bValid)
      {
        construct(*orig);
        m_bValid = true;
      }
    }

    return *this;
  }

  template <typename T>
  bool Optional<T>::operator==(Optional const& rhs) const
  {
    if (!m_bValid && !rhs.m_bValid) 
      return true;

    // Exclusive comparison: Does the validity not match?
    if (m_bValid ^ rhs.m_bValid)
      return false;

    return **this == *rhs;
  }

  template <typename T>
  bool Optional<T>::operator<(Optional const& rhs) const
  {
    if (!m_bValid && !rhs.m_bValid)
      return false;

    if (!m_bValid)
      return true;

    if (!rhs.m_bValid)
      return false;

    return **this < *other;
  }

  template <typename T>
  T const& Optional<T>::operator*(void) const
  {
    if (!m_bValid)
      throw OptionalException(OptionalException::INVALID_ACCESS);

    return *GetT();
  }

  template <typename T>
  T& Optional<T>::operator*(void)
  {
    if (!m_bValid)
      throw OptionalException(OptionalException::INVALID_ACCESS);

    return *GetT();
  }

  template <typename T>
  T& Optional<T>::operator->(void)
  {
    if (!m_bValid)
      throw OptionalException(OptionalException::INVALID_ACCESS);

    return *GetT();
  }

  template <typename T>
  void Optional<T>::Clear(void)
  {
    if (m_bValid)
    {
      m_bValid = false;
      destroy();
    }
  }

  template <typename T>
  bool Optional<T>::IsValid(void) const
  {
    return m_bValid;
  }

  template <typename T>
  T const* const Optional<T>::GetT(void) const
  {
    return RCAST(T const* const, m_Data);
  }

  template <typename T>
  T* const Optional<T>::GetT(void)
  {
    return RCAST(T*, m_Data);
  }

  template <typename T>
  void Optional<T>::construct(T const& dat)
  {
    new (GetT()) T(dat);
  }

  template <typename T>
  void Optional<T>::destroy(void)
  {
    GetT()->~T();
  }

} // namespace virtus