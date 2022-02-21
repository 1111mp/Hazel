#pragma once

#include <Hazel.h>

namespace Hazel {

  class EditorLayer : public Layer
  {
  public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(TimeStep ts) override;
    virtual virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& e) override;
  private:
    OrthographicCameraController m_CameraController;

    Ref<Texture2D> m_CheckerboardTexture;
    Ref<Framebuffer> m_Framebuffer;

    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
  };

}