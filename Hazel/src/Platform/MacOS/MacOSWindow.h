
#pragma once

#include "Hazel/Core/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hazel
{

	class MacOSWindow : public Window
	{
	public:
		MacOSWindow(const WindowProps &props);
		virtual ~MacOSWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn &callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void *GetNativeWindow() const override { return m_Window; }
		virtual GraphicsContext *GetRendererContext() const override { return m_Context.get(); }

	private:
		virtual void Init(const WindowProps &props);
		virtual void Shutdown();

	private:
		GLFWwindow *m_Window;
		Scope<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
			GraphicsContext *m_Context;
		};

		WindowData m_Data;
	};

}
