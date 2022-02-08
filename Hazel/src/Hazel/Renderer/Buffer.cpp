#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Hazel {

  /////////////////////////////////////////////////////////////////////////////
  // VertexBuffer Create //////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
  {
    switch (Renderer::GetAPI())
    {
			case RendererAPI::API::None:
      HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
			case RendererAPI::API::OpenGL:
      return std::make_shared<OpenGLVertexBuffer>(vertices, size);
    }

    HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
  }

  /////////////////////////////////////////////////////////////////////////////
  // IndexBuffer Create ///////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
  {
    switch (Renderer::GetAPI())
    {
			case RendererAPI::API::None:
      HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
			case RendererAPI::API::OpenGL:
      return std::make_shared<OpenGLIndexBuffer>(indices, count);
    }

    HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
  }

}
