#include "ExampleLayer.h"

ExampleLayer::ExampleLayer()
  : Layer("Ecample"), m_CameraController(1280.0f / 720.0f, true)
{
  float squareVertices[5 * 4] = {
    -0.75f, -0.75f, 0.0f, 0.0f, 0.0f,
     0.75f, -0.75f, 0.0f, 1.0f, 0.0f,
     0.75f,  0.75f, 0.0f, 1.0f, 1.0f,
    -0.75f,  0.75f, 0.0f, 0.0f, 1.0f
  };

  Hazel::Ref<Hazel::VertexBuffer> square_VB;
  square_VB = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
  square_VB->SetLayout({
    { Hazel::ShaderDataType::Float3, "a_Position" },
    { Hazel::ShaderDataType::Float2, "a_TexCoord" },
    });

  m_SquareVA = Hazel::VertexArray::Create();
  m_SquareVA->AddVertexBuffer(square_VB);

  unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
  Hazel::Ref<Hazel::IndexBuffer> square_IB;
  square_IB = Hazel::IndexBuffer::Create(squareIndices, 6);
  m_SquareVA->SetIndexBuffer(square_IB);

  auto textureShader = m_ShaderLibrary.Load(AssetsDir + "/assets/shaders/Texture.glsl");


  m_Texture = Hazel::Texture2D::Create(AssetsDir + "/assets/textures/Checkerboard.png");
  m_ChernoLogoTexture = Hazel::Texture2D::Create(AssetsDir + "/assets/textures/ChernoLogo.png");

  textureShader->SetInt("u_Texture", 0);
}

void ExampleLayer::OnAttach()
{
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate(Hazel::TimeStep ts)
{
  m_CameraController.OnUpdate(ts);

  Hazel::RenderCommand::SetColorClear({ 0.1f, 0.1f, 0.1f, 1.0f });
  Hazel::RenderCommand::Clear();

  Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

  auto textureShader = m_ShaderLibrary.Get("Texture");

  m_Texture->Bind();
  Hazel::Renderer::Submit(textureShader, m_SquareVA);

  m_ChernoLogoTexture->Bind();
  Hazel::Renderer::Submit(textureShader, m_SquareVA);

  Hazel::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender()
{
}

void ExampleLayer::OnEvent(Hazel::Event& e)
{
  m_CameraController.OnEvent(e);
}