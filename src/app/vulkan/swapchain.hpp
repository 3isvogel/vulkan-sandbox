#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/additional_types.hpp>
#include <app/vulkan/logicaldevice.hpp>
#include <app/vulkan/physicaldevice.hpp>
#include <app/vulkan/surface.hpp>
#include <app/vulkan/window.hpp>
#include <vector>

// #define ADDITIONAL_SWAPCHAIN_IMAGES 1

class SwapChain {
public:
  void destroy();
  SwapChain &connect(LogicalDevice &device, Window &window);
  SwapChain &build();
  inline SwapChain &askTripleBuffering() {
    tripleBuffering = true;
    return *this;
  }
  inline LogicalDevice &getDevice() { return device; }
  inline const VkSwapchainKHR get() { return swapChain; }
  inline const std::vector<VkImage> getImages() { return images; }
  inline const VkFormat getFormat() { return imageFormat; }
  inline const VkExtent2D getExtent() { return extent; }

private:
  VkSurfaceFormatKHR bestFormat();
  VkPresentModeKHR bestPresentMode();
  VkExtent2D bestExtent();

  std::vector<VkImage> images;
  VkFormat imageFormat;
  VkSwapchainKHR swapChain;

  VkExtent2D extent;
  SwapChainSupportDetails details;

  LogicalDevice device;
  Window window;

  bool tripleBuffering = false;
};