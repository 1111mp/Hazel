#pragma once

#include <imgui.h>

namespace Hazel {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
    virtual void Destory() = 0;

		virtual void FrameRender(ImDrawData* draw_data) = 0;
    virtual void FramePresent() = 0;
		virtual void InitForVulkan() = 0;

		static Scope<GraphicsContext> Create(void* window);
	};

}