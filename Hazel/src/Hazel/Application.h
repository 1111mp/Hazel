#pragma once

#include "Window.h"
#include "LayerStack.h"
#include "Core/TimeStep.h"

#include "Hazel/ImGui/ImGuiLayer.h"

namespace Hazel
{

  class HazelExports Application
  {
  public:
    Application();
    virtual ~Application();

    void Run();
		
		void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);

    inline Window& GetWindow() { return *m_Window; }
    inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		
	private:
    Scope<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
    LayerStack m_LayerStack;
		
		float m_LastFrameTime = 0.0f;
  private:
    static Application* s_Instance;
  };

  // To be defined in CLIENT
  Application* CreateApplication();
}
