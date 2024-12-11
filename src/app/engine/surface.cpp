#include "surface.hpp"
#include <cstddef>
#include <lib/log.hpp>

Status createSurface(VkSurfaceKHR &surface, VkInstance instance,
                     GLFWwindow *window) {
  VkResult result =
      glfwCreateWindowSurface(instance, window, nullptr, &surface);
  if (result != VK_SUCCESS) {
    logDebug("Failed to create window surface: %i", result);
    return Status::SURFACE_CREATION_FAIL;
  }

  return Status::OK;
}
