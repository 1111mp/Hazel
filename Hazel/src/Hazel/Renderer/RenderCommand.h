#pragma once

#include "RendererAPI.h"

namespace Hazel {
	
	class RenderCommand
	{
	public:
		static void Init()
		{
			s_RendererAPI->Init();
		}

		static void SetColorClear(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		
		static void Clear()
		{
			s_RendererAPI->Clear();
		}
		
		static void DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}
		
	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}
