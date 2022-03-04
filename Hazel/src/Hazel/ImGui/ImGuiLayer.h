#pragma once

#include "Hazel/Core/Layer.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Renderer/GraphicsContext.h"

namespace Hazel {
  #define GLFW_HAS_GET_KEY_NAME (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwGetKeyName()

  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer(GraphicsContext* context);
    ImGuiLayer(GraphicsContext* context, const std::string& ini);
    ~ImGuiLayer();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& e) override;

    void Begin();
    void End();
		
		inline void BlockEvents(bool block) { m_BlockEvents = block; }
		
    void SetDarkThemeColors();
  private:
    GraphicsContext* m_GraphicsContext = nullptr;
    std::string m_IniPath = "imgui.ini";
		bool m_BlockEvents = true;
    float m_Time = 0.0f;
  };

}
