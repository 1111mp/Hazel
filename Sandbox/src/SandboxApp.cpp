#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"
#include "Game/GameLayer.h"
#include "ParticleDemo.h"

// #pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // 设置入口地址

class Sandbox :public Hazel::Application
{
public:
  Sandbox()
  {
    //PushLayer(new ExampleLayer());
    PushLayer(new Sandbox2D());
    //PushLayer(new GameLayer());
  }

  ~Sandbox()
  {

  }


};

Hazel::Application* Hazel::CreateApplication()
{
  return new Sandbox();
}
