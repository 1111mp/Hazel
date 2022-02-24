#pragma once

#include "Window.h"
#include "LayerStack.h"
#include "TimeStep.h"

#include "Hazel/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Hazel
{

  class HazelExports Application
  {
  public:
    Application(const std::string& name = "Hazel App", const std::string& ini = "imgui.ini");
    virtual ~Application();
		
		void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);

    Window& GetWindow() { return *m_Window; }

    void Close();

    ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

    static Application& Get() { return *s_Instance; }
	private:
    void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);		
	private:
    Scope<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
    bool m_Minimized = false;
    LayerStack m_LayerStack;
		
		float m_LastFrameTime = 0.0f;
  private:
    static Application* s_Instance;
    friend int ::main(int argc, char** argv);
  };

  // To be defined in CLIENT
  Application* CreateApplication();
}
