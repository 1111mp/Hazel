#include "Application.h"

#include "Hazel/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace Hazel {

  Application* Application::s_Instance = nullptr;

  Application::Application()
  {
    HZ_PROFILE_FUNCTION();

    HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    m_Window = Window::Create();
    m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));

    Renderer::Init();

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
  }

  Application::~Application()
  {
    HZ_PROFILE_FUNCTION();

    Renderer::Shutdown();
  }

  void Application::PushLayer(Layer* layer)
  {
    HZ_PROFILE_FUNCTION();

    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
  }

  void Application::PushOverlay(Layer* overlay)
  {
    HZ_PROFILE_FUNCTION();

    m_LayerStack.PushOverlay(overlay);
    overlay->OnAttach();
  }

  void Application::OnEvent(Event& e)
  {
    HZ_PROFILE_FUNCTION();

    EventDispatcher dispather(e);
    dispather.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));
    dispather.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
    {
      // *it 就是迭代器it指向的元素 这里做 --it 运算 进入下一个迭代
      if (e.Handled)
        break;
      (*it)->OnEvent(e);
    }
  }

  void Application::Run()
  {
    HZ_PROFILE_FUNCTION();

    while (m_Running) {
      float time = (float)glfwGetTime();
      TimeStep timestep = time - m_LastFrameTime;
      m_LastFrameTime = time;

      if (!m_Minimized)
      {
        {
          HZ_PROFILE_SCOPE("LayerStack OnUpdate");

          for (Layer* layer : m_LayerStack)
            layer->OnUpdate(timestep);
        }

        m_ImGuiLayer->Begin();

        {
          HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

          for (Layer* layer : m_LayerStack)
            layer->OnImGuiRender();
        }

        m_ImGuiLayer->End();
      }

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
    HZ_PROFILE_FUNCTION();

    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
      m_Minimized = true;
      return false;
    }

    m_Minimized = false;

    return true;
  };

}
