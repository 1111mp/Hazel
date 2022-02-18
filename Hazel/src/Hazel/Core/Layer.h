#pragma once

#include "Base.h"
#include "TimeStep.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

  class Layer
  {
  public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer();

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(TimeStep ts) {}
    virtual void OnImGuiRender() {}
    virtual void OnEvent(Event& event) {}

    const std::string& GetName() { return m_DebugName; }

  protected:
    std::string m_DebugName;
  };

}
