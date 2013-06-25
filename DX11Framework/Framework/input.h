#ifndef INPUT_H
#define INPUT_H

#include "system.h"
class Input : public System
{
  public:
    Input();
    ~Input();

    void Initialize();

    void KeyDown(unsigned);
    void KeyUp(unsigned);

    bool IsKeyDown(unsigned);

    static Input* Instance() { return m_instance; };

  private:
    bool m_keys[256];
    static Input* m_instance;
};

#endif // INPUT_H