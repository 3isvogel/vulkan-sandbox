#include <app/vulkan/swapchain.hpp>
#include <app/vulkan/sync.hpp>
#include <lib/log.hpp>
#include <lib/macros.hpp>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

uint32_t CURRENT_FRAME = 0;
const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

Sync &Sync::build() {
  check();

  imageAvailable.semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinished.semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  inFlight.fences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

  VkFenceCreateInfo fenceInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      // ensures the fence to be signaled before the first frame
      .flags = VK_FENCE_CREATE_SIGNALED_BIT};

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &imageAvailable.semaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &renderFinished.semaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &inFlight.fences[i]) !=
            VK_SUCCESS) {
      e_runtime("Failed to create Sync");
    }
  }

  pass;
}

Sync &Sync::bind(VkSwapchainKHR swapChain) {
  this->swapChain = swapChain;
  pass;
}

Sync &Sync::bind(VkDevice device) {
  this->device = device;
  pass;
}
Sync &Sync::bind(VkQueue graphicsQueue, VkQueue presentQueue) {
  this->graphicsQueue = graphicsQueue;
  this->presentQueue = presentQueue;
  pass;
}

void Sync::destroy() {
  // TODO: I could just use vector.size()
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, imageAvailable.semaphores[i], nullptr);
    vkDestroySemaphore(device, renderFinished.semaphores[i], nullptr);
    vkDestroyFence(device, inFlight.fences[i], nullptr);
    logDebug("Semaphores: destroyed");
  }
}

void Sync::acquireNextImage(uint32_t &imageIndex) {

  inflightWaitAndReset();
  vkAcquireNextImageKHR(device, swapChain, imageAvailable.timeout,
                        imageAvailable.semaphores[CURRENT_FRAME],
                        VK_NULL_HANDLE, &imageIndex);
}

void Sync::submitCommand(VkCommandBuffer commandBuffer) {

  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo submitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &imageAvailable.semaphores[CURRENT_FRAME],
      .pWaitDstStageMask = waitStages,
      .commandBufferCount = 1,
      .pCommandBuffers = &commandBuffer,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &renderFinished.semaphores[CURRENT_FRAME]};

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo,
                    inFlight.fences[CURRENT_FRAME])) {
    e_runtime("Failed to submit draw command buffer");
  }
}

void Sync::present(uint32_t &imageIndex) {

  VkPresentInfoKHR presentInfo{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                               .waitSemaphoreCount = 1,
                               .pWaitSemaphores =
                                   &renderFinished.semaphores[CURRENT_FRAME],
                               .swapchainCount = 1,
                               // was previously handled as a vector
                               .pSwapchains = &swapChain,
                               .pImageIndices = &imageIndex};

  vkQueuePresentKHR(presentQueue, &presentInfo);
}

void Sync::check() {
  if (!device)
    runtime_dep(Sync, VkLogicalDevice);
  if (!graphicsQueue)
    runtime_dep(Sync, VkQueue(graphics));
  if (!presentQueue)
    runtime_dep(Sync, VkQueue(present));
}

void Sync::inflightWaitAndReset() {
  vkWaitForFences(device, 1, &inFlight.fences[CURRENT_FRAME], VK_TRUE,
                  inFlight.timeout);
  vkResetFences(device, 1, &inFlight.fences[CURRENT_FRAME]);
}