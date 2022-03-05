#include "VulkanContext.h"

namespace Hazel {

  #define IMGUI_UNLIMITED_FRAME_RATE

  #ifdef HZ_DEBUG
    #define IMGUI_VULKAN_DEBUG_REPORT
  #endif

  #ifdef IMGUI_VULKAN_DEBUG_REPORT
  static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
  {
    (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
    //fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
    HZ_HAZEL_INFO("[vulkan] Debug report from ObjectType: {0} \n nMessage: {1}", objectType, pMessage);
    return VK_FALSE;
  }
  #endif // IMGUI_VULKAN_DEBUG_REPORT

  static void check_vk_result(VkResult err)
  {
    if (err == 0)
      return;
    HZ_HAZEL_ERROR("[vulkan] Error: VkResult = {0}", err);
    HZ_CORE_ASSERT(false);
  }

  VulkanContext::VulkanContext(GLFWwindow* windowHandle)
    : m_WindowHandle(windowHandle)
  {
  }

  void VulkanContext::Init()
  {
    HZ_CORE_ASSERT(glfwVulkanSupported(), "GLFW: Vulkan Not Supported!");

    uint32_t extensions_count = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    SetupVulkan(extensions, extensions_count);

    // Create Window Surface
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(m_Instance, m_WindowHandle, m_Allocator, &surface);
    check_vk_result(err);

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize(m_WindowHandle, &w, &h);
    ImGui_ImplVulkanH_Window* wd = &m_MainWindowData;
    ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
    wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
    wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
    wd->ClearValue.color.float32[3] = clear_color.w;
    SetupVulkanWindow(wd, surface, w, h);
  }

  void VulkanContext::SwapBuffers()
  {
    // Resize swap chain?
    if (m_SwapChainRebuild)
    {
      int width, height;
      glfwGetFramebufferSize(m_WindowHandle, &width, &height);
      if (width > 0 && height > 0)
      {
        ImGui_ImplVulkan_SetMinImageCount(m_MinImageCount);
        ImGui_ImplVulkanH_CreateOrResizeWindow(m_Instance, m_PhysicalDevice, m_Device, &m_MainWindowData, m_QueueFamily, m_Allocator, width, height, m_MinImageCount);
        m_MainWindowData.FrameIndex = 0;
        m_SwapChainRebuild = false;
      }
    }
  }

  void VulkanContext::Destory()
  {
    // Cleanup
    VkResult err = vkDeviceWaitIdle(m_Device);
    check_vk_result(err);

    CleanupVulkanWindow();
    CleanupVulkan();
  }

  void VulkanContext::SetupVulkan(const char** extensions, uint32_t extensions_count)
  {
    VkResult err;

    // Create Vulkan Instance
    {
      VkInstanceCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      create_info.enabledExtensionCount = extensions_count;
      create_info.ppEnabledExtensionNames = extensions;

      #ifdef IMGUI_VULKAN_DEBUG_REPORT
      // Enabling validation layers
      const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
      create_info.enabledLayerCount = 1;
      create_info.ppEnabledLayerNames = layers;

      // Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
      const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
      memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
      extensions_ext[extensions_count] = "VK_EXT_debug_report";
      create_info.enabledExtensionCount = extensions_count + 1;
      create_info.ppEnabledExtensionNames = extensions_ext;

      // Create Vulkan Instance
      err = vkCreateInstance(&create_info, m_Allocator, &m_Instance);
      check_vk_result(err);
      free(extensions_ext);

      // Get the function pointer (required for any extensions)
      auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugReportCallbackEXT");
      HZ_CORE_ASSERT(vkCreateDebugReportCallbackEXT != NULL);

      // Setup the debug report callback
      VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
      debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
      debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
      debug_report_ci.pfnCallback = debug_report;
      debug_report_ci.pUserData = NULL;
      err = vkCreateDebugReportCallbackEXT(m_Instance, &debug_report_ci, m_Allocator, &m_DebugReport);
      check_vk_result(err);
      #else
      // Create Vulkan Instance without any debug feature
      err = vkCreateInstance(&create_info, m_Allocator, &m_Instance);
      check_vk_result(err);
      IM_UNUSED(m_DebugReport);
      #endif
    }

    // Select GPU
    {
      uint32_t gpu_count;
      err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, NULL);
      check_vk_result(err);
      HZ_CORE_ASSERT(gpu_count > 0);

      VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
      err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, gpus);
      check_vk_result(err);

      // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
      // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
      // dedicated GPUs) is out of scope of this sample.
      int use_gpu = 0;
      for (int i = 0; i < (int)gpu_count; i++)
      {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(gpus[i], &properties);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
          use_gpu = i;
          break;
        }
      }

