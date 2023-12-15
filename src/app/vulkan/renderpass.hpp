#pragma once
#include <app/vulkan/swapchain.hpp>
#include <vulkan/vulkan_core.h>

class RenderPass {
public:
  void destroy();
  RenderPass &build();
  RenderPass &bind(SwapChain &swapChain);
  void setFramebuffer(VkFramebuffer frameBuffer);
  inline const VkRenderPass get() { return renderPass; }
  inline const VkRenderPassBeginInfo getInfo() { return info; }
  inline SwapChain *getSwapChain() { return swapChain; }

private:
  void check();

private:
  SwapChain *swapChain = nullptr;
  VkRenderPass renderPass;
  VkRenderPassCreateInfo renderPassCreateInfo;

  VkRenderPassBeginInfo info;
  static VkClearValue clear;
};