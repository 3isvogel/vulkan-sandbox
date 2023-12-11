#pragma once
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/swapchains.hpp>

class RenderPass {
public:
  RenderPass();
  RenderPass(VkDevice device, SwapChain swapChain);
  void destroy();
  VkRenderPass data() { return renderPass; };

private:
  VkDevice device;
  VkRenderPass renderPass;
};