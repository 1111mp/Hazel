#include "Hazel.h"

#include <iostream>

#include <GLFW/glfw3.h>

namespace Hazel
{

  Test::Test() {
    std::cout << glfwInit() << std::endl;
  }

  void Test::Print()
  {
    std::cout << "Hello" << std::endl;
  }
}