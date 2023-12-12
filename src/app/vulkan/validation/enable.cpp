#include <app/vulkan/validation/EXT_proxy.hpp>
#include <app/vulkan/validation/debugger.hpp>
#include <app/vulkan/validation/enable.hpp>
#include <cstddef>
#include <lib/log.hpp>
#include <vulkan/vulkan.h>

std::vector<const char *> Validation::layers = {"VK_LAYER_KHRONOS_validation"};

#define VALIDATION_ENABLE true
#ifdef BUILD_RELEASE
#define VALIDATION_ENABLE
#endif

const bool Validation::enabled = VALIDATION_ENABLE;

Validation::Validation(PFN_vkDebugUtilsMessengerCallbackEXT callback) {
  debugMessengerCreateInfo = Debugger().callback(callback).info();
}
Validation::Validation() { debugMessengerCreateInfo = Debugger().info(); }

void Validation::enable() {
  if (!enabled)
    return;
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : layers) {
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

void Validation::populateDebugMessenger(VkInstanceCreateInfo &createInfo) {
  if (enabled) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.data();
    createInfo.pNext = &debugMessengerCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }
}

void Validation::setup(VkInstance &instance) {

  if (!enabled)
    return;

  ref(instance);

  if (CreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr,
                                   &debugMessenger) != VK_SUCCESS) {
    e_runtime("Failed to setup debug messenger");
  }
}

void Validation::destroy() {
  if (enabled) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }
}

void Validation::legacyPopulateDeviceSpecific(VkDeviceCreateInfo &createInfo) {
  if (enabled) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }
}