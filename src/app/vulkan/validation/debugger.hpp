#pragma once

#include "vulkan/vulkan_core.h"
#include <app/vulkan/validation/callbacks.hpp>
#include <vulkan/vulkan.h>

VkDebugUtilsMessengerCreateInfoEXT &getDefaultDebugMessengerCreateInfo();
void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo);