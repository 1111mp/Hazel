#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>

#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel {
  #ifdef HZ_DEBUG
    // —È÷§≤„
  #define ENABLE_VALIDATION_LAYERS
  #endif

  class VulkanContext : public GraphicsContext
  {
  public:
    VulkanContext(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

    virtual void Destory() override;
  private:
    struct QueueFamilyIndices {
      std::optional<uint32_t> graphicsFamily;
      std::optional<uint32_t> presentFamily;

      bool isComplete()
      {
        return graphicsFamily.has_value() && presentFamily.has_value();
      }
    };

    struct SwapChainSupportDetails {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
    };
  private:
    void createInstance();
    void pickPhysicalDevice();

    std::vector<const char*> VulkanContext::getRequiredExtensions();
    bool isDeviceSuitable(const VkPhysicalDevice& device);
    VulkanContext::QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);
    VulkanContext::SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);

    #ifdef ENABLE_VALIDATION_LAYERS
    void setupDebugMessenger();
    #endif
  private:
    GLFWwindow* m_WindowHandle;
    VkInstance m_Instance;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface;

    #ifdef ENABLE_VALIDATION_LAYERS
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    #endif
  };

}
