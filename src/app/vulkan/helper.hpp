#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/queues.hpp>
#include <vector>

#define DEVICE_EXTENSIONS_LIST X(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
extern std::vector<const char *> deviceExtensions;

std::vector<const char *> getRequiredExtensions();

void printExtensionSupport();
bool requireExtensionIfAvailable(std::vector<const char *> &extensions,
                                 const char *ext);
void printPhysicalDeviceExtensionProperties(VkPhysicalDevice physicalDevice);
bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

void enforceDeviceExtensionsRequirements(VkPhysicalDevice physicalDevice);