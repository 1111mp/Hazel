#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include <cstring>

#include "Hazel/Scene/Components.h"

/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Hazel
{

  SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &context)
  {
    SetContext(context);
  }

  void SceneHierarchyPanel::SetContext(const Ref<Scene> &context)
  {
    m_Context = context;
    m_SelectionContext = {};
  }

  void SceneHierarchyPanel::OnImGuiRender()
  {
    {
      ImGui::Begin("Scene Hierarchy");

      m_Context->m_Registry.each([&](auto entityID)
                                 {
          Entity entity{ entityID, m_Context.get() };

          DrawEntityNode(entity); });

      if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        m_SelectionContext = {};

      if (ImGui::BeginPopupContextWindow(0, 1, false))
      {
        if (ImGui::MenuItem("Create Entity"))
          m_Context->CreateEntity("Empty Entity");

        ImGui::EndPopup();
      }

      ImGui::End();
    }

    {
      ImGui::Begin("Properties");

      if (m_SelectionContext)
        DrawComponents(m_SelectionContext);

      ImGui::End();
    }
  }

  void SceneHierarchyPanel::SetSelectedentity(Entity entity)
  {
    m_SelectionContext = entity;
  }

  void SceneHierarchyPanel::DrawEntityNode(Entity entity)
  {
    auto &tag = entity.GetComponent<TagComponent>().Tag;

    ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, tag.c_str());
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
      m_SelectionContext = entity;
    }

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
      if (ImGui::MenuItem("Delete Entity"))
        entityDeleted = true;

      ImGui::EndPopup();
    }

    if (opened)
    {
      ImGui::BulletText(tag.c_str());
      ImGui::TreePop();
    }

    if (entityDeleted)
    {
      m_Context->DestroyEntity(entity);
      if (m_SelectionContext == entity)
        m_SelectionContext = {};
    }
  }

  static void DrawVec3Control(const std::string &label, glm::vec3 &values, float defaultVal = 0.0f, float labelWidth = 100.0f)
  {
    ImGuiIO &io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, labelWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize))
      values.x = defaultVal;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize))
      values.y = defaultVal;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize))
      values.z = defaultVal;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();
    ImGui::Columns(1);

    ImGui::PopID();
  }

  template <typename T, typename UIFunction>
  static void DrawComponent(const std::string &name, Entity entity, UIFunction uiFunction)
  {
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
    if (entity.HasComponent<T>())
    {
      auto &component = entity.GetComponent<T>();
      ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
      float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
      ImGui::Separator();
      bool open = ImGui::TreeNodeEx((void *)typeid(T).hash_code(), treeNodeFlags, name.c_str());
      ImGui::PopStyleVar();
      ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
      if (ImGui::Button("+", ImVec2{lineHeight, lineHeight}))
      {
        ImGui::OpenPopup("ComponentSettings");
      }

      bool removeComponent = false;
      if (ImGui::BeginPopup("ComponentSettings"))
      {
        if (ImGui::MenuItem("Remove component"))
          removeComponent = true;

        ImGui::EndPopup();
      }

      if (open)
      {
        uiFunction(component);
        ImGui::TreePop();
      }

      if (removeComponent)
        entity.RemoveComponent<T>();
    }
  }

  void SceneHierarchyPanel::DrawComponents(Entity entity)
  {
    if (entity.HasComponent<TagComponent>())
    {
      auto &tag = entity.GetComponent<TagComponent>().Tag;

      char buffer[256];
      memset(buffer, 0, sizeof(buffer));
      std::strncpy(buffer, tag.c_str(), sizeof(buffer));

      if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
      {
        tag = std::string(buffer);
      }
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(-1);

    if (ImGui::Button("Add Component"))
      ImGui::OpenPopup("Add Component");

    if (ImGui::BeginPopup("Add Component"))
    {
      if (ImGui::MenuItem("Camera"))
      {
        if (!m_SelectionContext.HasComponent<CameraComponent>())
          m_SelectionContext.AddComponent<CameraComponent>();
        else
          HZ_HAZEL_WARN("This entity already has the Camera Component!");
        ImGui::CloseCurrentPopup();
      }

      if (ImGui::MenuItem("Sprite Renderer"))
      {
        if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
          m_SelectionContext.AddComponent<SpriteRendererComponent>();
        else
          HZ_HAZEL_WARN("This entity already has the Sprite Renderer Component!");
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }

    ImGui::PopItemWidth();

    DrawComponent<TransformComponent>("Transform", entity, [](auto &component)
                                      {
        DrawVec3Control("Translation", component.Translation);

        glm::vec3 rotation = glm::degrees(component.Rotation);
        DrawVec3Control("Rotation", rotation);
        component.Rotation = glm::radians(rotation);

        DrawVec3Control("Scale", component.Scale, 1.0f); });

    DrawComponent<CameraComponent>("Camera", entity, [](auto &component)
                                   {
        auto& camera = component.Camera;

        ImGui::Checkbox("Primary", &component.Primary);

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

          ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
        } });

    DrawComponent<SpriteRendererComponent>("Sprite Renderre", entity, [](auto &component)
                                           { ImGui::ColorEdit4("Color", glm::value_ptr(component.Color)); });
  }

}
