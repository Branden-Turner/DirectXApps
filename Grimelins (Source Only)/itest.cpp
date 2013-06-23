/*! ====================================================================================================================
  @file itest.cpp

  @author Maximilian Manndorff
  @par Email maximilian.manndorff@@digipen.edu
  @par Login maximilian.manndorff
  @par ID 50006909
  @par Course CS350
  @par Project 1
  @date Feb.07.2012

  @brief 
    Test stuff

                        * Copyright 2012, DigiPen Institute os Technology, All Rights Reserved *

===================================================================================================================== */
#include "precompiled.h"
#include "itest.h"

Tester::~Tester(void)
{
  std::list<ITest*>::iterator it = mTests.begin();
  std::list<ITest*>::iterator endIt = mTests.end();

  for (; it != endIt; ++it)
    delete *it;
}

void Tester::TestAll(std::ostream& os)
{
  int numFailed = 0;
  int numSucceed = 0;
  int pass = 0;
  int total = mTests.size();

  os << "Starting test harness on " << total << " cases:" << std::endl;

  std::list<ITest*>::iterator it = mTests.begin();
  std::list<ITest*>::iterator endIt = mTests.end();


  for (; it != endIt; ++it, ++pass)
  {
    if (!(*it)->RunTest())
    {
      ++numFailed;
      os << "FAILURE (" << pass + 1 << "/" << total << "): " << (*it)->GetName() << std::endl;
    }
    else 
    {
      ++numSucceed;
      os << "SUCCESS (" << pass + 1 << "/" << total << "): " << (*it)->GetName() << std::endl;
    }
  }

  os << "Completed test harness with " << total << " cases." << std::endl;
  os << "  Succeeded: " << numSucceed << std::endl;
  os << "  Failed: " << numFailed << std::endl;
  os << "  Success Rate: " << numSucceed / (float)total << std::endl;
}
