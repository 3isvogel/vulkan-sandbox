#include "lib/log.hpp"
#include <app/vulkan/validation/EXT_proxy.hpp>
#include <app/vulkan/validation/debugger.hpp>
#include <app/vulkan/validation/enable.hpp>
#include <stdexcept>
#include <vulkan/vulkan.h>

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef BUILD_RELEASE
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void requireValidationLayerOnValidation(std::vector<const char *> &extensions) {
  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    logDebug("Validation layer enabled");
  }
}

void enableValidationLayerOnValidation() {
  if (!enableValidationLayers)
    return;
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
      e_runtime("Validation layers requested not available");
    }
  }
  logDebug("ValidationLayer Available");
}

void populateDebugMessengerOnValidation(VkInstanceCreateInfo &createInfo) {
  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.pNext = &getDefaultDebugMessengerCreateInfo();
  }
}

void setupDebugMessengerOnValidation(VkInstance instance,
                                     VkDebugUtilsMessengerEXT &debugMessenger) {

  if (!enableValidationLayers)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  populateDebugMessengerCreateInfo(createInfo);

  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                   &debugMessenger) != VK_SUCCESS) {
    e_runtime("Failed to setup debug messenger");
  }
}

void destroyDebugMessengerOnValidation(
    VkInstance instance, VkDebugUtilsMessengerEXT &debugMessenger) {
  if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }
}