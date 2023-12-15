#pragma once
#include <app/vulkan/additional_types.hpp>
#include <app/vulkan/logicaldevice.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

// #define ADDITIONAL_SWAPCHAIN_IMAGES 1
// TODO: move these in a better place

class SwapChain {
public:
  void destroy();
  SwapChain &connect(LogicalDevice &device);
  SwapChain &build();
  inline SwapChain &askTripleBuffering() {
    tripleBuffering = true;
    return *this;
  }
  inline LogicalDevice *getDevice() { return device; }
  inline const VkSwapchainKHR get() { return swapChain; }
  inline const std::vector<VkImage> &getImages() { return images; }
  inline const VkFormat getFormat() { return imageFormat; }
  inline const VkExtent2D getExtent() { return extent; }

private:
  void check();

  VkSurfaceFormatKHR bestFormat();
  VkPresentModeKHR bestPresentMode();
  VkExtent2D bestExtent();

  std::vector<VkImage> images;
  VkFormat imageFormat;
  VkSwapchainKHR swapChain;

  VkExtent2D extent;
  SwapChainSupportDetails supportDetails;

  LogicalDevice *device = nullptr;

  bool tripleBuffering = false;
};