#pragma once

#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  bool isComplete();
};

extern float defaultQueuePriority;

std::vector<const char *> getRequiredExtensions();
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
void printExtensionSupport();
void printPhysicalDeviceExtensionProperties(VkPhysicalDevice physicalDevice);
bool isDeviceSuitable(VkPhysicalDevice device);

void enforceDeviceExtensionsRequirements(VkDeviceCreateInfo &createInfo,
                                         VkPhysicalDevice physicalDevice);