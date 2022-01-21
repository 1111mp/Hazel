#pragma once

#include "Core.h"
//#include "Events/Event.h"

namespace Hazel
{

  class HazelExports Application
  {
  public:
    Application();
    virtual ~Application();

    void Run();
  };

  // To be defined in CLIENT
  Application* CreateApplication();
}
