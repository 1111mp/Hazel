#include <Hazel.h>

#include "imgui.h"

class ExampleLayer : public Hazel::Layer
{
public:
  ExampleLayer()
    : Layer("Ecample"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosiition(0.0f)
  {
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		Hazel::Ref<Hazel::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));

		Hazel::BufferLayout layout = {
			{ Hazel::ShaderDataType::Float3, "a_Position" },
			{ Hazel::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		
		m_VertexArray.reset(Hazel::VertexArray::Create());
		m_VertexArray->AddVertexBuffer(vertexBuffer);
		

		unsigned int indices[3] = { 0, 1, 2 };
		Hazel::Ref<Hazel::IndexBuffer> indexBuffer;
		indexBuffer.reset(Hazel::IndexBuffer::Create(indices, 3));
		m_VertexArray->SetIndexBuffer(indexBuffer);
		
		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			0.75f, -0.75f, 0.0f,
			0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		
		Hazel::Ref<Hazel::VertexBuffer> square_VB;
		square_VB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		square_VB->SetLayout({
			{ Hazel::ShaderDataType::Float3, "a_Position" }
		});
		
		m_SquareVA.reset(Hazel::VertexArray::Create());
		m_SquareVA->AddVertexBuffer(square_VB);
		
		unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0};
		Hazel::Ref<Hazel::IndexBuffer> square_IB;
		square_IB.reset(Hazel::IndexBuffer::Create(squareIndices, 6));
		m_SquareVA->SetIndexBuffer(square_IB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
	
			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";
		
		m_Shader.reset(Hazel::Shader::Create(vertexSrc, fragmentSrc));
		
		std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
	 
			uniform mat4 u_ViewProjection;
	 
			void main()
			{
			 gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		 )";
		
		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
			void main()
			{
			 color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		 )";

		m_SquareShader.reset(Hazel::Shader::Create(blueShaderVertexSrc, blueShaderFragmentSrc));
  }

  void OnUpdate(Hazel::TimeStep ts) override
  {
		if(Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
			m_CameraPosiition.x -= m_CameraMoveSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
			m_CameraPosiition.x += m_CameraMoveSpeed * ts;
		
		if(Hazel::Input::IsKeyPressed(HZ_KEY_UP))
			m_CameraPosiition.y += m_CameraMoveSpeed * ts;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
			m_CameraPosiition.y -= m_CameraMoveSpeed * ts;
		
		if(Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		
		Hazel::RenderCommand::SetColorClear({0.1f, 0.1f, 0.1f, 1.0f});
		Hazel::RenderCommand::Clear();
		
		m_Camera.SetPosition(m_CameraPosiition);
		m_Camera.SetRotation(m_CameraRotation);
		
		Hazel::Renderer::BeginScene(m_Camera);
		
		Hazel::Renderer::Submit(m_SquareShader, m_SquareVA);

		Hazel::Renderer::Submit(m_Shader, m_VertexArray);
		
		Hazel::Renderer::EndScene();
  }

  virtual void OnImGuiRender() override
  {
    ImGui::Begin("Test");
    ImGui::Text("Hello World");
    ImGui::End();
  }

  void OnEvent(Hazel::Event& event) override
  {
    //HZ_TRACE("{0}", event);
  }
	
private:
	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;
	
	Hazel::Ref<Hazel::Shader> m_SquareShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	
	Hazel::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosiition;
	float m_CameraMoveSpeed = 5.0f;
	
	float m_CameraRotation = 0.0f;
  float m_CameraRotationSpeed = 180.0f;

};

class Sandbox :public Hazel::Application
{
public:
  Sandbox()
  {
    PushLayer(new ExampleLayer());
  }

  ~Sandbox()
  {

  }


};

Hazel::Application* Hazel::CreateApplication()
{
  return new Sandbox();
}
