#include "Application.h"

#include <GLFW/glfw3.h>

#include "Events/ApplicationEvent.h"
#include "Log.h"

namespace Hazel
{

  Application::Application() {
    std::cout << glfwInit() << std::endl;
  }

  Application::~Application() {}
  
  void Application::Run()
  {
    WindowResizeEvent e(1280, 720);

    if (e.IsInCategory(EventCategoryApplication))
    {
      HZ_TRACE(e);
    }

    if (e.IsInCategory(EventCategoryInput))
    {
      HZ_TRACE(e);
    }

    while (true);
  }

}