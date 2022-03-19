#pragma once

//#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel
{

  class VulkanContext : public GraphicsContext
  {
  public:
    VulkanContext(GLFWwindow *windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

    virtual void Destory() override;

    virtual void InitForVulkan() override;

    void DrawFrame() override;
    virtual void SetFramebufferResized(bool resized) override { m_FramebufferResized = resized; };

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

    struct SwapChainSupportDetails
    {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
    };

  private:
    void InitVulkan();

    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSwapChain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateImGuiRenderPass();
    void CreateDescriptorSetLayout();
    void CreateGraphicsPipeline();
    void CreateCommandPool(VkCommandPool *cmdPool);
    void CreateDepthResources();
    void CreateFramebuffers();
    void CreateImGuiFramebuffers();
    void CreateTextureImage();
    void CreateTextureImageView();
    void CreateTextureSampler();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSets();
    void CreateCommandBuffers();
    void CreateImGuiCommandBuffers();
    void CreateSyncObjects();

    void RecreateSwapChain();
    void CleanupSwapChain();
    void RecordCommandBuffer(const uint32_t &imageIndex);
    void RecordImGuiCommandBuffer(const uint32_t &imageIndex);
    void UpdateUniformBuffer(const uint32_t &currentImage);
    void DestroyDebugUtilsMessengerEXT(const VkInstance &instance, const VkDebugUtilsMessengerEXT &debugMessenger, const VkAllocationCallbacks *pAllocator);

    VkCommandBuffer BeginSingleTimeCommands(const VkCommandPool &cmdPool);
    void EndSingleTimeCommands(const VkCommandBuffer &commandBuffer, const VkCommandPool &cmdPool);
    bool CheckValidationLayerSupport();
    std::vector<const char *> GetRequiredExtensions();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    VkResult CreateDebugUtilsMessengerEXT(const VkInstance &instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
    bool IsDeviceSuitable(const VkPhysicalDevice &device);
    QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &device);
    bool CheckDeviceExtensionSupport(const VkPhysicalDevice &device);
    SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice &device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    VkShaderModule CreateShaderModule(const std::vector<char> &code);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    uint32_t FindMemoryType(const uint32_t &typeFilter, const VkMemoryPropertyFlags &properties);
    void CopyBuffer(const VkBuffer &srcBuffer, const VkBuffer &dstBuffer, const VkDeviceSize &size);
    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    bool HasStencilComponent(VkFormat format);
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    VkFormat FindDepthFormat();
    VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

  private:
    GLFWwindow *m_WindowHandle;

    VkInstance m_Instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device;
    QueueFamilyIndices m_QueueFamilyIndices;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
    VkSwapchainKHR m_SwapChain;
    std::vector<VkImage> m_SwapChainImages;
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;
    std::vector<VkImageView> m_SwapChainImageViews;
    VkRenderPass m_RenderPass;
    VkRenderPass m_ImGuiRenderPass;
    VkDescriptorSetLayout m_DescriptorSetLayout;
    VkPipelineLayout m_PipelineLayout;
    VkPipeline m_GraphicsPipeline;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;
    std::vector<VkFramebuffer> m_ImGuiFramebuffers;
    VkCommandPool m_CommandPool;
    VkCommandPool m_ImGuiCommandPool;

    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;
    VkBuffer m_IndexBuffer;
    VkDeviceMemory m_IndexBufferMemory;
    std::vector<VkBuffer> m_UniformBuffers;
    std::vector<VkDeviceMemory> m_UniformBuffersMemory;

    VkImage m_TextureImage;
    VkDeviceMemory m_TextureImageMemory;
    VkImageView m_TextureImageView;
    VkSampler m_TextureSampler;

    VkImage m_DepthImage;
    VkDeviceMemory m_DepthImageMemory;
    VkImageView m_DepthImageView;

    VkDescriptorPool m_DescriptorPool;
    VkDescriptorPool m_ImGuiDescriptorPool;
    std::vector<VkDescriptorSet> m_DescriptorSets;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    std::vector<VkCommandBuffer> m_ImGuiCommandBuffers;
    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;

    uint32_t m_CurrentFrame = 0;
    uint32_t m_ImageCount = 2;
    bool m_FramebufferResized = false;

    const std::vector<const char *> m_DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};
    bool m_EnableValidationLayers = false;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    // validation layers
    const std::vector<const char *> m_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"};
  };

}
