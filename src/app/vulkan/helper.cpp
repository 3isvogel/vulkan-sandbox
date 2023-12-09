#include <GLFW/glfw3.h>
#include <app/vulkan/helper.hpp>
#include <app/vulkan/validation/EXT_proxy.hpp>
#include <app/vulkan/validation/callbacks.hpp>
#include <app/vulkan/validation/enable.hpp>
#include <lib/log.hpp>
#include <vector>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  bool isComplete();
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

std::vector<const char *> getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  requireValidationLayerOnValidation(extensions);
  return extensions;
}

void checkExtensionSupport() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());
  logDebug("Available Vulkan extensions");

  for (const auto &extension : extensions)
    logDebug("\t%s", extension.extensionName);
}

// TODO:
// Following
// https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/03_Physical_devices_and_queue_families.html#_base_device_suitability_checks
// Can implement device sorting by suitability or manual selection

bool isDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = findQueueFamilies(device);
  return indices.isComplete();
}

// To quickly check later that all properties in the struct have a value
bool QueueFamilyIndices::isComplete() { return graphicsFamily.has_value(); }

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

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