#pragma once

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv)
{
  Hazel::Log::Init();

  HZ_HAZEL_WARN("Initialized Log!");
  HZ_INFO("Hello! Var={0}", 5);

  auto app = Hazel::CreateApplication();
  app->Run();
  delete app;
}
