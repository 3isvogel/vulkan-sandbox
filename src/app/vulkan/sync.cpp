#include "vulkan/vulkan_core.h"
#include <app/vulkan/sync.hpp>
#include <lib/log.hpp>
#include <stdexcept>

Sync::Sync() = default;

Sync::Sync(VkDevice &device, SwapChain &swapChain)
    : device(device), swapChain(swapChain) {
  VkSemaphoreCreateInfo semaphoreInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

  VkFenceCreateInfo fenceInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      // ensures the fence to be signaled before the first frame
      .flags = VK_FENCE_CREATE_SIGNALED_BIT};

  if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &imageAvailabelSemaphore) != VK_SUCCESS ||
      vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &renderFinishedSemaphore) != VK_SUCCESS ||
      vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) !=
          VK_SUCCESS) {
    e_runtime("Failed to create Sync");
  }
}

void Sync::destroy() {
  vkDestroySemaphore(device, imageAvailabelSemaphore, nullptr);
  vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
  vkDestroyFence(device, inFlightFence, nullptr);
}

void Sync::acquireNextImage(uint32_t &imageIndex) {

  inflightWaitAndreset();
  vkAcquireNextImageKHR(device, swapChain.chain, UINT64_MAX,
                        imageAvailabelSemaphore, VK_NULL_HANDLE, &imageIndex);
}

void Sync::submitCommand(VkCommandBuffer &commandBuffer,
                         VkQueue &graphicsQueue) {

  VkSemaphore waitSemaphores[] = {imageAvailabelSemaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};

  VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                          .waitSemaphoreCount = 1,
                          .pWaitSemaphores = waitSemaphores,
                          .pWaitDstStageMask = waitStages,
                          .commandBufferCount = 1,
                          .pCommandBuffers = &commandBuffer,
                          .signalSemaphoreCount = 1,
                          .pSignalSemaphores = signalSemaphores};

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence)) {
    e_runtime("Failed to submit draw command buffer");
  }
}

void Sync::presentQueue(VkQueue &presentQueue, uint32_t &imageIndex) {

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
  VkSwapchainKHR swapChains[] = {swapChain.chain};
  VkPresentInfoKHR presentInfo{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                               .waitSemaphoreCount = 1,
                               .pWaitSemaphores = signalSemaphores,
                               .swapchainCount = 1,
                               // was previously handled as a vector
                               .pSwapchains = swapChains,
                               .pImageIndices = &imageIndex};

  vkQueuePresentKHR(presentQueue, &presentInfo);
}