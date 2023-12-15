#pragma once
#include <app/vulkan/instance.hpp>
#include <app/vulkan/window.hpp>
#include <vulkan/vulkan_core.h>

class Surface {
public:
  void destroy();
  Surface &setBase(Instance &instance, Window &window);
  Surface &build();

  inline const VkSurfaceKHR get() { return surface; }
  inline Instance *getInstance() { return instance; }
  inline Window *getWindow() { return window; }

private:
  void check();

private:
  Instance *instance = nullptr;
  Window *window = nullptr;
  VkSurfaceKHR surface;
};
