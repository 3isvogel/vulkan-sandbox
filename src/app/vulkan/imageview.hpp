#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/logicaldevice.hpp>
#include <app/vulkan/swapchain.hpp>
#include <vector>

class ImageView {
public:
  void destroy();
  ImageView &bind(SwapChain &swapChain);
  ImageView &build();
  inline const std::vector<VkImageView> get() { return views; }

private:
  SwapChain *swapChain;
  std::vector<VkImageView> views;
};