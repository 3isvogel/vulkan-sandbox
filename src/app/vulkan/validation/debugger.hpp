#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkDebugUtilsMessengerCreateInfoEXT &getDefaultDebugMessengerCreateInfo();
void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo);