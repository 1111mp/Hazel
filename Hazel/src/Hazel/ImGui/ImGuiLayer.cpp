#include "ImGuiLayer.h"

#include "backends/imgui_impl_glfw.h"
#if defined(HZ_RENDERER_OPENGL)
#include "backends/imgui_impl_opengl3.h"
#elif defined(HZ_RENDERER_VULKAN)
#include "backends/imgui_impl_vulkan.h"
#endif

#include "Hazel/Core/Application.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Hazel
{

  ImGuiLayer::ImGuiLayer(GraphicsContext *context)
      : Layer("ImGuiLayer"), m_GraphicsContext(context)
  {
  }

  ImGuiLayer::ImGuiLayer(GraphicsContext *context, const std::string &ini)
      : Layer("ImGuiLayer"), m_GraphicsContext(context), m_IniPath(ProjectSourceDir + ini)
  {
  }

  ImGuiLayer::~ImGuiLayer()
  {
  }

  void ImGuiLayer::OnAttach()
  {
    HZ_PROFILE_FUNCTION();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = nullptr;

    ImGui::LoadIniSettingsFromDisk(m_IniPath.c_str());

    io.Fonts->AddFontFromFileTTF((AssetsDir + "/fonts/opensans/OpenSans-Bold.ttf").c_str(), 18.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF((AssetsDir + "/fonts/opensans/OpenSans-Regular.ttf").c_str(), 18.0f);

    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    SetDarkThemeColors();

#if defined(HZ_RENDERER_OPENGL)
    Application &app = Application::Get();
    GLFWwindow *window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __APPLE__
    ImGui_ImplOpenGL3_Init("#version 330");
#else
    ImGui_ImplOpenGL3_Init("#version 450");
#endif
#elif defined(HZ_RENDERER_VULKAN)
    m_GraphicsContext->InitForVulkan();
#endif
  }

  void ImGuiLayer::OnDetach()
  {
    HZ_PROFILE_FUNCTION();

#if defined(HZ_RENDERER_OPENGL)
    ImGui_ImplOpenGL3_Shutdown();
#elif defined(HZ_RENDERER_VULKAN)
    ImGui_ImplVulkan_Shutdown();
#endif
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGuiLayer::OnEvent(Event &e)
  {
    if (m_BlockEvents)
    {
      ImGuiIO &io = ImGui::GetIO();
      e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
      e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
    }
  }

  void ImGuiLayer::Begin()
  {
    HZ_PROFILE_FUNCTION();
    ImGui::SaveIniSettingsToDisk(m_IniPath.c_str());

#if defined(HZ_RENDERER_OPENGL)
    ImGui_ImplOpenGL3_NewFrame();
#elif defined(HZ_RENDERER_VULKAN)
    ImGui_ImplVulkan_NewFrame();
#endif
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void ImGuiLayer::End()
  {
    HZ_PROFILE_FUNCTION();

    ImGuiIO &io = ImGui::GetIO();
    Application &app = Application::Get();
    GLFWwindow *window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());

    ImGui::Render();

#if defined(HZ_RENDERER_OPENGL)
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow *backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
#elif defined(HZ_RENDERER_VULKAN)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow *backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
    m_GraphicsContext->DrawFrame();
#endif
  }

  void ImGuiLayer::SetDarkThemeColors()
  {
    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
  }
}
