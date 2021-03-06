#pragma once

#include <entt.hpp>

#include "Hazel/Core/TimeStep.h"
#include "Hazel/Renderer/EditorCamera.h"

namespace Hazel
{

  class Entity;

  class Scene
  {
  public:
    Scene();
    ~Scene();

    Entity CreateEntity(const std::string &name = "");
    void DestroyEntity(Entity entity);

    void OnUpdateRuntime(TimeStep ts);
    void OnUpdateEditor(TimeStep ts, const EditorCamera &camera);
    void OnViewportResize(float width, float height);

    Entity GetPrimaryCameraEntity();

  private:
    template <typename T>
    void OnComponentAdded(Entity entity, T &component);

  private:
    entt::registry m_Registry;
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    friend class Entity;
    friend class SceneSerializer;
    friend class SceneHierarchyPanel;
  };
}
