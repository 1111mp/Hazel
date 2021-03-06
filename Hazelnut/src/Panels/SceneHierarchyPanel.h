#pragma once

#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel
{

  class SceneHierarchyPanel
  {
  public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene> &context);

    void SetContext(const Ref<Scene> &context);

    void OnImGuiRender();

    Entity GetSelectedEntity() { return m_SelectionContext; }
    void SetSelectedentity(Entity entity);

  private:
    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);

  private:
    Ref<Scene> m_Context;
    Entity m_SelectionContext;
  };

}
