/*! ====================================================================================================================
  @file threadpool.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.24.2011

  @brief 
    Pooling of threads to optimize task performance.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_THREADPOOL_H
#define VIRTUS_THREADPOOL_H
#include "appdat.h"
#include "appmutex.h"

namespace Grimelins
{

    class ITask; 


    class Thread
    {
    public:
      Thread(void);
      ~Thread(void);

      u1 AssignTask(shrdptr<ITask> task);
      u1 IsDone(void) const;
      u1 IsBusy(void) const;
      void WaitForCompletion(void) const;
      void Kill(void);
      void SetName(std::string name);

    private:
      static u32 _stdcall execution(void* thrd);
      void spawnThread(void);

    private: // Deny copies
      Thread(Thread const& rhs);
      Thread& operator=(Thread const& rhs);

    private:
      shrdptr<ITask> mCurrentTask;

      Application::OSHANDLE mThreadHandle;
      u32 mThreadId;
      std::string mThreadName;
      u1 mThreadSpawned;
      u1 mThreadSleeping;

      mutable Mutex mMutex;
    };

    class ThreadPool
    {
    public:
      ThreadPool(void);
      ~ThreadPool(void);

    public:
      void SetThreadsPerCore(u32 num);
      void SpawnThreads(void);
      void UnloadThreads(void);
      u1 HasCapacity(void) const;
      u16 NumberOfIdleThreads(void) const;
      u1 AssignTask(shrdptr<ITask>);

    private: // Deny copies
      ThreadPool(ThreadPool const& rhs);
      ThreadPool& operator=(ThreadPool const& rhs);

    private:
      u32 mThrdsPerCore;  // Number of Threads Per Logical Core
      u32 mNumCores;      // Number of Cores
      u32 mNumThrds;      // Number of Threads in pool
      Thread** mThreads;  // Array of Threads
    };

} // namespace Virtus

#endif // Include Guard