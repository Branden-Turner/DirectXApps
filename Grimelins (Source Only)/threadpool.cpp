/*! ====================================================================================================================
  @file threadpool.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.24.2011

  @brief 
    Management and pooling of threads.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "threadpool.h"
#include "apputilities.h"
#include "appmachine.h"
#include "taskmanager.h"

namespace Grimelins
{
    //////////////////////////////////////////////////////////////////////////
    // Thread class

    Thread::Thread(void) : 
                            mCurrentTask(NULL), 
                            mThreadHandle(0), 
                            mThreadId(-1), 
                            mThreadName("Unnamed Virtus Worker"),
                            mThreadSpawned(false), 
                            mThreadSleeping(false)
    {
      // Start suspended thread
      spawnThread();
    }


    Thread::~Thread(void)
    {
      ErrorIf(!mThreadSleeping, 
"Destruction of thread before task finished. \n\
 It is the programmers' responsibility to assure Thread and assigned Task remain in scope while task is running!\n\
 IMPORTANT: Task will be killed without clean up now! \n\
 Resource LEAKS are probable. \n\
 Result of currently performed operation on that thread is UNDEFINED!");

      Application::KillThread(mThreadHandle);
      Application::CloseHandle(mThreadHandle);
    }

    u1 Thread::IsDone(void) const
    {
      return mThreadSleeping;
    }

    u1 Thread::IsBusy(void) const
    {
      return !mThreadSleeping;
    }

    void Thread::WaitForCompletion(void) const
    {
      while (mCurrentTask);
    }

    void Thread::Kill(void)
    {
      if (mThreadSpawned)
      {
        Application::KillThread(mThreadHandle);
        Application::CloseHandle(mThreadHandle);
      }

      mThreadSpawned = false;
      mThreadSleeping = false;
      mThreadHandle = 0;
      mCurrentTask = 0;

      // Respawn sleeping thread
      spawnThread();
    }

    void Thread::SetName(std::string name)
    {
      mThreadName = name;

      if (mThreadSpawned)
        Application::NameThread(mThreadId, mThreadName.c_str());
    }

    u1 Thread::AssignTask(shrdptr<ITask> task)
    {
      mMutex.Acquire();
      if (mThreadSpawned && !mThreadSleeping)
      {
        mMutex.Release();
        return false;
      }

      mCurrentTask = task;
      mThreadSleeping = false;
      
      Application::UnPauseThread(mThreadHandle);
      mMutex.Release();


      return true;
    }

    u32 _stdcall Thread::execution(void* thrd)
    {
      Thread* MyManager = RCAST(Thread*, thrd);

      // Wait for initial GO
      MyManager->mMutex.Acquire();
      MyManager->mMutex.Release();

      while (1)
      {
        ErrorIf(!MyManager->mCurrentTask, "Thread execution running even though no task is assigned.");
        
        // Execute Task
        MyManager->mCurrentTask->Execute();

        // Finish execution
        MyManager->mMutex.Acquire();
        MyManager->mThreadSleeping = true;
        MyManager->mCurrentTask = 0;
        Application::OSHANDLE thrd = MyManager->mThreadHandle;
        MyManager->mMutex.Release();

        Application::PauseThread(thrd);
      }

      return 0;
    }


    void Thread::spawnThread(void)
    {
      Assert(!mThreadSpawned);
      mThreadSpawned = true;
      mThreadSleeping = true;
      mThreadHandle = Application::MakeThread(execution, this, true, &mThreadId);

      SetName(mThreadName);
    }


    //////////////////////////////////////////////////////////////////////////
    // Thread Pool


    ThreadPool::ThreadPool(void) : mThrdsPerCore(0), mThreads(NULL), mNumThrds(0)
    {
      mNumCores = Application::GetNumberProcessors();
    }

    ThreadPool::~ThreadPool(void)
    {
      if (mThreads)
        UnloadThreads();
    }

    void ThreadPool::SetThreadsPerCore(u32 num)
    {
      mThrdsPerCore = num;
      mNumThrds = mThrdsPerCore * mNumCores;
    }

    void ThreadPool::SpawnThreads(void)
    {
      if (mThreads)
        UnloadThreads();

      if (mNumThrds == 0)
        return;

      mThreads = new Thread*[mNumThrds];
      char buff[1024];

      for (u32 i = 0 ; i < mNumThrds; ++i)
      {
        mThreads[i] = new Thread();
        sprintf(buff, "Pooled Task Worker Thread #%i", i);
        mThreads[i]->SetName(buff);
      }
    }

    void ThreadPool::UnloadThreads(void)
    {
      for (u32 i = 0; i < mNumThrds; ++i)
      {
        if (mThreads[i]->IsBusy())
        {
          Error("WARNING: Threadpool is unloading threads that are currently busy! This may result in undefined behavior.");
          mThreads[i]->Kill();
        }

        delete mThreads[i];
      }

      delete [] mThreads;
      mThreads = 0;
    }

    u1 ThreadPool::HasCapacity(void) const
    {
      if (mThreads)
      {
        for (u32 i = 0; i < mNumThrds; ++i)
          if (!mThreads[i]->IsBusy())
            return true;
      }

      return false;
    }

    u16 ThreadPool::NumberOfIdleThreads(void) const
    {
      u16 count = 0;

      if (mThreads)
      {
        for (u32 i = 0; i < mNumThrds; ++i)
          if (!mThreads[i]->IsBusy())
            ++count;
      }

      return count;
    }

    u1 ThreadPool::AssignTask(shrdptr<ITask> task)
    {
      if (mThreads)
      {
        for (u32 i = 0; i < mNumThrds; ++i)
        {
          if (!mThreads[i]->IsBusy())
            return mThreads[i]->AssignTask(task);
        }
      }

      return false;
    }

} // namespace Virtus