#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/swapchain.hpp>

class RenderPass {
public:
  void destroy();
  RenderPass &bind(SwapChain &swapChain);
  void setFramebuffer(VkFramebuffer frameBuffer);
  RenderPass &build();
  inline const VkRenderPass get() { return renderPass; }
  inline const VkRenderPassBeginInfo getInfo() { return info; }
  inline SwapChain *getsSwapChain() { return swapChain; }

private:
  SwapChain *swapChain;
  VkRenderPass renderPass;
  VkRenderPassCreateInfo renderPassCreateInfo;

  VkRenderPassBeginInfo info;
  static VkClearValue clear;
};