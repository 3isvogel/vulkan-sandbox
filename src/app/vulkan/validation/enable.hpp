#pragma once
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <lib/log.hpp>
#include <vector>

class Validation {
public:
  Validation();
  Validation(PFN_vkDebugUtilsMessengerCallbackEXT callback);
  void enable();
  void setup(VkInstance &instance);
  void populateDebugMessenger(VkInstanceCreateInfo &createInfo);
  void legacyPopulateDeviceSpecific(VkDeviceCreateInfo &createInfo);
  void destroy();

  static void require(std::vector<const char *> &extensions) {
    if (enabled) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      logInfo("Validation layer enabled");
    }
  }
  void inline ref(VkInstance &instance) {
    if (enabled)
      this->instance = instance;
  }

private:
  static std::vector<const char *> layers;
  static const bool enabled;
  VkInstance instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
  VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
};