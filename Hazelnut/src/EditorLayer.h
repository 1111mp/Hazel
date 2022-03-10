#pragma once

#include <Hazel.h>

#include "Panels/SceneHierarchyPanel.h"

namespace Hazel
{

  class EditorLayer : public Layer
  {
  public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(TimeStep ts) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event &e) override;

  private:
    bool OnKeyPressed(KeyPressedEvent &e);

    void NewScene();
    void OpenScene();
    void SaveSceneAs();

  private:
    OrthographicCameraController m_CameraController;

    Ref<Framebuffer> m_Framebuffer;

    Ref<Scene> m_ActiveScene;
    Entity m_SquareEntity;
    Entity m_CameraEntity;

    Ref<Texture2D> m_CheckerboardTexture;

    bool m_ViewportFocused = false, m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = {0.0f, 0.0f};

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};

    int m_GizmoType = -1;

    // Panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
  };

}