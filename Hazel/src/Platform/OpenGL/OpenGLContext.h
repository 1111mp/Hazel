#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel
{

  class OpenGLContext : public GraphicsContext
  {
  public:
    OpenGLContext(GLFWwindow *windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;
    virtual void Destory() override{};

    virtual void InitForVulkan() override{};
    virtual void DrawFrame() override{};
    virtual void SetFramebufferResized(bool resized) override{};

  private:
    GLFWwindow *m_WindowHandle;
  };
}
