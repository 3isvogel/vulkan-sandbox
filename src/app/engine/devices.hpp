#pragma once
#include "vulkan.hpp"
#include <GLFW/glfw3.h>

// truct to internally handle swapchain capabilities
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);

// select the best or the first compatible device
Status pickPhysicalDevice(VkPhysicalDevice &physicalDevice, VkInstance instance,
                          VkSurfaceKHR surface, const EngineConf &conf);

// gets information about available queue families for the desired device
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surface);

Status createLogicalDevice(VkDevice &device, VkPhysicalDevice physicalDevice,
                           VkSurfaceKHR surface, const EngineConf &conf);
