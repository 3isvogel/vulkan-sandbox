#include <app/vulkan/instance.hpp>
#include <app/vulkan/validation/enable.hpp>

std::vector<VkExtensionProperties> queryInstanceExtensionProperties() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());
  return extensions;
}

std::vector<const char *> getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  requireValidationLayerOnValidation(extensions);
  return extensions;
}

bool requireExtensionIfAvailable(std::vector<const char *> &extensions,
                                 const char *ext) {

  auto availableExt = queryInstanceExtensionProperties();

  for (const auto &extProp : availableExt) {
    if (strcmp(ext, extProp.extensionName) == 0) {
      extensions.push_back(ext);
      return true;
    }
  }
  return false;
}