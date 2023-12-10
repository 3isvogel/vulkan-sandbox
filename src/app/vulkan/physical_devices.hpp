#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

#define DEVICE_EXTENSIONS_LIST X(VK_KHR_SWAPCHAIN_EXTENSION_NAME)

bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
std::vector<const char *>
enforceDeviceExtensionsRequirements(VkPhysicalDevice physicalDevice);