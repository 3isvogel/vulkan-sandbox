#include <app/vulkan/helper.hpp>
#include <app/vulkan/swapchains.hpp>
#include <app/vulkan/validation/enable.hpp>
#include <lib/log.hpp>

#define X(name) name,
std::vector<const char *> deviceExtensions = {DEVICE_EXTENSIONS_LIST};
#undef X

std::vector<VkExtensionProperties>
queryDeviceExtensionProperties(VkPhysicalDevice physicalDevice);
bool checkDeviceExtensionSupport(VkPhysicalDevice device);

std::vector<const char *> getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  requireValidationLayerOnValidation(extensions);
  return extensions;
}

std::vector<VkExtensionProperties> queryInstanceExtensionProperties() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());
  return extensions;
}

bool requireExtensionIfAvailable(std::vector<const char *> &extensions,
                                 const char *ext) {

  auto availableExt = queryInstanceExtensionProperties();

  for (const auto &extProp : availableExt) {
    if (strcmp(ext, extProp.extensionName) == 0) {
      extensions.emplace_back(ext);
      return true;
    }
  }
  return false;
}

void printExtensionSupport() {

  auto extensions = queryInstanceExtensionProperties();

  logDebug("Available Instance extensions");
  for (const auto &extension : extensions)
    logDebug("\t%s", extension.extensionName);
}

void printPhysicalDeviceExtensionProperties(VkPhysicalDevice physicalDevice) {

  auto extensions = queryInstanceExtensionProperties();

  logDebug("Physical device supported extensions");
  for (const auto &extension : extensions)
    logDebug(" |\t%s", extension.extensionName);
}

// TODO: Better device selection
// Following
// https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/03_Physical_devices_and_queue_families.html#_base_device_suitability_checks
// Can implement device sorting by suitability or manual selection

bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

  bool extensionSupported = checkDeviceExtensionSupport(physicalDevice);

  bool swapChainAdequate = false;
  if (extensionSupported) {
    SwapChainSupportDetails SwapChainSupport =
        querySwapChainSupport(physicalDevice, surface);
    swapChainAdequate = !SwapChainSupport.formats.empty() &&
                        !SwapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionSupported && swapChainAdequate;
}

#include <set>
#include <string>

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
  auto availableExtensions = queryDeviceExtensionProperties(device);

  std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                           deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

std::vector<VkExtensionProperties>
queryDeviceExtensionProperties(VkPhysicalDevice physicalDevice) {
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       extensions.data());
  return extensions;
}

std::vector<const char *> enforcingExtensions = {"VK_KHR_portability_subset"};

void enforceDeviceExtensionsRequirements(VkPhysicalDevice physicalDevice) {

  auto extensions = queryDeviceExtensionProperties(physicalDevice);

  for (const auto &extension : extensions)
    for (const auto &enforcingExtension : enforcingExtensions)
      if (strcmp(enforcingExtension, extension.extensionName) == 0)
        deviceExtensions.emplace_back(enforcingExtension);
}