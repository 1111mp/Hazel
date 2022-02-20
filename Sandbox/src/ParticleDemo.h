#pragma once

#include "Hazel.h"

#include "ParticleSystem.h"

class ParticleDemo: public Hazel::Layer
{
public:
  ParticleDemo();
  ~ParticleDemo() = default;

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnUpdate(Hazel::TimeStep ts) override;
  virtual void OnImGuiRender() override;
  virtual void OnEvent(Hazel::Event& e) override;
private:
  Hazel::OrthographicCameraController m_CameraController;

  Hazel::Ref<Hazel::Texture2D> m_SpriteSheet;

  ParticleProps m_Particle;
  ParticleSystem m_ParticleSystem;

  uint32_t m_MapWidth, m_MapHeight;
  std::unordered_map<char, Hazel::Ref<Hazel::SubTexture2D>> s_TextureMap;
};
