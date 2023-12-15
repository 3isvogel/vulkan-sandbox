#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

extern uint32_t CURRENT_FRAME;
extern const uint32_t MAX_FRAMES_IN_FLIGHT;

// FIXME: May want to make sync a swapchain element
class Sync {
public:
  void destroy();
  Sync &bind(VkSwapchainKHR swapChain);
  Sync &bind(VkDevice device);
  Sync &bind(VkQueue graphicsQueue, VkQueue presentQueue);
  Sync &build();
  void acquireNextImage(uint32_t &imageIndex);
  void submitCommand(VkCommandBuffer commandBuffer);
  void present(uint32_t &imageIndex);

private:
  void check();
  void inflightWaitAndReset();
  VkDevice device = nullptr;
  VkSwapchainKHR swapChain = nullptr;
  VkQueue graphicsQueue = nullptr;
  VkQueue presentQueue = nullptr;

  struct {
    std::vector<VkSemaphore> semaphores;
    uint64_t timeout = UINT64_MAX;
  } imageAvailable, renderFinished;

  struct {
    std::vector<VkFence> fences;
    uint64_t timeout = UINT64_MAX;
  } inFlight;
};