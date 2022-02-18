#include "Sandbox2D.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

Sandbox2D::Sandbox2D()
  : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
  HZ_PROFILE_FUNCTION();

  m_CheckerboardTexture = Hazel::Texture2D::Create(AssetsDir + "/assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
  HZ_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Hazel::TimeStep ts)
{
  HZ_PROFILE_FUNCTION();

  // Update
  m_CameraController.OnUpdate(ts);

  // Render
  Hazel::Renderer2D::ResetStats();
  {
    HZ_PROFILE_SCOPE("Renderer Prep");
    Hazel::RenderCommand::SetColorClear({ 0.1f, 0.1f, 0.1f, 1.0f });
    Hazel::RenderCommand::Clear();
  }

  {
    static float rotation = 0.0f;
    rotation += ts * 50.0f;

    HZ_PROFILE_SCOPE("Renderer Draw");
    Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
    Hazel::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
    Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
    Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
    Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
    Hazel::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20.0f);
    Hazel::Renderer2D::EndScene();
  }
}

void Sandbox2D::OnImGuiRender()
{
  HZ_PROFILE_FUNCTION();
  ImGui::Begin("Settings");

  auto stats = Hazel::Renderer2D::GetStats();
  ImGui::Text("Renderer2D Stats:");
  ImGui::Text("Draw Calls: %d", stats.DrawCalls);
  ImGui::Text("Quads: %d", stats.QuadCount);
  ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
  ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

  ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
  ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
  m_CameraController.OnEvent(e);
}
