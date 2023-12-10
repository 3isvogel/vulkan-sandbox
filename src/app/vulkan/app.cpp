#include "vulkan/vulkan_core.h"
#include <app/app.hpp>
#include <app/vulkan/helper.hpp>
#include <app/vulkan/validation/enable.hpp>
#include <lib/log.hpp>

void MainApp::createInstance() {
  // checkExtensionSupport();

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
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  extensions.emplace_back(
      VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

  createInfo.enabledExtensionCount = (uint32_t)extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();
  logDebug("Required extensions");
  for (const auto &extension : extensions) {
    logDebug("| %s", extension);
  }

  populateDebugMessengerOnValidation(createInfo);

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    e_runtime("failed to create Vulkan instance");
  }
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

  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice = device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    e_runtime("No suitable GPU found");
  }
}

// Read further
// https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/04_Logical_device_and_queues.html#_specifying_the_queues_to_be_created
// Command buffer creation on multiple threads and submission in a single call

void MainApp::createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

  VkDeviceQueueCreateInfo queueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = indices.graphicsFamily.value(),
      .queueCount = 1,
      .pQueuePriorities = &defaultQueuePriority};

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                .pQueueCreateInfos = &queueCreateInfo,
                                .queueCreateInfoCount = 1,
                                .pEnabledFeatures = &deviceFeatures};

  enforceDeviceExtensionsRequirements(createInfo, physicalDevice);
  legacyPopulateDeviceSpecificValidationOnValidation(createInfo);

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    e_runtime("Failed to create logical device");
  }
}