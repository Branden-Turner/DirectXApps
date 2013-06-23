/*! ====================================================================================================================
  @file taskmanager.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.22.2011

  @brief 
    Task management.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "taskmanager.h"

namespace Grimelins
{
    //////////////////////////////////////////////////////////////////////////
    // Task

    ITask::ITask(void) : mDone(false), mPriority(STANDARD), mName("Unnamed Task")
    {
      
    }

    ITask::~ITask(void)
    {

    }

    void ITask::AddDependency(shrdptr<ITask> dependency)
    {
      mUnResDepend.push_back(dependency);
    }

    u1 ITask::Execute(void)
    {
      if (mDone)
      {
        Error("WARNING: Attempt to execute task that's already complete. (Reset task before reuse)");
        return false;
      }

      if (!mUnResDepend.empty())
      {
        // Resolve dependency tree
        TaskList::iterator it = mUnResDepend.begin();
        TaskList::iterator endIt = mUnResDepend.end();
        
        for (; it != endIt; ++it)
        {
          if (!(*it)->Execute())
            break;
        }

        if (it == endIt)
          mResDepend.splice(mResDepend.end(), mUnResDepend);
        else if (!mUnResDepend.empty())
        {
          Error("ABORT: Task did not complete sucessfully in dependency tree. Aborting further execution.");

          // Move tasks that have been resolved to resolved dependency list
          TaskList::iterator failedIt = it;
          --failedIt;
          mResDepend.splice(mResDepend.end(), mUnResDepend, failedIt, mUnResDepend.end());
          return false;
        }

      }




      this->vRun();
      mDone = true;

      return true;
    }

    u1 ITask::IsDone(void) const
    {
      return mDone;
    }

    u1 ITask::DependenciesResolved(void) const
    {
      return mUnResDepend.empty();
    }

    u1 ITask::Reset(void)
    {
      if (!mDone)
      {
        Error("WARNING: Attempt to reset task that didn't complete yet.");
        return false;
      }

      // Reset all resolved dependencies in the chain
      // which effectively marks all task below this one 
      // as yet to run with no resolved dependencies
      TaskList::iterator it = mResDepend.begin();
      TaskList::iterator endIt = mResDepend.end();

      for (; it != endIt; ++it)
        (*it)->Reset();

      // Move resolved dependencies to unresolved task list
      mUnResDepend.splice(mUnResDepend.end(), mResDepend);

      return true;
    }

    void ITask::SetPriority(Priority priority)
    {
      mPriority = priority;
    }

    ITask::Priority ITask::GetPriority(void) const
    {
      return mPriority;
    }
    
    void ITask::SetName(std::string name)
    {
      mName = name;
    }
    
    std::string ITask::GetName(void) const
    {
      return mName;
    }


    //////////////////////////////////////////////////////////////////////////
    // TaskManager


    TaskManager::TaskManager(void)
    {
      mPool.SetThreadsPerCore(VIRTUS_CONFIG_CORE_THREAD_PER_CORE);
      mPool.SpawnThreads();
    }

    TaskManager::~TaskManager(void)
    {

    }

    void TaskManager::process(void)
    {
      u16 count = mPool.NumberOfIdleThreads();

      for (u16 i = 0; i < count && !mQueue.empty(); ++i)
      {
        mPool.AssignTask(*mQueue.begin());
        mQueue.erase(mQueue.begin());
      }

    }

    void TaskManager::ScheduleTask(shrdptr<ITask> process)
    {
      mQueue.insert(process);
    }


} // namespace Virtus