#pragma once

//#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel {

  class VulkanContext : public GraphicsContext
  {
  public:
    VulkanContext(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

    virtual void Destory() override;

    virtual void FrameRender(ImDrawData* draw_data) override;
    virtual void FramePresent() override;
    virtual void InitForVulkan() override;
  private:
    void SetupVulkan(const char** extensions, uint32_t extensions_count);
    void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
    void CleanupVulkanWindow();
    void CleanupVulkan();
  private:
    GLFWwindow* m_WindowHandle;

    VkAllocationCallbacks* m_Allocator = NULL;
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    uint32_t m_QueueFamily = (uint32_t)-1;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_Queue = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT m_DebugReport = VK_NULL_HANDLE;
    VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;

    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

    ImGui_ImplVulkanH_Window m_MainWindowData;
    int m_MinImageCount = 2;
    bool m_SwapChainRebuild = false;
  };

}
