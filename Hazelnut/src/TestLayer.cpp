#include "TestLayer.h"

namespace Hazel
{

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
  }

  void TestLayer::OnImGuiRender()
  {
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
  }

  void TestLayer::OnEvent(Event &e)
  {
  }
}
