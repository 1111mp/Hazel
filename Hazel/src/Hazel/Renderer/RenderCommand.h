#pragma once

#include "RendererAPI.h"

namespace Hazel {
	
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetColorClear(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}
		
		inline static void DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}
		
	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}
