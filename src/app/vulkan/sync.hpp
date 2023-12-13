#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/swapchain.hpp>
#include <cstdint>

class Sync {
public:
  void destroy();
  Sync &connect(SwapChain &swapChain);
  Sync &build();
  void acquireNextImage(uint32_t &imageIndex);
  void submitCommand(VkCommandBuffer commandBuffer);
  void present(uint32_t &imageIndex);

private:
  inline void inflightWaitAndReset() {
    vkWaitForFences(device, 1, &inFlight.fence, VK_TRUE, inFlight.timeout);
    vkResetFences(device, 1, &inFlight.fence);
  }
  VkDevice device;
  SwapChain swapChainRef;
  VkSwapchainKHR swapChain;
  VkQueue graphicsQueue;
  VkQueue presentQueue;

  struct {
    VkSemaphore semaphore;
    uint64_t timeout = UINT64_MAX;
  } imageAvailable, renderFinished;

  struct {
    VkFence fence;
    uint64_t timeout = UINT64_MAX;
  } inFlight;
};