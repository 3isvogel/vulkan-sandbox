#pragma once
#include <vector>
#include <vulkan/vulkan.h>

extern std::vector<const char *> getRequiredExtensions();
extern void checkExtensionSupport();
extern bool checkValidationLayerSupport();
extern bool isDeviceSuitable(VkPhysicalDevice device);