      m_PhysicalDevice = gpus[use_gpu];
      free(gpus);
    }

    // Select graphics queue family
    {
      uint32_t count;
      vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, NULL);
      VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
      vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queues);
      for (uint32_t i = 0; i < count; i++)
        if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
          m_QueueFamily = i;
          break;
        }
      free(queues);
      HZ_CORE_ASSERT(m_QueueFamily != (uint32_t)-1);
    }

    // Create Logical Device (with 1 queue)
    {
      int device_extension_count = 1;
      const char* device_extensions[] = { "VK_KHR_swapchain" };
      const float queue_priority[] = { 1.0f };
      VkDeviceQueueCreateInfo queue_info[1] = {};
      queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_info[0].queueFamilyIndex = m_QueueFamily;
      queue_info[0].queueCount = 1;
      queue_info[0].pQueuePriorities = queue_priority;
      VkDeviceCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
      create_info.pQueueCreateInfos = queue_info;
      create_info.enabledExtensionCount = device_extension_count;
      create_info.ppEnabledExtensionNames = device_extensions;
      err = vkCreateDevice(m_PhysicalDevice, &create_info, m_Allocator, &m_Device);
      check_vk_result(err);
      vkGetDeviceQueue(m_Device, m_QueueFamily, 0, &m_Queue);
    }

    // Create Descriptor Pool
    {
      VkDescriptorPoolSize pool_sizes[] =
      {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
      };
      VkDescriptorPoolCreateInfo pool_info = {};
      pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
      pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
      pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
      pool_info.pPoolSizes = pool_sizes;
      err = vkCreateDescriptorPool(m_Device, &pool_info, m_Allocator, &m_DescriptorPool);
      check_vk_result(err);
    }
  }

  // All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
  // Your real engine/app may not use them.
  void VulkanContext::SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
  {
    wd->Surface = surface;

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_QueueFamily, wd->Surface, &res);
    HZ_CORE_ASSERT(res == VK_TRUE, "Error no WSI support on physical device 0");

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(m_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

    // Select Present Mode
    #ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
    #else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
    #endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(m_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));

    // Create SwapChain, RenderPass, Framebuffer, etc.
    HZ_CORE_ASSERT(m_MinImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(m_Instance, m_PhysicalDevice, m_Device, wd, m_QueueFamily, m_Allocator, width, height, m_MinImageCount);
  }

  void VulkanContext::CleanupVulkanWindow()
  {
    ImGui_ImplVulkanH_DestroyWindow(m_Instance, m_Device, &m_MainWindowData, m_Allocator);
  }

  void VulkanContext::CleanupVulkan()
  {
    vkDestroyDescriptorPool(m_Device, m_DescriptorPool, m_Allocator);

    #ifdef IMGUI_VULKAN_DEBUG_REPORT
    // Remove the debug report callback
    auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXT(m_Instance, m_DebugReport, m_Allocator);
    #endif // IMGUI_VULKAN_DEBUG_REPORT

    vkDestroyDevice(m_Device, m_Allocator);
    vkDestroyInstance(m_Instance, m_Allocator);
  }

  void VulkanContext::FrameRender(ImDrawData* draw_data)
  {
    VkResult err;
    auto wd = &m_MainWindowData;

    VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    err = vkAcquireNextImageKHR(m_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    {
      m_SwapChainRebuild = true;
      return;
    }
    check_vk_result(err);

    ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
    {
      err = vkWaitForFences(m_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
      check_vk_result(err);

      err = vkResetFences(m_Device, 1, &fd->Fence);
      check_vk_result(err);
    }
    {
      err = vkResetCommandPool(m_Device, fd->CommandPool, 0);
      check_vk_result(err);
      VkCommandBufferBeginInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
      err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
      check_vk_result(err);
    }
    {
      VkRenderPassBeginInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      info.renderPass = wd->RenderPass;
      info.framebuffer = fd->Framebuffer;
      info.renderArea.extent.width = wd->Width;
      info.renderArea.extent.height = wd->Height;
      info.clearValueCount = 1;
      info.pClearValues = &wd->ClearValue;
      vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(fd->CommandBuffer);
    {
      VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      VkSubmitInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      info.waitSemaphoreCount = 1;
      info.pWaitSemaphores = &image_acquired_semaphore;
      info.pWaitDstStageMask = &wait_stage;
      info.commandBufferCount = 1;
      info.pCommandBuffers = &fd->CommandBuffer;
      info.signalSemaphoreCount = 1;
      info.pSignalSemaphores = &render_complete_semaphore;

      err = vkEndCommandBuffer(fd->CommandBuffer);
      check_vk_result(err);
      err = vkQueueSubmit(m_Queue, 1, &info, fd->Fence);
      check_vk_result(err);
    }
  }

  void VulkanContext::FramePresent()
  {
    if (m_SwapChainRebuild)
      return;

    auto wd = &m_MainWindowData;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &wd->Swapchain;
    info.pImageIndices = &wd->FrameIndex;
    VkResult err = vkQueuePresentKHR(m_Queue, &info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    {
      m_SwapChainRebuild = true;
      return;
    }
    check_vk_result(err);
    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
  }

  void VulkanContext::InitForVulkan()
  {
    auto wd = &m_MainWindowData;
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = m_Instance;
    init_info.PhysicalDevice = m_PhysicalDevice;
    init_info.Device = m_Device;
    init_info.QueueFamily = m_QueueFamily;
    init_info.Queue = m_Queue;
    init_info.PipelineCache = m_PipelineCache;
    init_info.DescriptorPool = m_DescriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = m_MinImageCount;
    init_info.ImageCount = wd->ImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = m_Allocator;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);

    // Upload Fonts
    {
      // Use any command queue
      VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
      VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

      VkResult err = vkResetCommandPool(m_Device, command_pool, 0);
      check_vk_result(err);
      VkCommandBufferBeginInfo begin_info = {};
      begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
      err = vkBeginCommandBuffer(command_buffer, &begin_info);
      check_vk_result(err);

      ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

      VkSubmitInfo end_info = {};
      end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      end_info.commandBufferCount = 1;
      end_info.pCommandBuffers = &command_buffer;
      err = vkEndCommandBuffer(command_buffer);
      check_vk_result(err);
      err = vkQueueSubmit(m_Queue, 1, &end_info, VK_NULL_HANDLE);
      check_vk_result(err);

      err = vkDeviceWaitIdle(m_Device);
      check_vk_result(err);
      ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
  }

}
