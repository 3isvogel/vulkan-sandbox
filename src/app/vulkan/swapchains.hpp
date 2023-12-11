#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

// #define ADDITIONAL_SWAPCHAIN_IMAGES 1

typedef struct SwapChain {
  VkSwapchainKHR chain;
  std::vector<VkImage> images;
  VkFormat imageFormat;
  VkExtent2D extent;
  std::vector<VkImageView> imageViews;
  std::vector<VkFramebuffer> framebuffers;
} SwapChain;

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);

VkSurfaceFormatKHR chooseBestSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats);

VkPresentModeKHR chooseBestSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes);

VkExtent2D chooseBestSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                                GLFWwindow *window);