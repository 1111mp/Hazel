#pragma once

//#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel
{

  class VulkanContexts : public GraphicsContext
  {
  public:
    VulkanContexts(GLFWwindow *windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;
    virtual void Destory() override;

    virtual void InitForVulkan() override;
    virtual void DrawFrame() override{};
    void FrameRender(ImDrawData *draw_data);
    void FramePresent();
    virtual void SetFramebufferResized(bool resized = true) override{};

  private:
    struct QueueFamilyIndices
    {
      std::optional<uint32_t> graphicsFamily;
      std::optional<uint32_t> presentFamily;

      bool isComplete()
      {
        return graphicsFamily.has_value() && presentFamily.has_value();
      }
    };

  private:
    void SetupVulkan(const char **extensions, uint32_t extensions_count);
    void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height);
    void CleanupVulkanWindow();
    void CleanupVulkan();

    void CreateRenderPass();
    void CreateGraphicsPipeline();
    VkShaderModule CreateShaderModule(const std::vector<char> &code);
    void CreateFramebuffers();
    void CreateCommandPool();
    VulkanContexts::QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &device);
    void CreateCommandBuffer();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  private:
    GLFWwindow *m_WindowHandle;

    VkAllocationCallbacks *m_Allocator = NULL;
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    uint32_t m_QueueFamily = (uint32_t)-1;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_Queue = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT m_DebugReport = VK_NULL_HANDLE;
    VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;

    VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;

    ImGui_ImplVulkanH_Window m_MainWindowData;
    VkRenderPass m_RenderPass;
    VkPipelineLayout m_PipelineLayout;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;
    VkCommandPool m_CommandPool;
    VkCommandBuffer m_CommandBuffer;

    int m_MinImageCount = 2;
    bool m_SwapChainRebuild = false;
  };

}