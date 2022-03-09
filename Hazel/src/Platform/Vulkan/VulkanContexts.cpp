#include "VulkanContexts.h"

#include <fstream>

namespace Hazel
{

#define IMGUI_UNLIMITED_FRAME_RATE

#ifdef HZ_DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

#ifdef IMGUI_VULKAN_DEBUG_REPORT
  static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData)
  {
    (void)flags;
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pUserData;
    (void)pLayerPrefix; // Unused arguments
    // fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
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

  static std::vector<char> readFile(const std::string &filename)
  {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    HZ_CORE_ASSERT(file.is_open(), "failed to open file!");

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
  }

  VulkanContexts::VulkanContexts(GLFWwindow *windowHandle)
      : m_WindowHandle(windowHandle)
  {
  }

  void VulkanContexts::Init()
  {
    HZ_CORE_ASSERT(glfwVulkanSupported(), "GLFW: Vulkan Not Supported!");

    uint32_t extensions_count = 0;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    SetupVulkan(extensions, extensions_count);

    // Create Window Surface
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(m_Instance, m_WindowHandle, m_Allocator, &surface);
    check_vk_result(err);

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize(m_WindowHandle, &w, &h);
    ImGui_ImplVulkanH_Window *wd = &m_MainWindowData;
    // ImVec4 clear_color = ImVec4(0.8f, 0.3f, 0.2f, 1.0f);
    // wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
    // wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
    // wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
    // wd->ClearValue.color.float32[3] = clear_color.w;
    SetupVulkanWindow(wd, surface, w, h);

    {
      CreateRenderPass();
      CreateGraphicsPipeline();
      CreateFramebuffers();
      CreateCommandPool();
      CreateCommandBuffer();
    }
  }

  void VulkanContexts::SwapBuffers()
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

  void VulkanContexts::Destory()
  {
    // Cleanup
    VkResult err = vkDeviceWaitIdle(m_Device);
    check_vk_result(err);

    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);

    for (auto framebuffer : m_SwapChainFramebuffers)
    {
      vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
    }

    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
    vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

    CleanupVulkanWindow();
    CleanupVulkan();
  }

  void VulkanContexts::SetupVulkan(const char **extensions, uint32_t extensions_count)
  {
    VkResult err;

    // Create Vulkan Instance
    {
      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Renderer";
      appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.pEngineName = "No Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.apiVersion = VK_API_VERSION_1_3;

      VkInstanceCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      create_info.pApplicationInfo = &appInfo;
      create_info.enabledExtensionCount = extensions_count;
      create_info.ppEnabledExtensionNames = extensions;

#ifdef IMGUI_VULKAN_DEBUG_REPORT
      // Enabling validation layers
      const char *layers[] = {"VK_LAYER_KHRONOS_validation"};
      create_info.enabledLayerCount = 1;
      create_info.ppEnabledLayerNames = layers;

      // Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
      const char **extensions_ext = (const char **)malloc(sizeof(const char *) * (extensions_count + 1));
      memcpy(extensions_ext, extensions, extensions_count * sizeof(const char *));
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

      VkPhysicalDevice *gpus = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * gpu_count);
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
      VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * count);
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
      int device_extension_count = 2;
      const char *device_extensions[] = {"VK_KHR_swapchain", "VK_KHR_portability_subset"};
      const float queue_priority[] = {1.0f};
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
              {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
              {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
              {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
              {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
              {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
              {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
              {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
              {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
              {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
              {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
              {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
      VkDescriptorPoolCreateInfo pool_info = {};
      pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
      pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
      pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
      pool_info.pPoolSizes = pool_sizes;
      err = vkCreateDescriptorPool(m_Device, &pool_info, m_Allocator, &m_ImGuiDescriptorPool);
      check_vk_result(err);
    }
  }

  // All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
  // Your real engine/app may not use them.
  void VulkanContexts::SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height)
  {
    wd->Surface = surface;

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_QueueFamily, wd->Surface, &res);
    HZ_CORE_ASSERT(res == VK_TRUE, "Error no WSI support on physical device 0");

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(m_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
#else
    VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(m_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));

    // Create SwapChain, RenderPass, Framebuffer, etc.
    HZ_CORE_ASSERT(m_MinImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(m_Instance, m_PhysicalDevice, m_Device, wd, m_QueueFamily, m_Allocator, width, height, m_MinImageCount);
  }

  void VulkanContexts::CleanupVulkanWindow()
  {
    ImGui_ImplVulkanH_DestroyWindow(m_Instance, m_Device, &m_MainWindowData, m_Allocator);
  }

  void VulkanContexts::CleanupVulkan()
  {
    vkDestroyDescriptorPool(m_Device, m_ImGuiDescriptorPool, m_Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
    // Remove the debug report callback
    auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXT(m_Instance, m_DebugReport, m_Allocator);
#endif // IMGUI_VULKAN_DEBUG_REPORT

    vkDestroyDevice(m_Device, m_Allocator);
    vkDestroyInstance(m_Instance, m_Allocator);
  }

  void VulkanContexts::FrameRender(ImDrawData *draw_data)
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

    ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
    {
      err = vkWaitForFences(m_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX); // wait indefinitely instead of periodically checking
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

    {
      vkResetCommandBuffer(m_CommandBuffer, 0);
      RecordCommandBuffer(m_CommandBuffer, wd->FrameIndex);
    }

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

  void VulkanContexts::FramePresent()
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
    info.pResults = nullptr; // Optional
    VkResult err = vkQueuePresentKHR(m_Queue, &info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    {
      m_SwapChainRebuild = true;
      return;
    }
    check_vk_result(err);
    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
  }

  void VulkanContexts::InitForVulkan()
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
    init_info.DescriptorPool = m_ImGuiDescriptorPool;
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

  void VulkanContexts::CreateRenderPass()
  {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_MainWindowData.SurfaceFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    HZ_CORE_ASSERT(vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) == VK_SUCCESS, "failed to create render pass!");
  }

  void VulkanContexts::CreateGraphicsPipeline()
  {
    auto vertShaderCode = readFile(AssetsDir + "/shaders/Vulkan/vert.spv");
    auto fragShaderCode = readFile(AssetsDir + "/shaders/Vulkan/frag.spv");

    VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_MainWindowData.Width;
    viewport.height = (float)m_MainWindowData.Height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent.width = (uint32_t)(m_MainWindowData.Width);
    scissor.extent.height = (uint32_t)(m_MainWindowData.Height);

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;          // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;            // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    HZ_CORE_ASSERT(vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) == VK_SUCCESS, "failed to create pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = m_RenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    HZ_CORE_ASSERT(vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_MainWindowData.Pipeline) == VK_SUCCESS, "failed to create graphics pipeline!");

    vkDestroyShaderModule(m_Device, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_Device, vertShaderModule, nullptr);
  }

  VkShaderModule VulkanContexts::CreateShaderModule(const std::vector<char> &code)
  {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;

    HZ_CORE_ASSERT(vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) == VK_SUCCESS, "failed to create shader module!");

    return shaderModule;
  }

  void VulkanContexts::CreateFramebuffers()
  {
    m_SwapChainFramebuffers.resize(m_MainWindowData.ImageCount);

    ImGui_ImplVulkanH_Window *wd = &m_MainWindowData;
    for (size_t i = 0; i < m_MainWindowData.ImageCount; i++)
    {
      ImGui_ImplVulkanH_Frame *fd = &wd->Frames[i];
      VkImageView attachments[] = {
          fd->BackbufferView};

      VkFramebufferCreateInfo framebufferInfo{};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = m_RenderPass;
      framebufferInfo.attachmentCount = 1;
      framebufferInfo.pAttachments = attachments;
      framebufferInfo.width = m_MainWindowData.Width;
      framebufferInfo.height = m_MainWindowData.Height;
      framebufferInfo.layers = 1;

      if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to create framebuffer!");
      }
    }
  }

  void VulkanContexts::CreateCommandPool()
  {
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_PhysicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    HZ_CORE_ASSERT(vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) == VK_SUCCESS, "failed to create command pool!");
  }

  VulkanContexts::QueueFamilyIndices VulkanContexts::FindQueueFamilies(const VkPhysicalDevice &device)
  {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        indices.graphicsFamily = i;

      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_MainWindowData.Surface, &presentSupport);

      if (presentSupport)
        indices.presentFamily = i;

      if (indices.isComplete())
        break;

      i++;
    }

    return indices;
  }

  void VulkanContexts::CreateCommandBuffer()
  {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(m_Device, &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void VulkanContexts::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
  {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_SwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent.width = m_MainWindowData.Width;
    renderPassInfo.renderArea.extent.height = m_MainWindowData.Height;

    VkClearValue clearColor = {{{1.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_MainWindowData.Pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}