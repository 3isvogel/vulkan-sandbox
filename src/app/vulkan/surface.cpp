#include "app/vulkan/instance.hpp"
#include "lib/codes.hpp"
#include "lib/log.hpp"
#include "lib/macros.hpp"
#include <app/vulkan/surface.hpp>

Surface &Surface::setBase(Instance &instance, Window &window) {
  this->instance = &instance;
  this->window = &window;
  pass;
}

Surface &Surface::build() {
  check();
  auto result = glfwCreateWindowSurface(instance->get(), window->get(), nullptr,
                                        &surface);
  if (result != VK_SUCCESS) {
    // Putting it here since it's where it's most likely to break when MoltenVK,
    // Vulkan or MacOS updates and everything breaks down (just happened)
    logWarning(":/ did you update something?: %s", meanErrorCode(result));
    e_runtime("Failed to create window surface");
  }
  logDebug("GLFW Surface: created");

  pass;
}

void Surface::destroy() {
  vkDestroySurfaceKHR(instance->get(), surface, nullptr);
  logDebug("GLFW surface: destroyed");
}

void Surface::check() {
  if (!instance)
    runtime_dep(Surface, Instance);
  if (!window)
    runtime_dep(Surface, Window);
}
