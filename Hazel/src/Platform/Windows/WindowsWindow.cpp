#include "WindowsWindow.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Hazel {

  static uint8_t s_GLFWWindowCount = 0;

  static void GLFWErrorCallback(int error_code, const char* description)
  {
    HZ_HAZEL_ERROR("GLFW ERROR ({0}): {1}", error_code, description);
  }

  WindowsWindow::WindowsWindow(const WindowProps& props)
  {
    HZ_PROFILE_FUNCTION();

    Init(props);
  }

  WindowsWindow::~WindowsWindow()
  {
    HZ_PROFILE_FUNCTION();

    Shutdown();
  }

  void WindowsWindow::Init(const WindowProps& props)
  {
    HZ_PROFILE_FUNCTION();

    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    HZ_HAZEL_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (s_GLFWWindowCount == 0)
    {
      HZ_PROFILE_SCOPE("glfwInit");

      HZ_HAZEL_INFO("Initializing GLFW");
      // TODO: glfwTerminate on system shutdown
      int success = glfwInit();
      HZ_CORE_ASSERT(success, "Could not initialize GLFW!");

      #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        //const char* glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
      #elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        //const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
      #else
        // GL 3.0 + GLSL 130
        //const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
      #endif

      glfwSetErrorCallback(GLFWErrorCallback);
    }

    {
      HZ_PROFILE_SCOPE("glfwCreateWindow");
      m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
      ++s_GLFWWindowCount;
    }

    m_Context = CreateScope<OpenGLContext>(m_Window);
    m_Context->Init();

    glfwSetWindowUserPointer(m_Window, &m_Data);
    SetVSync(true);

    // window resize event
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
      data.Width = width;
      data.Height = height;

      WindowResizeEvent event(width, height);
      data.EventCallback(event);
      });

    // window close event
    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
      WindowCloseEvent event;
      data.EventCallback(event);
      });

    // key pressede
    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      switch (action) {
      case GLFW_PRESS:
      {
        KeyPressedEvent event(static_cast<KeyCode>(key), 0);
        data.EventCallback(event);
        break;
      }
      case GLFW_RELEASE:
      {
        KeyReleasedEvent event(static_cast<KeyCode>(key));
        data.EventCallback(event);
        break;
      }
      case GLFW_REPEAT:
      {
        KeyPressedEvent event(static_cast<KeyCode>(key), 1);
        data.EventCallback(event);
        break;
      }
      }
      });

    glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      KeyTypedEvent event(static_cast<KeyCode>(keycode));
      data.EventCallback(event);
      });

    // mouse pressed
    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      switch (action) {
      case GLFW_PRESS:
      {
        MouseButtonPressedEvent event(static_cast<MouseCode>(button));
        data.EventCallback(event);
        break;
      }
      case GLFW_RELEASE:
      {
        MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
        data.EventCallback(event);
        break;
      }
      }
      });

    // scroll event
    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      MouseScrolledEvent event((float)xoffset, (float)yoffset);
      data.EventCallback(event);
      });

    // mouse moved
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      MouseMovedEvent event((float)xpos, (float)ypos);
      data.EventCallback(event);
      });
  }

  void WindowsWindow::Shutdown()
  {
    HZ_PROFILE_FUNCTION();

    glfwDestroyWindow(m_Window);
    --s_GLFWWindowCount;

    if (s_GLFWWindowCount == 0)
    {
      glfwTerminate();
    }
  }

  void WindowsWindow::OnUpdate()
  {
    HZ_PROFILE_FUNCTION();

    glfwPollEvents();
    m_Context->SwapBuffers();
  }

  void WindowsWindow::SetVSync(bool enabled)
  {
    HZ_PROFILE_FUNCTION();

    if (enabled)
      glfwSwapInterval(1);
    else
      glfwSwapInterval(0);

    m_Data.VSync = enabled;
  }

  bool WindowsWindow::IsVSync() const
  {
    return m_Data.VSync;
  }

}
