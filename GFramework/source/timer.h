#ifndef TIMER_H
#define TIMER_H


namespace cs350
{
  class Timer
  {
    public:
      Timer();
      ~Timer();

      //in seconds
      float getGameTime() const;
      float getTimeElapsed() const;

      void reset();
      void start();
      void stop();
      void tick();

    private:
      double secondsPerCount;
      double dt;

      __int64 baseTime;
      __int64 pausedTime;
      __int64 stopTime;
      __int64 prevTime;
      __int64 currTime;

      bool stopped;
  };
}

#endif