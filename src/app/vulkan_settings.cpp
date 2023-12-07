#include <GLFW/glfw3.h>
#include <app/EXT_proxy.hpp>
#include <app/callbacks.hpp>
#include <app/vulkan_settings.hpp>
#include <lib/log.hpp>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

std::vector<const char *> getRequiredExtensions();
void checkExtensionSupport();
bool checkValidationLayerSupport();

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef BUILD_RELEASE
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void createInstance(VkInstance &instance, const char name[]) {

  // checkExtensionSupport();

  if (enableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error(_REF_ "Validation layers requested not available");
  }

  auto extensions = getRequiredExtensions();

  VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                            .pApplicationName = name,
                            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                            .pEngineName = "No Engine",
                            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                            .apiVersion = VK_API_VERSION_1_0};

  VkInstanceCreateInfo createInfo{.sType =
                                      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                  .pApplicationInfo = &appInfo,
                                  .enabledLayerCount = 0};

#ifdef __APPLE__
  //   Following
  // https://vulkan.lunarg.com/doc/view/1.3.216.0/mac/getting_started.html#user-content-encountered-vk_error_incompatible_driver
  // It is mandatory to enable this extensions
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

  createInfo.enabledExtensionCount = (uint32_t)extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();

  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
  if (result != VK_SUCCESS) {
    throw std::runtime_error(_REF_ "failed to create Vulkan instance");
  }
}

std::vector<const char *> getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);
  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
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

bool checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers) {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}

void setupDebugMessenger(VkInstance &instance,
                         VkDebugUtilsMessengerEXT &debugMessenger) {
  if (!enableValidationLayers)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugCallback,
      .pUserData = nullptr};

  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                   &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error(_REF_ "Failed to setup debug messenger");
  }
}

void destroyDebugMessenger(VkInstance &instance,
                           VkDebugUtilsMessengerEXT &debugMessenger) {
  if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }
}