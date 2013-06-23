/*! ====================================================================================================================
  @file itest.h

  @author Maximilian Manndorff
  @par Email maximilian.manndorff@@digipen.edu
  @par Login maximilian.manndorff
  @par ID 50006909
  @par Course CS350
  @par Project 1
  @date Feb.07.2012

  @brief 
    Testing framework

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *

===================================================================================================================== */
#pragma once


class ITest
{
public:
  virtual ~ITest(void) {}
  virtual std::string GetName(void) const = 0;
  virtual bool RunTest(void) = 0;
};

class Tester
{
public:
  ~Tester(void);

  template <typename T>
  void AddTest(T const& test)
  {
    ITest* iTest = new T(test);
    mTests.push_back(iTest);
  }

  void TestAll(std::ostream& os);

private:
  std::list<ITest*> mTests;
};
