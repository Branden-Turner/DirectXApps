/*! ====================================================================================================================
  @file taskmanager.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.17.2011

  @brief 
    Task managing and thread pooling.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_TASKMANAGER_H
#define VIRTUS_TASKMANAGER_H

#include <set>
#include "threadpool.h"

namespace Grimelins
{

    class ITask
    {
      typedef std::list<shrdptr<ITask>> TaskList;

    public:
      enum Priority
      {
        URGENT,
        STANDARD,
        LOW
      };


    public:
      ITask(void);
      virtual ~ITask(void);

      void AddDependency(shrdptr<ITask> dependency);
      u1 Execute(void);
      u1 IsDone(void) const;
      u1 DependenciesResolved(void) const;
      u1 Reset(void);
      void SetPriority(Priority priority);
      Priority GetPriority(void) const;
      void SetName(std::string name);
      std::string GetName(void) const;

    protected:
      virtual void vRun(void) = 0;

    private:
      ITask(ITask const& rhs);
      ITask& operator=(ITask const& rhs);

    private:
      TaskList mResDepend;
      TaskList mUnResDepend;
      u1 mDone;
      Priority mPriority;
      std::string mName;
    };


    class TaskSortByPriority
    {
    public:
      u1 operator()(shrdptr<ITask> const& lhs, shrdptr<ITask> const& rhs) const
      {
        return lhs->GetPriority() < rhs->GetPriority();
      }
    };

    class TaskManager
    {
      friend class Core;
      typedef std::multiset<shrdptr<ITask>, TaskSortByPriority> TaskQueue;

    public:
      TaskManager(void);
      virtual ~TaskManager(void);

    public:
      void ScheduleTask(shrdptr<ITask> process);

    protected:
      void process(void);

    private:
      TaskManager(TaskManager const& rhs);
      TaskManager& operator=(TaskManager const& rhs);

    private:
      ThreadPool mPool;
      TaskQueue  mQueue;
    };

} // namespace Virtus

#endif
