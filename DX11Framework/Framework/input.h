#ifndef INPUT_H
#define INPUT_H

class Input
{
  public:
    Input();
    Input(const Input&);
    ~Input();

    void Initialize();

    void KeyDown(unsigned);
    void KeyUp(unsigned);

    bool IsKeyDown(unsigned);

  private:
    bool m_keys[256];
};

#endif // INPUT_H