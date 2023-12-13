#include <app/vulkan/validation/EXT_proxy.hpp>
#include <app/vulkan/validation/callbacks.hpp>
#include <app/vulkan/validation/debugger.hpp>

std::vector<const char *> Validation::layers = {"VK_LAYER_KHRONOS_validation"};

#define VALIDATION_ENABLE true
#ifdef BUILD_RELEASE
#define VALIDATION_ENABLE false
#endif

const bool Validation::enabled = VALIDATION_ENABLE;

Validation::Validation() {
  createInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pUserData = nullptr};
}

Validation &
Validation::callback(PFN_vkDebugUtilsMessengerCallbackEXT callback) {
  createInfo.pfnUserCallback = callback;
  return *this;
}

Validation &Validation::build() {
  if (!enabled)
    pass;
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
  pass;
}

Validation &
Validation::populateDebugMessenger(VkInstanceCreateInfo &createInfo) {
  if (enabled) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.data();
    createInfo.pNext = &this->createInfo;
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }
  pass;
}

Validation &Validation::bind(VkInstance &instance) {

  if (!enabled)
    pass;

  ref(instance);

  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                   &debugMessenger) != VK_SUCCESS) {
    e_runtime("Failed to setup debug messenger");
  }
  pass;
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