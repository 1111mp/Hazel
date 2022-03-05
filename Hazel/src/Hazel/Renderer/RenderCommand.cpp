#include "RenderCommand.h"

#if defined(HZ_RENDERER_OPENGL)
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#else defined(HZ_RENDERER_VULKAN)
#include "Platform/Vulkan/VulkanRendererAPI.h"
#endif

namespace Hazel {

	#if defined(HZ_RENDERER_OPENGL)
	Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<OpenGLRendererAPI>();
	#else defined(HZ_RENDERER_VULKAN)
	Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<VulkanRendererAPI>();
	#endif
}
