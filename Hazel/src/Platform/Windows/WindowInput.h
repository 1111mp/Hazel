#pragma once

#include "Hazel/Input.h"

namespace Hazel {

  class WindowInput : public Input
  {
  protected:
    virtual bool IsKeyPressedImpl(int keycode) override;

    virtual bool IsMouseButtonPressedImpl(int button) override;
    virtual std::pair<float, float> GetMousePositionImpl() override;
  };

}
