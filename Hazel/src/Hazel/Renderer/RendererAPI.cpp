#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Hazel {

	#if defined(HZ_RENDERER_OPENGL)
  RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
	#elif defined(HZ_RENDERER_VULKAN)
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;
	#endif

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
		case RendererAPI::API::Vulkan:  return CreateScope<VulkanRendererAPI>();
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
