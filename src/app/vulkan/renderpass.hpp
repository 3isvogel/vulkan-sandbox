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
  inline VkRenderPass &get() { return renderPass; };
  inline VkRenderPassCreateInfo &getInfo() { return renderPassInfo; }

private:
  VkDevice device;
  VkRenderPass renderPass;
  VkRenderPassCreateInfo renderPassInfo;
};