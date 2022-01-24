#include "Application.h"

#include <GLFW/glfw3.h>

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

Application::Application() {
	m_Window = std::unique_ptr<Window>(Window::Create());
	
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
}

Application::~Application() {}

void Application::OnEvent(Event& e)
{
	HZ_HAZEL_TRACE("{0}", e);
	
	EventDispatcher dispather(e);
	
	dispather.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
}

void Application::Run()
{
	while (m_Running) {
		glClearColor(0.4f, 0.3f, 0.4f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		m_Window->OnUpdate();
	}
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	
	return true;
};

}
