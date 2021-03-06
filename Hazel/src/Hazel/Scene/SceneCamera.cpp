#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

  SceneCamera::SceneCamera()
  {
    RecalculateProjection();
  }

  void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
  {
    m_ProjectionType = ProjectionType::Orthographic;
    m_OrthographicSize = size;
    m_OrthographicNear = nearClip;
    m_OrthographicFar = farClip;

    RecalculateProjection();
  }

  void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
  {
    m_ProjectionType = ProjectionType::Perspective;
    m_PerspectiveFOV = fov;
    m_PerspectiveNear = nearClip;
    m_PerspectiveFar= farClip;

    RecalculateProjection();
  }

  void SceneCamera::SetViewportSize(float width, float height)
  {
    m_AspectRatio = width / height;
    RecalculateProjection();
  }

  void SceneCamera::RecalculateProjection()
  {
    if (m_ProjectionType == ProjectionType::Perspective)
    {
      m_Projection = glm::perspective(glm::radians(m_PerspectiveFOV), m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
    }
    else 
    {
      float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
      float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
      float orthoBottom = -m_OrthographicSize * 0.5f;
      float orthoTop = m_OrthographicSize * 0.5f;

      m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
    }
  }

}
