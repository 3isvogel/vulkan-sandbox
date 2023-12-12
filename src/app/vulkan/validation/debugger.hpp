#pragma once

#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Debugger {
public:
  Debugger();
  Debugger &callback(PFN_vkDebugUtilsMessengerCallbackEXT callback);
  VkDebugUtilsMessengerCreateInfoEXT info() { return debugCreateInfo; }

private:
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
};