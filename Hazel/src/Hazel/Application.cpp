#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Hazel
{

Application::Application() {
	
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
	
	
	GLFWwindow* window;
	
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	
	if (glewInit() != GLEW_OK)
		std::cout << "GLEW Error\n";
	
	std::cout << glGetString(GL_VERSION) << "\n";
	
	GLint nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		
		/* Poll for and process events */
		glfwPollEvents();
	}
	
	glfwTerminate();
}

}
