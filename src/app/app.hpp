#pragma once
#include "engine/vulkan.hpp"
#include <GLFW/glfw3.h>
#include <lib/types.h>

class EngineApp {
public:
  // main application loop
  Status run();

private:
  EngineConf conf;
  uint64_t seed; // not the time to add it, but I am gonna do it anyway

private:
  GLFWwindow *window = nullptr;
  VkInstance instance = VK_NULL_HANDLE;
  // represents the actual device
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  // Struct for queues indexing
  QueueFamilyIndices indices;
  // virtual device, for interfacing with the physical device
  VkDevice device = VK_NULL_HANDLE;
  // queues handler
  uint32_t imageCount = 0;
  // swapchain handler
  VkSwapchainKHR swapChain = VK_NULL_HANDLE;
  // images themself
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainIMageViews;

#define x(n) VkQueue n##Queue = VK_NULL_HANDLE;
  QUEUE_HANDLER_LIST
#undef x
  // surface, to handle window drawing
  VkSurfaceKHR surface = VK_NULL_HANDLE;

  Status setSeed();

  void config();

  Status initWindow();
  Status initVulkan();
  Status mainLoop();
  void cleanup();

  void bindQueueHandler();
};
