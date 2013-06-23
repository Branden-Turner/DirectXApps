/*! ====================================================================================================================
  @file testgeom.h

  @author Maximilian Manndorff
  @par Email maximilian.manndorff@@digipen.edu
  @par Login maximilian.manndorff
  @par ID 50006909
  @par Course CS350
  @par Project 1
  @date Feb.07.2012

  @brief 
    Unit tests for geometry library

                        * Copyright 2012, DigiPen Institute of Technology, All Rights Reserved *

===================================================================================================================== */
#pragma once
#include "itest.h"

void TestAll(void);

class TestProjPointLine : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestProjPointPlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistPointPoint : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistPointLineSegment : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistPointRay : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistPointLine : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistPointPlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistPointTriangle : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistPointSphere : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistPointAABB : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistLineLine : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistLineSphere : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestDistLineSegLineSeg : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestContainedPointLineSeg : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestContainedPointRay : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestContainedPointLine : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestContainedPointPlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestContainedPointTriangle : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestContainedPointSphere : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestContainedPointAABB : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionLinePLane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionLineSegmentPlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionLineSegmentTriangle : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionRayPlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionRayTriangle : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionRaySphere : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionRayAABB : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionSphereSphere : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionTriangleTriangle : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestIntersectionAabbAabb : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestAngleLineLine : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestAngleLinePlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestAnglePlanePlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestParallelLineLine : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestParallelLinePlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestParallelPlanePlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestPerpendicularLineLine : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestPerpendicularLinePlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestPerpendicularPlanePlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestCoplanarLinePlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};

class TestCoplanarPlanePlane : public ITest
{
public:
  virtual std::string GetName(void) const;
  virtual bool RunTest(void);
};