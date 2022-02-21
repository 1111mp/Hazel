#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "EditorLayer.h"

// #pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // 设置入口地址

namespace Hazel {

  class HazelEditor :public Application
  {
  public:
    HazelEditor()
      : Application("Hazel Editor")
    {
      PushLayer(new EditorLayer());
    }

    ~HazelEditor()
    {

    }


  };

  Application* CreateApplication()
  {
    return new HazelEditor();
  }

}
