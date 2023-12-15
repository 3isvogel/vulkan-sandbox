#pragma once

#include <app/vulkan/swapchain.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

class ImageView {
public:
  void destroy();
  ImageView &bind(SwapChain *swapChain);
  ImageView &build();
  inline const std::vector<VkImageView> get() { return views; }

private:
  SwapChain *swapChain = nullptr;
  std::vector<VkImageView> views;
};