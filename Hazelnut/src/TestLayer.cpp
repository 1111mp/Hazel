#include "TestLayer.h"

namespace Hazel {

  TestLayer::TestLayer()
    : Layer("TestLayer")
  {
  }
  void TestLayer::OnAttach()
  {
  }

  void TestLayer::OnDetach()
  {
  }

  void TestLayer::OnUpdate(TimeStep ts)
  {
    RenderCommand::SetColorClear({ 0.1f, 0.1f, 0.1f, 1.0f });
    RenderCommand::Clear();
  }

  void TestLayer::OnImGuiRender()
  {
  }

  void TestLayer::OnEvent(Event& e)
  {
  }
}
