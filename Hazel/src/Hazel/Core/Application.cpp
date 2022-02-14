#include "Application.h"

#include "Hazel/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace Hazel {

  Application* Application::s_Instance = nullptr;

  Application::Application()
  {
    HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    m_Window = Window::Create();
    m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));

    Renderer::Init();

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
  }

  Application::~Application() {}

  void Application::PushLayer(Layer* layer)
  {
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
  }

  void Application::PushOverlay(Layer* overlay)
  {
    m_LayerStack.PushOverlay(overlay);
    overlay->OnAttach();
  }

  void Application::OnEvent(Event& e)
  {
    EventDispatcher dispather(e);
    dispather.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));
    dispather.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
      // *it 就是迭代器it指向的元素 这里做 --it 运算 进入下一个迭代
      (*--it)->OnEvent(e);
      if (e.Handled)
        break;
    }
  }

  void Application::Run()
  {
    while (m_Running) {
      float time = (float)glfwGetTime();
      TimeStep timestep = time - m_LastFrameTime;
      m_LastFrameTime = time;

      if (!m_Minimized)
      {
        for (Layer* layer : m_LayerStack)
          layer->OnUpdate(timestep);
      }

      m_ImGuiLayer->Begin();
      for (Layer* layer : m_LayerStack)
        layer->OnImGuiRender();
      m_ImGuiLayer->End();

      m_Window->OnUpdate();
    }
  }

  bool Application::OnWindowClose(WindowCloseEvent& e)
  {
    m_Running = false;

    return true;
  };

  bool Application::OnWindowResize(WindowResizeEvent& e)
  {
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
      m_Minimized = true;
      return false;
    }

    m_Minimized = false;

    return true;
  };

}
