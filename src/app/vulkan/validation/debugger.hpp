#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <lib/log.hpp>
#include <vector>

class Validation {
public:
  Validation();
  void destroy();
  Validation &callback(PFN_vkDebugUtilsMessengerCallbackEXT callback);
  Validation &bind(VkInstance &instance);
  Validation &build();
  Validation &populateDebugMessenger(VkInstanceCreateInfo &createInfo);

  static void require(std::vector<const char *> &extensions) {
    if (enabled) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      logInfo("Validation layer enabled");
    }
  }

private:
  void ref(VkInstance instance) { this->instance = instance; }
  void legacyPopulateDeviceSpecific(VkDeviceCreateInfo &createInfo);

private:
  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  VkDebugUtilsMessengerEXT debugMessenger;
  static const bool enabled;
  static std::vector<const char *> layers;
  VkInstance instance = VK_NULL_HANDLE;
};