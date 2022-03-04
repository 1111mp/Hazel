#pragma once

#include <Hazel.h>

namespace Hazel {

  class TestLayer : public Layer
  {
  public:
    TestLayer();
    virtual ~TestLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(TimeStep ts) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event & e) override;
  };

}
