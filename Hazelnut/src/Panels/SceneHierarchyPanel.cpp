#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Scene/Components.h"

namespace Hazel {

  SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
  {
    SetContext(context);
  }

  void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
  {
    m_Context = context;
  }

  void SceneHierarchyPanel::OnImGuiRender()
  {
    {
      ImGui::Begin("Scene Hierarchy");

      m_Context->m_Registry.each([&](auto entityID)
        {
          Entity entity{ entityID, m_Context.get() };

          DrawEntityNode(entity);
        }
      );

      if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        m_SelectionContext = {};

      ImGui::End();
    }

    {
      ImGui::Begin("Properties");

      if (m_SelectionContext)
        DrawComponents(m_SelectionContext);

      ImGui::End();
    }
  }

  void SceneHierarchyPanel::DrawEntityNode(Entity entity)
  {
    auto& tag = entity.GetComponent<TagComponent>().Tag;

    ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
      m_SelectionContext = entity;
    }

    if (opened)
    {
      ImGui::BulletText("Blah blah\nBlah Blah");
      ImGui::TreePop();
    }
  }

  void SceneHierarchyPanel::DrawComponents(Entity entity)
  {
    if (entity.HasComponent<TagComponent>())
    {
      auto& tag = entity.GetComponent<TagComponent>().Tag;

      char buffer[256];
      memset(buffer, 0, sizeof(buffer));
      strcpy_s(buffer, sizeof(buffer), tag.c_str());

      if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
      {
        tag = std::string(buffer);
      }
    }

    if (entity.HasComponent<TransformComponent>())
    {
      ImGui::Separator();
      if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
      {
        auto& transform = entity.GetComponent<TransformComponent>().Transform;
        ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);

        ImGui::TreePop();
      }      
    }

    if (entity.HasComponent<CameraComponent>())
    {
      ImGui::Separator();
      if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
      {
        auto& cameraComponent = entity.GetComponent<CameraComponent>();
        auto& camera = cameraComponent.Camera;

        ImGui::Checkbox("Primary", &cameraComponent.Primary);

        const char* proTypes[] = { "Perspective", "Orthographic" };
        const char* currentProType = proTypes[static_cast<int>(camera.GetProjectionType())];
        if (ImGui::BeginCombo("Projection", currentProType))
        {
          for (int i = 0; i < IM_ARRAYSIZE(proTypes); i++)
          {
            const bool is_selected = (currentProType == proTypes[i]);
            if (ImGui::Selectable(proTypes[i], is_selected))
            {
              currentProType = proTypes[i];
              camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
              ImGui::SetItemDefaultFocus();
          }

          ImGui::EndCombo();
        }

        if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
        {
          float fov = camera.GetPerspectiveFOV();
          if (ImGui::DragFloat("FOV", &fov))
            camera.SetPerspectiveFOV(fov);

          float perspectiveNear = camera.GetPerspectiveNear();
          if (ImGui::DragFloat("Near", &perspectiveNear))
            camera.SetPerspectiveNear(perspectiveNear);

          float perspectiveFar = camera.GetPerspectiveFar();
          if (ImGui::DragFloat("Far", &perspectiveFar))
            camera.SetPerspectiveFar(perspectiveFar);
        }

        if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
        {
          float orthoSize = camera.GetOrthographicSize();
          if (ImGui::DragFloat("Size", &orthoSize))
            camera.SetOrthographicSize(orthoSize);

          float orthoNear = camera.GetOrthographicNear();
          if (ImGui::DragFloat("Near", &orthoNear))
            camera.SetOrthographicNear(orthoNear);

          float orthoFar = camera.GetOrthographicFar();
          if (ImGui::DragFloat("Far", &orthoFar))
            camera.SetOrthographicFar(orthoFar);

          ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
        }

        ImGui::TreePop();
      }
    }

    if (entity.HasComponent<SpriteRendererComponent>())
    {
      ImGui::Separator();
      if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
      {
        auto& src = entity.GetComponent<SpriteRendererComponent>();
        ImGui::ColorEdit4("Color", glm::value_ptr(src.Color));

        ImGui::TreePop();
      }
    }
  }

}