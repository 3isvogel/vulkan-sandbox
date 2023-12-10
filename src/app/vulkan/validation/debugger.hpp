#pragma once

#include <vulkan/vulkan.h>

VkDebugUtilsMessengerCreateInfoEXT &getDefaultDebugMessengerCreateInfo();
void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo);