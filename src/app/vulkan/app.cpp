#include <app/app.hpp>
#include <app/vulkan/instance.hpp>
#include <app/vulkan/physical_devices.hpp>
#include <app/vulkan/pipeline.hpp>
#include <app/vulkan/queues.hpp>
#include <app/vulkan/renderpass.hpp>
#include <app/vulkan/validation/enable.hpp>
#include <lib/log.hpp>

void MainApp::createInstance() {

  validation.enable();

  auto extensions = getRequiredExtensions();

  VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                            .pApplicationName = name,
                            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                            .pEngineName = "No Engine",
                            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                            .apiVersion = VK_API_VERSION_1_0};

  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
  };

#ifdef __APPLE__
  //   Following
  // https://vulkan.lunarg.com/doc/view/1.3.216.0/mac/getting_started.html#user-content-encountered-vk_error_incompatible_driver
  // It is mandatory to enable this extensions
  requireExtensionIfAvailable(
      extensions, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
  if (requireExtensionIfAvailable(
          extensions, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  logDebug("Required extensions [%i]", extensions.size());
  for (const auto &extension : extensions) {
    logDebug("| %s", extension);
  }

  validation.populateDebugMessenger(createInfo);

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    e_runtime("failed to create Vulkan instance");
  }

  validation.ref(instance);

  logDebug("Vulkan instance: created \"%s\"", name);
}

void MainApp::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    e_runtime("No GPUs found with vulkan support");
  }
  logDebug("GPUs Found: %d", deviceCount);

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  int i = 0;
  for (const auto &device : devices) {
    if (isDeviceSuitable(device, surface)) {
      physicalDevice = device;
      logDebug("Physical device: selected [%i]", i);
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    e_runtime("No suitable GPU found");
  }
}

#include <set>

// Read further
// https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/04_Logical_device_and_queues.html#_specifying_the_queues_to_be_created
// Command buffer creation on multiple threads and submission in a single call

void MainApp::createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
#define X(name) indices.name##Family.value(),
  std::set<uint32_t> uniqueQueueFamilies = {FAMILIES_LIST};
#undef X

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority};
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pQueueCreateInfos = queueCreateInfos.data(),
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pEnabledFeatures = &deviceFeatures};

  auto deviceExtensions = enforceDeviceExtensionsRequirements(physicalDevice);
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  validation.legacyPopulateDeviceSpecific(createInfo);

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    e_runtime("Failed to create logical device");
  }
  logDebug("Logical device: created");

#define X(name)                                                                \
  vkGetDeviceQueue(device, indices.name##Family.value(), 0, &name##Queue);     \
  logDebug("Device queue (" #name "): attached");
  FAMILIES_LIST
#undef X

  logDebug("Queue handler: retrieved");
}

#include <iostream>

void MainApp::createSurface() {
  auto result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
  if (result != VK_SUCCESS) {
    e_runtime("Failed to create window surface");
  }
  logDebug("GLFW Surface: created");
}

void MainApp::createRenderPass() { renderPass = RenderPass(device, swapChain); }

void MainApp::createGraphicPipelines() {

  pipeline = Pipeline(device, swapChain, renderPass)
                 .setFragStage("triangleFrag.spv")
                 .setVertStage("triangleVert.spv")
                 .setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                 .setDynamicStates()
                 .build();
}

void MainApp::createFramebuffers() {
  swapChain.framebuffers.resize(swapChain.imageViews.size());
  size_t i = 0;
  for (; i < swapChain.imageViews.size(); i++) {
    VkImageView attachments[] = {swapChain.imageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass.get(),
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = swapChain.extent.width,
        .height = swapChain.extent.height,
        .layers = 1};
    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                            &swapChain.framebuffers[i]) != VK_SUCCESS) {
      e_runtime("Failed to create framebuffer");
    }
  }
  logDebug("Framebuffers: created [%i]", i);
}

void MainApp::createCommandPool() {
  QueueFamilyIndices queueFamilyIndices =
      findQueueFamilies(physicalDevice, surface);

  VkCommandPoolCreateInfo poolInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()};

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
      VK_SUCCESS) {
    e_runtime("Failed to create command pool");
  }
  logDebug("Command pool: created");
}

void MainApp::createCommandBuffer() {
  // TODO: handle this better
  VkCommandBufferAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = commandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1};

  if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) !=
      VK_SUCCESS) {
    e_runtime("Failed to allocate command buffers");
  }
  logDebug("Command buffers: allocated");
}

void MainApp::upateDynamicViewport() {

  // TODO: hardcoded default viewport, allow to set later
  // TODO: fix fat assignment
  config.viewport = {.x = 0.0f,
                     .y = 0.0f,
                     .width = (float)swapChain.extent.width,
                     .height = (float)swapChain.extent.height,
                     .minDepth = 0.0f,
                     .maxDepth = 1.0f};

  config.scissor = {.offset = {0, 0}, .extent = swapChain.extent};
}