#include "Scene.h"

#include "Components.h"
#include "Hazel/Renderer/Renderer2D.h"

namespace Hazel {

  Scene::Scene()
  {
  }

  Scene::~Scene()
  {
  }

  entt::entity Scene::CreateEntity()
  {
    return m_Registry.create();
  }

  void Scene::OnUpdate(TimeStep ts)
  {
    auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (auto entity : group)
    {
      auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
    
      Renderer2D::DrawQuad(transform, sprite.Color);
    }
  }

}
