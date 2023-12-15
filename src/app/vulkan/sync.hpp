#pragma once
#include <app/vulkan/swapchain.hpp>
#include <cstdint>
#include <vulkan/vulkan_core.h>

// FIXME: May want to make sync a swapchain element
class Sync {
public:
  void destroy();
  Sync &connect(SwapChain &swapChain);
  Sync &build();
  void acquireNextImage(uint32_t &imageIndex);
  void submitCommand(VkCommandBuffer commandBuffer);
  void present(uint32_t &imageIndex);

private:
  void check();
  inline void inflightWaitAndReset() {
    vkWaitForFences(device, 1, &inFlight.fence, VK_TRUE, inFlight.timeout);
    vkResetFences(device, 1, &inFlight.fence);
  }
  VkDevice device;
  SwapChain *swapChainRef = nullptr;
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