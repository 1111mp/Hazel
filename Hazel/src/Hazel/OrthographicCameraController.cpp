#include "OrthographicCameraController.h"

#include "Input.h"
#include "KeyCodes.h"

namespace Hazel {

  OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
    : m_AspectRatio(aspectRatio), m_LastAspectRatio(aspectRatio), 
    m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
  {
  }

  void OrthographicCameraController::OnUpdate(TimeStep ts)
  {
    if (Input::IsKeyPressed(HZ_KEY_A))
      m_CameraPosiition.x -= m_CameraTranslationSpeed * ts;
    else if (Input::IsKeyPressed(HZ_KEY_D))
      m_CameraPosiition.x += m_CameraTranslationSpeed * ts;

    if (Input::IsKeyPressed(HZ_KEY_W))
      m_CameraPosiition.y += m_CameraTranslationSpeed * ts;
    else if (Input::IsKeyPressed(HZ_KEY_S))
      m_CameraPosiition.y -= m_CameraTranslationSpeed * ts;

    if (m_Rotation)
    {
      if (Input::IsKeyPressed(HZ_KEY_Q))
        m_CameraRotation += m_CameraRotationSpeed * ts;
      else if (Input::IsKeyPressed(HZ_KEY_E))
        m_CameraRotation -= m_CameraRotationSpeed * ts;

      m_Camera.SetRotation(m_CameraRotation);
    }

    m_Camera.SetPosition(m_CameraPosiition);

    m_CameraTranslationSpeed = m_ZoomLevel;
  }

  void OrthographicCameraController::OnEvent(Event& e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
  }

  bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
  {
    m_ZoomLevel -= e.GetYOffset() * 0.25f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    return false;
  }

  bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
  {
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
      m_AspectRatio = m_LastAspectRatio;
    }
    else
    {
      m_AspectRatio = m_LastAspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
    }
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    return false;
  }

}
