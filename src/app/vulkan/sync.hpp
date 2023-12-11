#pragma once
#include "app/vulkan/swapchains.hpp"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Sync {
public:
  Sync();
  Sync(VkDevice &device, SwapChain &swapChain);
  void destroy();
  void acquireNextImage(uint32_t &imageIndex);
  void submitCommand(VkCommandBuffer &commandBuffer, VkQueue &graphicsQueue);
  void presentQueue(VkQueue &presentQueue, uint32_t &imageIndex);

  VkSemaphore imageAvailabelSemaphore;
  VkSemaphore renderFinishedSemaphore;

  VkFence inFlightFence;

private:
  inline void inflightWaitAndreset() {
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);
  }
  VkDevice device;
  SwapChain swapChain;
};