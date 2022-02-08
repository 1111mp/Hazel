#include <Hazel.h>

#include "Platform/OpenGL/OpenGLShader.h"

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
		vertexBuffer =  Hazel::VertexBuffer::Create(vertices, sizeof(vertices));

		Hazel::BufferLayout layout = {
			{ Hazel::ShaderDataType::Float3, "a_Position" },
			{ Hazel::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		
		m_VertexArray = Hazel::VertexArray::Create();
		m_VertexArray->AddVertexBuffer(vertexBuffer);
		

		unsigned int indices[3] = { 0, 1, 2 };
		Hazel::Ref<Hazel::IndexBuffer> indexBuffer;
		indexBuffer = Hazel::IndexBuffer::Create(indices, 3);
		m_VertexArray->SetIndexBuffer(indexBuffer);
		
		float squareVertices[5 * 4] = {
			-0.75f, -0.75f, 0.0f, 0.0f, 0.0f,
			 0.75f, -0.75f, 0.0f, 1.0f, 0.0f,
			 0.75f,  0.75f, 0.0f, 1.0f, 1.0f,
			-0.75f,  0.75f, 0.0f, 0.0f, 1.0f
		};
		
		Hazel::Ref<Hazel::VertexBuffer> square_VB;
		square_VB =  Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		square_VB->SetLayout({
			{ Hazel::ShaderDataType::Float3, "a_Position" },
			{ Hazel::ShaderDataType::Float2, "a_TexCoord" },
		});
		
		m_SquareVA= Hazel::VertexArray::Create();
		m_SquareVA->AddVertexBuffer(square_VB);
		
		unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0};
		Hazel::Ref<Hazel::IndexBuffer> square_IB;
		square_IB= Hazel::IndexBuffer::Create(squareIndices, 6);
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
		
		m_Shader = Hazel::Shader::Create(vertexSrc, fragmentSrc);
		
		std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
	 
			uniform mat4 u_ViewProjection;

			out vec2 v_TexCoord;
	 
			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		 )";
		
		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;
			
			uniform sampler2D u_Texture;
			
			void main()
			{
			 color = texture(u_Texture, v_TexCoord);
			}
		 )";

		m_SquareShader = Hazel::Shader::Create(blueShaderVertexSrc, blueShaderFragmentSrc);


		m_Texture = Hazel::Texture2D::Create(AssetsDir + "/assets/textures/Checkerboard.png");
		m_ChernoLogoTexture = Hazel::Texture2D::Create(AssetsDir + "/assets/textures/ChernoLogo.png");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_SquareShader)->UploadUniformInt("u_Texture", 0);
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
		
		m_Texture->Bind();
		Hazel::Renderer::Submit(m_SquareShader, m_SquareVA);

		m_ChernoLogoTexture->Bind();
		Hazel::Renderer::Submit(m_SquareShader, m_SquareVA);

		//Hazel::Renderer::Submit(m_Shader, m_VertexArray);
		
		Hazel::Renderer::EndScene();
  }

  virtual void OnImGuiRender() override
  {
    /*ImGui::Begin("Test");
    ImGui::Text("Hello World");
    ImGui::End();*/
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

	Hazel::Ref<Hazel::Texture2D> m_Texture, m_ChernoLogoTexture;
	
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
