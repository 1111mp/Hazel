#include "ParticleDemo.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles =
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWDDDDDDDWWWWWWWWWW"
"WWWWDDDDDDDDDDDDWWWWWWWW"
"WWWDDDDDDDDDDDDDDDDWWWWW"
"WWDDDDDDDDDDDDDDDDDDDWWW"
"WDDDDDWWWDDDDDDDDDDDDDWW"
"WWDDDDWWWDDDDDDDDDDDDWWW"
"WWWDDDDDDDDDDDDDDDDDWWWW"
"WWWWDDDDDDDDDDDDDDDWWWWW"
"WWWWWDDDDDDDDDDDDDWWWWWW"
"WWWWWWDDDDDDDDDDWWWWWWWW"
"WWWWWWWDDDDDDDWWWWWWWWWW"
"WWWWWWWWDDDDWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW";

ParticleDemo::ParticleDemo()
  : Layer("ParticleDemo"), m_CameraController(1280.0f / 720.0f)
{
}

void ParticleDemo::OnAttach()
{
  m_SpriteSheet = Hazel::Texture2D::Create(AssetsDir + "/assets/game/textures/RPGpack_sheet_2X.png");

  m_MapWidth = s_MapWidth;
  m_MapHeight = strlen(s_MapTiles) / s_MapWidth;

  s_TextureMap['D'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6, 11 }, { 128, 128 });
  s_TextureMap['W'] = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 });

  m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
  m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
  m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
  m_Particle.LifeTime = 1.0f;
  m_Particle.Velocity = { 0.0f, 0.0f };
  m_Particle.VelocityVariation = { 3.0f, 1.0f };
  m_Particle.Position = { 0.0f, 0.0f };

  m_CameraController.SetZoomLevel(5.0f);
}

void ParticleDemo::OnDetach()
{
}

void ParticleDemo::OnUpdate(Hazel::TimeStep ts)
{
  // Update
  m_CameraController.OnUpdate(ts);

  // Render
  Hazel::Renderer2D::ResetStats();
  {
    HZ_PROFILE_SCOPE("Renderer Prep");
    Hazel::RenderCommand::SetColorClear({ 0.1f, 0.1f, 0.1f, 1.0f });
    Hazel::RenderCommand::Clear();
  }

  if (Hazel::Input::IsMouseButtonPressed(Hazel::Mouse::ButtonLeft))
  {
    auto mousePosition = Hazel::Input::GetMousePosition();
    auto width = Hazel::Application::Get().GetWindow().GetWidth();
    auto height = Hazel::Application::Get().GetWindow().GetHeight();

    auto bounds = m_CameraController.GetBounds();
    auto pos = m_CameraController.GetCamera().GetPosition();
    mousePosition.x = (mousePosition.x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
    mousePosition.y = bounds.GetHeight() * 0.5f - (mousePosition.y / height) * bounds.GetHeight();
    m_Particle.Position = { mousePosition.x + pos.x, mousePosition.y + pos.y };
    for (int i = 0; i < 5; i++)
      m_ParticleSystem.Emit(m_Particle);
  }

  {
    Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

    for (uint32_t y = 0; y < m_MapHeight; y++)
    {
      for (uint32_t x = 0; x < m_MapWidth; x++)
      {
        char tileType = s_MapTiles[x + y * m_MapWidth];
        Hazel::Ref<Hazel::SubTexture2D> texture;
        if (s_TextureMap.find(tileType) != s_TextureMap.end())
          texture = s_TextureMap[tileType];
        else
          //texture = m_TextureStairs;

        Hazel::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, m_MapHeight - y - m_MapHeight / 2.0f, 0.0f }, { 1.0f, 1.0f }, texture);
      }
    }

    Hazel::Renderer2D::EndScene();
  }

  m_ParticleSystem.OnUpdate(ts);
  m_ParticleSystem.OnRender(m_CameraController.GetCamera());
}

void ParticleDemo::OnImGuiRender()
{
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
  ImGui::End();

  ImGui::End();
}

void ParticleDemo::OnEvent(Hazel::Event& e)
{
  m_CameraController.OnEvent(e);
}
