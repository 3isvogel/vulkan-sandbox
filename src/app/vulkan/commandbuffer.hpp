#pragma once
#include "app/vulkan/swapchain.hpp"
#include <app/vulkan/commandpool.hpp>
#include <app/vulkan/framebuffer.hpp>
#include <app/vulkan/pipeline.hpp>
#include <app/vulkan/sync.hpp>
#include <vulkan/vulkan_core.h>

class CommandBuffer {
public:
  CommandBuffer();
  CommandBuffer &build();
  void close();
  CommandBuffer &bind(CommandPool &commandPool);
  CommandBuffer &bind(Pipeline &pipeline);
  CommandBuffer &attach(Framebuffer &framebuffer);
  void record();
  inline void reset() { vkResetCommandBuffer(commandBuffers[currentImage], 0); }
  inline void submitCommand() {
    sync.submitCommand(commandBuffers[CURRENT_FRAME]);
  }
  inline void acquireNextImage() { sync.acquireNextImage(currentImage); }
  inline void present() {
    sync.present(currentImage);
    CURRENT_FRAME = (currentImage + 1) % MAX_FRAMES_IN_FLIGHT;
  }

private:
  void check();
  void updateDynamicStates(RenderPass *renderPass);

private:
  CommandPool *commandPool = nullptr;
  Pipeline *pipeline = nullptr;
  Framebuffer *framebuffer = nullptr;
  Sync sync;

  std::vector<VkCommandBuffer> commandBuffers;
  VkCommandBufferAllocateInfo allocInfo;
  VkCommandBufferBeginInfo beginInfo;

  VkViewport viewport;
  VkRect2D scissor;

  bool dynamicStates = false;
  uint32_t currentImage;
};