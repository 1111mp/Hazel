#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel {

  class OpenGLContext : public GraphicsContext
  {
  public:
    OpenGLContext(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;
    virtual void Destory() override {};

    virtual void FrameRender(ImDrawData*) override {};
    virtual void FramePresent() override {};
    virtual void InitForVulkan() override {};
  private:
    GLFWwindow* m_WindowHandle;
  };
}
