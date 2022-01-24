#include "Application.h"

#include <glad/glad.h>

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

  Application::Application() {
    m_Window = std::unique_ptr<Window>(Window::Create());

    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
  }

  Application::~Application() {}

  void Application::PushLayer(Layer* layer)
  {
    m_LayerStack.PushLayer(layer);
  }

  void Application::PushOverlay(Layer* overlay)
  {
    m_LayerStack.PushOverlay(overlay);
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

      m_Window->OnUpdate();
    }
  }

  bool Application::OnWindowClose(WindowCloseEvent& e)
  {
    m_Running = false;

    return true;
  };

}
