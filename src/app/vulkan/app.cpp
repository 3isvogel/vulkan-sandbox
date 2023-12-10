#include <app/app.hpp>
#include <app/vulkan/helper.hpp>
#include <app/vulkan/queues.hpp>
#include <app/vulkan/validation/enable.hpp>
#include <lib/log.hpp>

void MainApp::createInstance() {

  enableValidationLayerOnValidation();

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

  populateDebugMessengerOnValidation(createInfo);

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    e_runtime("failed to create Vulkan instance");
  }
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

  enforceDeviceExtensionsRequirements(physicalDevice);
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  legacyPopulateDeviceSpecificValidationOnValidation(createInfo);

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