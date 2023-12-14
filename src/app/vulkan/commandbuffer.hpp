#pragma once
#include "app/vulkan/framebuffer.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/commandpool.hpp>
#include <app/vulkan/pipeline.hpp>

class CommandBuffer {
public:
  CommandBuffer();
  void destroy();
  CommandBuffer &build();
  CommandBuffer &bind(CommandPool &commandPool);
  CommandBuffer &bind(Pipeline &pipeline);
  CommandBuffer &attach(Framebuffer &framebuffer);
  CommandBuffer &record(uint32_t bufferIdx);
  inline const VkCommandBuffer get() { return commandBuffer; }
  inline CommandBuffer &reset() {
    vkResetCommandBuffer(commandBuffer, 0);
    pass;
  }

private:
  void updateDynamicStates(RenderPass *renderPass);

private:
  CommandPool *commandPool;
  Pipeline *pipeline;
  Framebuffer *framebuffer;

  VkCommandBuffer commandBuffer;
  VkCommandBufferAllocateInfo allocInfo;
  VkCommandBufferBeginInfo beginInfo;

  VkViewport viewport;
  VkRect2D scissor;

  bool dynamicStates = false;
};