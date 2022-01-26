#include <Hazel.h>

#include "imgui.h"

class ExampleLayer : public Hazel::Layer
{
public:
  ExampleLayer()
    : Layer("Ecample")
  {

  }

  void OnUpdate() override
  {
    //HZ_INFO("ExampleLayer::Update");
    if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB))
      HZ_TRACE("Tab key is pressed!");
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