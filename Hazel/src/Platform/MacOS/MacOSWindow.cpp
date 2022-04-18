#include "MacOSWindow.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel
{

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error_code, const char *description)
	{
		HZ_HAZEL_ERROR("GLFW ERROR ({0}): {1}", error_code, description);
	}

	MacOSWindow::MacOSWindow(const WindowProps &props)
	{
		HZ_PROFILE_FUNCTION();

		Init(props);
	}

	MacOSWindow::~MacOSWindow()
	{
		HZ_PROFILE_FUNCTION();

		Shutdown();
	}

	void MacOSWindow::Init(const WindowProps &props)
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

#if defined(HZ_RENDERER_OPENGL)
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#elif defined(HZ_RENDERER_VULKAN)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			HZ_PROFILE_SCOPE("glfwCreateWindow");
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}

		m_Context = GraphicsContext::Create(m_Window);
		m_Data.m_Context = m_Context.get();
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
																	 {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.m_Context->SetFramebufferResized(true); });

		// window resize event
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
															{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event); });

		// window close event
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
															 {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event); });

		// key pressede
		glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
											 {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			} });

		glfwSetCharCallback(m_Window, [](GLFWwindow *window, unsigned int keycode)
												{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event); });

		// mouse pressed
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods)
															 {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			} });

		// scroll event
		glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xoffset, double yoffset)
													{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xoffset, (float)yoffset);
			data.EventCallback(event); });

		// mouse moved
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos, double ypos)
														 {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xpos, (float)ypos);
			data.EventCallback(event); });
	}

	void MacOSWindow::Shutdown()
	{
		HZ_PROFILE_FUNCTION();

		m_Context->Destory();

		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void MacOSWindow::OnUpdate()
	{
		HZ_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void MacOSWindow::SetVSync(bool enabled)
	{
		HZ_PROFILE_FUNCTION();

#ifdef HZ_RENDERER_OPENGL
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
#endif

		m_Data.VSync = enabled;
	}

	bool MacOSWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}
