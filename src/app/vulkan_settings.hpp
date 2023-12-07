#pragma once

#include <vulkan/vulkan_core.h>

extern void createInstance(VkInstance &instance, const char name[]);
extern void setupDebugMessenger(VkInstance &instance,
                                VkDebugUtilsMessengerEXT &debugMessenger);
extern void destroyDebugMessenger(VkInstance &instance,
                                  VkDebugUtilsMessengerEXT &debugMessenger);