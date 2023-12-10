#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>

// MF managed to put X macro inside his code
#define FAMILIES_LIST                                                          \
  X(graphics)                                                                  \
  X(present)

struct QueueFamilyIndices {
#define X(name) std::optional<uint32_t> name##Family;
  FAMILIES_LIST
#undef X
  bool isComplete();
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surface);