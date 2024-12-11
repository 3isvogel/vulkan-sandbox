#include "devices.hpp"
#include <lib/log.hpp>
#include <set>

bool checkDeviceExteinsionSupport(VkPhysicalDevice device, Names extensions);

// Not used now, use bool to pick first available, alternatively rate the
// devices and pick the best one
bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface,
                      const EngineConf &conf) {

  // // Probes device properties and features
  // VkPhysicalDeviceProperties deviceProperties;
  // VkPhysicalDeviceFeatures deviceFeatures;
  // vkGetPhysicalDeviceProperties(device, &deviceProperties);
  // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  // all required family exists
  QueueFamilyIndices indices = findQueueFamilies(device, surface);

  // supports all extensions
  bool extensionsAllSupported =
      checkDeviceExteinsionSupport(device, conf.extensions.device);

  // swapchain supports all required settings
  bool swapChainAdequate = false;
  // only check if extensions are supported already, to avoid problems
  if (extensionsAllSupported) {
    SwapChainSupportDetails swapChainSupport =
        querySwapChainSupport(device, surface);
    swapChainAdequate = !swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionsAllSupported && swapChainAdequate;
}

const std::vector<VkExtensionProperties>
enumerateDeviceExtensions(VkPhysicalDevice device) {

  // get all supported extensions
  uint32_t extensionConunt;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionConunt,
                                       nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionConunt);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionConunt,
                                       availableExtensions.data());

  // // Uncomment if you want to list all available extensions
  // logDebug("Device available extensions:");
  // for (const auto &extension : availableExtensions) {
  //   logDebug("  | %s", extension.extensionName);
  // }

  return availableExtensions;
}

bool checkDeviceExteinsionSupport(VkPhysicalDevice device, Names extensions) {
  // get all available extensions
  std::vector<VkExtensionProperties> availableExtensions =
      enumerateDeviceExtensions(device);

  // copy required extensions vector, for checkss
  std::set<String> requiredExtensions(extensions.begin(), extensions.end());

  // check all supported extensions
  for (const auto &extension : availableExtensions)
    requiredExtensions.erase(extension.extensionName);

  // all extensoins have been checked
  if (requiredExtensions.empty()) {
    return true;
  }

  logWarning("Device %p: required unsupported device extensions:", device);
  for (const auto &extension : requiredExtensions) {
    logWarning("  | %s", extension.c_str());
  }
  return false;
}

Status pickPhysicalDevice(VkPhysicalDevice &physicalDevice, VkInstance instance,
                          VkSurfaceKHR surface, const EngineConf &conf) {

  // Enumerate devices and pick the first that satisfy the requirements
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  logDebug("Found %u physical device(s)", deviceCount);
  if (deviceCount == 0) {
    return Status::PHYSICAL_DEVICE_FAIL;
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
  for (const auto &device : devices) {
    if (isDeviceSuitable(device, surface, conf)) {
      physicalDevice = device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    return Status::PHYSICAL_DEVICE_FAIL;
  }

  logDebug("Physical device selected");
  return OK;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surface) {
  QueueFamilyIndices indices;

  // probe the queueFamilies
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  // populate the structure
  int i = 0;
  for (const auto &queueFamily : queueFamilies) {

    // test graphics family
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    // test present family
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }

    // early exit
    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  return indices;
}

Status createLogicalDevice(VkDevice &device, VkPhysicalDevice physicalDevice,
                           VkSurfaceKHR surface, const EngineConf &conf) {
  // no caching baby, let's compute this hundreds of times
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
  std::set<int32_t> uniqueQueueFamilies = {indices.graphicsFamily,
                                           indices.presentFamily};

  // logical device creation
  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  // create required queues handler
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  // not used for now, will be used later to specify which features are used
  VkPhysicalDeviceFeatures deviceFeatures{};
  createInfo.pEnabledFeatures = &deviceFeatures;

  if (conf.validation.enable) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(conf.validation.layers.size());
    createInfo.ppEnabledLayerNames = conf.validation.layers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  Names requiredExtensions;

  for (const auto &extension : conf.extensions.device) {
    requiredExtensions.emplace_back(extension);
  }

  logDebug("Device extension required:");
  for (const auto &extension : requiredExtensions) {
    logDebug("  | %s", extension);
  }

  // add required extensions
  createInfo.enabledExtensionCount = requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  VkResult result =
      vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
  if (result != VK_SUCCESS) {
    logDebug("Failed to create logical device: %i", result);
    return Status::LOGICAL_DEVICE_FAIL;
  }

  logDebug("Logical device created");
  return Status::OK;
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface) {
  SwapChainSupportDetails details;

  // query capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  // query formats
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());
  }

  // query present modes
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}