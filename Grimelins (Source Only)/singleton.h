/*! ====================================================================================================================
  @file singleton.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.14.2011

  @brief 
    Singleton abstraction

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_SINGLETON_H
#define VIRTUS_SINGLETON_H

namespace Grimelins
{
  template <typename T>
  class Singleton
  {
  protected:
    Singleton(void)
    {
      assert ( m_pInstance == 0 && "Attempt to instantiate multiple singletons." );
      m_pInstance = this;
    }

    virtual ~Singleton(void) { m_pInstance = 0; }

  private: // disallow copies
    Singleton(Singleton const& rhs);
    Singleton& operator=(Singleton const& rhs);

  private: // static singleton pointer
    static Singleton<T>* m_pInstance;
  };


  template<typename T> Singleton<T>* Singleton<T>::m_pInstance = 0;


} // namespace Virtus

#endif // Include Guard