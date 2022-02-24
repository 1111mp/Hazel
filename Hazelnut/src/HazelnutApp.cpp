#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "EditorLayer.h"

// #pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // 设置入口地址

namespace Hazel {

  class Hazelnut :public Application
  {
  public:
    Hazelnut()
      : Application("Hazel Editor", "/Hazelnut/imgui.ini")
    {
      PushLayer(new EditorLayer());
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
