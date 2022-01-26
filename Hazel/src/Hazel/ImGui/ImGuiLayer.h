#pragma once

#include "Hazel/Layer.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel {
  #define GLFW_HAS_GET_KEY_NAME (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwGetKeyName()

  class HazelExports ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer();
    ~ImGuiLayer();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& event) override;

    void Begin();
    void End();
		
		inline void BlockEvents(bool block) { m_BlockEvents = block; }
		
  private:
		bool m_BlockEvents = true;
    float m_Time = 0.0f;
  };

}
