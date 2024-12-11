#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <lib/types.hpp>

Status createSurface(VkSurfaceKHR &surface, VkInstance instance,
                     GLFWwindow *window);