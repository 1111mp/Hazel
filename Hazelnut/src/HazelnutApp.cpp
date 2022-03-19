#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

// #include "EditorLayer.h"
#include "TestLayer.h"

// #pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // ������ڵ�ַ

namespace Hazel {

  class Hazelnut :public Application
  {
  public:
    Hazelnut()
      : Application("Hazel Editor", "/Hazelnut/imgui.ini")
    {
      PushLayer(new TestLayer());
    }

    ~Hazelnut()
    {

    }


  };

  Application* CreateApplication()
  {
    return new Hazelnut();
  }

}
