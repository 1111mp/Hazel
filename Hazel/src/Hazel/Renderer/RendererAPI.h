#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Hazel {

	class RendererAPI
	{
	public:
		enum class API
		{
      None = 0, OpenGL = 1, Vulkan = 2,
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		
		static API GetAPI() { return s_API; }
		static Scope<RendererAPI> Create();
	private:
		static API s_API;
	};

}
