#include "Application.h"

#include <glad/glad.h>

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

  Application* Application::s_Instance = nullptr;

  Application::Application() {
    HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

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
    dispather.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

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
      glClearColor(0.4f, 0.3f, 0.4f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      for (Layer* layer : m_LayerStack)
        layer->OnUpdate();

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

}
