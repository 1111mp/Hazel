#pragma once

#include <glm/glm.hpp>

#include "Base.h"
#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Hazel {

  class Input
  {
  public:

    static bool IsKeyPressed(KeyCode key);

    static bool IsMouseButtonPressed(MouseCode button);
    static glm::vec2 GetMousePosition();
  };

}