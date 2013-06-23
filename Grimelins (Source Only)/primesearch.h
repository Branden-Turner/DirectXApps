/*! ====================================================================================================================
  @file PrimeSearch.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.14.2011

  @brief 
    Primesearch 

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_PRIMESEARCH_H
#define VIRTUS_PRIMESEARCH_H

#include "optional.h"

namespace Grimelins
{
  class PrimeSearch
  {
  public:
    PrimeSearch(int elements);
    Optional<int> GetNext(bool restart = false);
    bool Done(void) const;
    void Restart(void);

  private:
    static int m_Primes[];

    int m_Skip;
    int m_CurrentPosition;
    int m_MaxElements;
    int* m_CurrentPrime;
    int m_Searches;
  };
} // namespace virtus

#endif // Include Guard