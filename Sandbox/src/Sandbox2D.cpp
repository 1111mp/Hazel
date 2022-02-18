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

  m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
  m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
  m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
  m_Particle.LifeTime = 1.0f;
  m_Particle.Velocity = { 0.0f, 0.0f };
  m_Particle.VelocityVariation = { 3.0f, 1.0f };
  m_Particle.Position = { 0.0f, 0.0f };
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
    Hazel::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(-45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
    Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
    Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
    Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
    Hazel::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, glm::radians(rotation), m_CheckerboardTexture, 20.0f);
    Hazel::Renderer2D::EndScene();
  }

  if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
  {
    auto [x, y] = Hazel::Input::GetMousePosition();
    auto width = Hazel::Application::Get().GetWindow().GetWidth();
    auto height = Hazel::Application::Get().GetWindow().GetHeight();

    auto bounds = m_CameraController.GetBounds();
    auto pos = m_CameraController.GetCamera().GetPosition();
    x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
    y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
    m_Particle.Position = { x + pos.x, y + pos.y };
    for (int i = 0; i < 5; i++)
      m_ParticleSystem.Emit(m_Particle);
  }

  m_ParticleSystem.OnUpdate(ts);
  m_ParticleSystem.OnRender(m_CameraController.GetCamera());
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

  ImGui::ColorEdit4("Birth Color", glm::value_ptr(m_Particle.ColorBegin));
  ImGui::ColorEdit4("Death Color", glm::value_ptr(m_Particle.ColorEnd));
  ImGui::DragFloat("Life Time", &m_Particle.LifeTime, 0.1f, 0.0f, 1000.0f);
  ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
  ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
  m_CameraController.OnEvent(e);
}
