#pragma once

namespace Hazel {

  class Shader
  {
  public:
    Shader(const std::string& vertextSrc, const std::string& fragmentSrc);
    ~Shader();

    void Bind() const;
    void UnBind() const;

  private:
    uint32_t m_RendererID;
  };

}
