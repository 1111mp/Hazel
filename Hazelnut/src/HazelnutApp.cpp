#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "EditorLayer.h"

// #pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // ������ڵ�ַ

namespace Hazel {

  class Hazelnut :public Application
  {
  public:
    Hazelnut()
      : Application("Hazel Editor")
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
