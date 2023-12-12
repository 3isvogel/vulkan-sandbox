#pragma once
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

std::vector<VkExtensionProperties> queryInstanceExtensionProperties();
bool requireExtensionIfAvailable(std::vector<const char *> &extensions,
                                 const char *ext);
std::vector<const char *> getRequiredExtensions();