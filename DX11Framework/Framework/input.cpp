#include "input.h"


Input::Input()
{
}


Input::Input(const Input& rhs)
{
}


Input::~Input()
{
}


void Input::Initialize()
{
  // All keys to up state
  for( unsigned i = 0; i < 256; ++i )
    m_keys[i] = false;

  return;
}


void Input::KeyDown(unsigned down)
{
  // Change key to down state
  if( down < 256 )
    m_keys[down] = true;
  return;
}


void Input::KeyUp(unsigned down)
{
  // Change key to up state
  if( down < 256 )
    m_keys[down] = false;
  return;
}


bool Input::IsKeyDown(unsigned key)
{
  // Return what state the key is in (pressed/not pressed).
  if( key < 256 )
    return m_keys[key];
  else
    return false;
}
