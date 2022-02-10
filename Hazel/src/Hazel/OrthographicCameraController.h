#pragma once

#include "Renderer/OrthographicCamera.h"
#include "Core/TimeStep.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

namespace Hazel {

  class OrthographicCameraController
  {
  public:
    OrthographicCameraController(float aspectRatio, bool rotation = false);

    void OnUpdate(TimeStep ts);
    void OnEvent(Event& e);

    OrthographicCamera& GetCamera() { return m_Camera; }
    const OrthographicCamera& GetCamera() const { return m_Camera; }
  private:
    bool OnMouseScrolled(MouseScrolledEvent& e);
    bool OnWindowResized(WindowResizeEvent& e);
  private:
    float m_AspectRatio;
    float m_LastAspectRatio;
    float m_ZoomLevel = 1.0f;
    OrthographicCamera m_Camera;

    bool m_Rotation;

    glm::vec3 m_CameraPosiition = { 0.0f, 0.0f, 0.0f };
    float m_CameraRotation = 0.0f;
    float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
  };

}
