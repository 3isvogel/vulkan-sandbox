#include <app/vulkan/window.hpp>
#include <lib/log.hpp>
#include <stdexcept>

Window::Window() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void Window::destroy() {
  glfwDestroyWindow(window);
  logDebug("GLFW Window: destroyed");
  glfwTerminate();
}

Window &Window::create(uint32_t width, uint32_t height, const char *name) {

  window = glfwCreateWindow(width, height, name, nullptr, nullptr);
  if (window == nullptr) {
    e_runtime("Failed to create GLFW window");
  }
  logDebug("GLFW window: created \"%s\" (%dx%d)", name, width, height);

  return *this;
}