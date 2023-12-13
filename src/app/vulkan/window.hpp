#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window {
public:
  Window();
  void destroy();
  Window &create(uint32_t width, uint32_t height,
                 const char *name = "GLFW window");
  inline GLFWwindow *get() { return window; }
  inline bool running() { return !glfwWindowShouldClose(window); }

private:
  GLFWwindow *window;
};