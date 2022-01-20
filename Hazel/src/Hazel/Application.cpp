#include "Application.h"

#include <iostream>

#include <GLFW/glfw3.h>

namespace Hazel
{

  Application::Application() {
    std::cout << glfwInit() << std::endl;
  }

  Application::~Application() {}
  
  void Application::Run()
  {
    while (true);
  }

}