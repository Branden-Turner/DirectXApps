#ifndef SYSTEM_H
#define SYSTEM_H

class System
{
  public:
    virtual ~System() {};

    virtual void Initialize() {};
    virtual void Shutdown() {};
    virtual bool Update() { return true; };
};

#endif // SYSTEM_H
