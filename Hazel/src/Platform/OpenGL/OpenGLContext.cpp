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

    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    HZ_HAZEL_INFO("Maximum nr of vertex attributes supported: {0}", nrAttributes);
  }

  void OpenGLContext::SwapBuffers()
  {
    HZ_PROFILE_FUNCTION();

    glfwSwapBuffers(m_WindowHandle);
  }
}
