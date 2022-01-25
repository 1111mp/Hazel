#pragma once

#include "Hazel/Layer.h"

namespace Hazel {

  class HazelExports ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer();
    ~ImGuiLayer();

    void OnAttach();
    void OnDetach();
    void OnUpdate();
    void OnEvent(Event& event);

  private:
    float m_Time = 0.0f;
  };

}
