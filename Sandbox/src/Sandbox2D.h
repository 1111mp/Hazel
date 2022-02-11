#pragma once

#include <Hazel.h>

class Sandbox2D : public Hazel::Layer
{
public:
  Sandbox2D();
  virtual ~Sandbox2D() = default;

  virtual void OnAttach() override;
  virtual void OnDetach() override;

  virtual void OnUpdate(Hazel::TimeStep ts) override;
  virtual virtual void OnImGuiRender() override;
  virtual void OnEvent(Hazel::Event& e) override;
private:
  Hazel::OrthographicCameraController m_CameraController;

  Hazel::ShaderLibrary m_ShaderLibrary;
  Hazel::Ref<Hazel::Shader> m_Shader;
  Hazel::Ref<Hazel::VertexArray> m_SquareVA;
  Hazel::Ref<Hazel::Texture2D> m_Texture;
};