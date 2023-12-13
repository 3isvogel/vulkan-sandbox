#include "app/vulkan/instance.hpp"
#include <app/vulkan/surface.hpp>

Surface::Surface() {}

Surface &Surface::setBase(Instance &instance, Window &window) {
  this->instance = instance;
  this->window = window;
  pass;
}

Surface &Surface::build() {
  auto result =
      glfwCreateWindowSurface(instance.get(), window.get(), nullptr, &surface);
  if (result != VK_SUCCESS) {
    e_runtime("Failed to create window surface");
  }
  logDebug("GLFW Surface: created");

  pass;
}

void Surface::destroy() {
  vkDestroySurfaceKHR(instance.get(), surface, nullptr);
  logDebug("GLFW surface: destroyed");
}
