#include "vulkan/vulkan_core.h"
#include <app/vulkan/helper.hpp>
#include <app/vulkan/validation/enable.hpp>
#include <lib/log.hpp>

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

bool isDeviceSuitable(VkPhysicalDevice physicalDevice) {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
  return indices.isComplete();
}

float defaultQueuePriority = 1.0f;
// To quickly check later that all properties in the struct have a value
bool QueueFamilyIndices::isComplete() { return graphicsFamily.has_value(); }

std::vector<VkQueueFamilyProperties>
queryQueueFamilies(VkPhysicalDevice device) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());
  return queueFamilies;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  auto queueFamilies = queryQueueFamilies(device);

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    if (indices.isComplete()) {
      break;
    }
    i++;
  }

  return indices;
}

#include <string.h>

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
std::vector<const char *> deviceExtensionsRequired;

void enforceDeviceExtensionsRequirements(VkDeviceCreateInfo &createInfo,
                                         VkPhysicalDevice physicalDevice) {

  auto extensions = queryDeviceExtensionProperties(physicalDevice);

  for (const auto &extension : extensions)
    for (const auto &enforcingExtension : enforcingExtensions)
      if (strcmp(enforcingExtension, extension.extensionName) == 0)
        deviceExtensionsRequired.emplace_back(enforcingExtension);

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensionsRequired.size());
  if (createInfo.enabledExtensionCount != 0)
    createInfo.ppEnabledExtensionNames = deviceExtensionsRequired.data();
}