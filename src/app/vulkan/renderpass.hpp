#pragma once
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/swapchain.hpp>

class RenderPass {
public:
  RenderPass();
  void destroy();
  RenderPass &bind(SwapChain &swapChain);
  void setFramebuffer(VkFramebuffer frameBuffer);
  RenderPass &setClearValue(const VkClearValue clearValue);
  RenderPass &build();
  inline const VkRenderPass get() { return renderPass; }
  inline const VkRenderPassBeginInfo getInfo() { return info; }
  inline SwapChain &getsSwapChain() { return swapChain; }

private:
  SwapChain swapChain;
  VkRenderPass renderPass;
  VkRenderPassCreateInfo renderPassCreateInfo;

  VkRenderPassBeginInfo info;
  VkClearValue clear = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
};