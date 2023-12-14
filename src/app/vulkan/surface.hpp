#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/instance.hpp>
#include <app/vulkan/window.hpp>
#include <vulkan/vulkan_core.h>

class Surface {
public:
  Surface();
  void destroy();
  Surface &setBase(Instance &instance, Window &window);
  Surface &build();

  inline const VkSurfaceKHR get() { return surface; }
  inline Instance *getInstance() { return instance; }
  inline Window *getWindow() { return window; }

private:
  Instance *instance;
  Window *window;
  VkSurfaceKHR surface;
};
