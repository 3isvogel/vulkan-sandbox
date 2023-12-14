#pragma once
#include "app/vulkan/logicaldevice.hpp"
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class CommandPool {
public:
  void destroy();
  CommandPool &build();
  CommandPool &bind(LogicalDevice &logicalDevice);
  inline const VkCommandPool get() { return commandPool; }
  inline LogicalDevice *getDevice() { return device; }

private:
  LogicalDevice *device;
  VkCommandPool commandPool;
};