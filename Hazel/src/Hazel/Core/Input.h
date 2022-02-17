#pragma once

#include "Core.h"
#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Hazel {

  class Input
  {
  public:
    static bool IsKeyPressed(KeyCode key) { return s_Instance->IsKeyPressedImpl(key); }

    static bool IsMouseButtonPressed(MouseCode button) { return s_Instance->IsMouseButtonPressedImpl(button); }
    static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }

  protected:
    virtual bool IsKeyPressedImpl(KeyCode key) = 0;

    virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
    virtual std::pair<float, float> GetMousePositionImpl() = 0;

    static Scope<Input> Create();
  private:
    static Scope<Input> s_Instance;
  };

}