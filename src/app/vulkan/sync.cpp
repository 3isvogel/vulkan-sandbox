#include <app/vulkan/swapchain.hpp>
#include <app/vulkan/sync.hpp>
#include <lib/log.hpp>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Sync &Sync::build() {

  swapChain = swapChainRef->get();
  device = swapChainRef->getDevice()->get();

  VkSemaphoreCreateInfo semaphoreInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

  VkFenceCreateInfo fenceInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      // ensures the fence to be signaled before the first frame
      .flags = VK_FENCE_CREATE_SIGNALED_BIT};

  if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &imageAvailable.semaphore) != VK_SUCCESS ||
      vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &renderFinished.semaphore) != VK_SUCCESS ||
      vkCreateFence(device, &fenceInfo, nullptr, &inFlight.fence) !=
          VK_SUCCESS) {
    e_runtime("Failed to create Sync");
  }

  graphicsQueue = swapChainRef->getDevice()->getQueue().graphicsQueue;
  presentQueue = swapChainRef->getDevice()->getQueue().presentQueue;

  return *this;
}

Sync &Sync::connect(SwapChain &swapChain) {
  this->swapChainRef = &swapChain;

  return *this;
}

void Sync::destroy() {
  vkDestroySemaphore(device, imageAvailable.semaphore, nullptr);
  vkDestroySemaphore(device, renderFinished.semaphore, nullptr);
  vkDestroyFence(device, inFlight.fence, nullptr);
  logDebug("Semaphores: destroyed");
}

void Sync::acquireNextImage(uint32_t &imageIndex) {

  inflightWaitAndReset();
  vkAcquireNextImageKHR(device, swapChain, imageAvailable.timeout,
                        imageAvailable.semaphore, VK_NULL_HANDLE, &imageIndex);
}

void Sync::submitCommand(VkCommandBuffer commandBuffer) {

  VkSemaphore waitSemaphores[] = {imageAvailable.semaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSemaphore signalSemaphores[] = {renderFinished.semaphore};

  VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                          .waitSemaphoreCount = 1,
                          .pWaitSemaphores = waitSemaphores,
                          .pWaitDstStageMask = waitStages,
                          .commandBufferCount = 1,
                          .pCommandBuffers = &commandBuffer,
                          .signalSemaphoreCount = 1,
                          .pSignalSemaphores = signalSemaphores};

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlight.fence)) {
    e_runtime("Failed to submit draw command buffer");
  }
}

void Sync::present(uint32_t &imageIndex) {

  VkSemaphore signalSemaphores[] = {renderFinished.semaphore};
  VkSwapchainKHR swapChains[] = {swapChain};
  VkPresentInfoKHR presentInfo{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                               .waitSemaphoreCount = 1,
                               .pWaitSemaphores = signalSemaphores,
                               .swapchainCount = 1,
                               // was previously handled as a vector
                               .pSwapchains = swapChains,
                               .pImageIndices = &imageIndex};

  vkQueuePresentKHR(presentQueue, &presentInfo);
}