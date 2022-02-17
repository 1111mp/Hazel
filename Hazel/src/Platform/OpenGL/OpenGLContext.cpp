#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Hazel {

  OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
    : m_WindowHandle(windowHandle)
  {
    HZ_CORE_ASSERT(windowHandle, "Window Handle is null!");
  }

  void OpenGLContext::Init()
  {
    HZ_PROFILE_FUNCTION();

    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    HZ_CORE_ASSERT(status, "Failed to initialize glad!");

    HZ_HAZEL_INFO(glGetString(GL_VERSION));
  }

  void OpenGLContext::SwapBuffers()
  {
    HZ_PROFILE_FUNCTION();

    glfwSwapBuffers(m_WindowHandle);
  }
}
