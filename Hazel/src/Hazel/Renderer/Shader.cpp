#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

  Ref<Shader> Shader::Create(const std::string& vertextSrc, const std::string& fragmentSrc)
  {
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return std::make_shared<OpenGLShader>(vertextSrc, fragmentSrc);
    }

    HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
  }

}

