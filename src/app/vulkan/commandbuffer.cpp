#include "app/vulkan/renderpass.hpp"
#include "app/vulkan/swapchain.hpp"
#include "lib/macros.hpp"
#include <app/vulkan/commandbuffer.hpp>
#include <lib/log.hpp>
#include <stdexcept>
#include <vector>

CommandBuffer::CommandBuffer() {
  // TODO: handle this better
  allocInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
               .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
               .commandBufferCount = 1};

  beginInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
               // Optional
               .flags = 0,
               .pInheritanceInfo = nullptr};

  viewport = {.x = 0.0f, .y = 0.0f, .minDepth = 0.0f, .maxDepth = 1.0f};
  scissor.offset = {0, 0};
}

CommandBuffer &CommandBuffer::bind(CommandPool &commandPool) {
  this->commandPool = &commandPool;
  allocInfo.commandPool = commandPool.get();
  pass;
}

CommandBuffer &CommandBuffer::bind(Pipeline &pipeline) {
  this->pipeline = &pipeline;
  dynamicStates = pipeline.getDynamic();
  pass;
}

CommandBuffer &CommandBuffer::build() {
  check();
  auto swapChain = pipeline->getRenderPass()->getSwapChain();
  auto device = swapChain->getDevice();
  auto queues = device->getQueue();

  sync.bind(device->get())
      .bind(swapChain->get())
      .bind(device->getQueue()->graphicsQueue,
            device->getQueue()->presentQueue);

  commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
  if (vkAllocateCommandBuffers(device->get(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    e_runtime("Failed to allocate command buffers");
  }
  logDebug("Command buffers: allocated");
  sync.build();
  pass;
}

void CommandBuffer::record() {

  if (vkBeginCommandBuffer(commandBuffers[currentImage], &beginInfo) !=
      VK_SUCCESS) {
    e_runtime("Failed to begin recording command buffer");
  }

  auto renderPass = pipeline->getRenderPass();

  renderPass->setFramebuffer(framebuffer->get(currentImage));

  auto renderPassBeginInfo = renderPass->getInfo();

  vkCmdBeginRenderPass(commandBuffers[currentImage], &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffers[currentImage],
                    VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->get());

  if (dynamicStates) {
    updateDynamicStates(renderPass);
  }

  vkCmdSetViewport(commandBuffers[currentImage], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[currentImage], 0, 1, &scissor);

  vkCmdDraw(commandBuffers[currentImage], 3, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffers[currentImage]);

  if (vkEndCommandBuffer(commandBuffers[currentImage]) != VK_SUCCESS) {
    e_runtime("Failed to record command buffer");
  }
}

void CommandBuffer::updateDynamicStates(RenderPass *renderPass) {
  auto extent = renderPass->getSwapChain()->getExtent();
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  scissor.extent = extent;
}

CommandBuffer &CommandBuffer::attach(Framebuffer &framebuffer) {
  this->framebuffer = &framebuffer;
  pass;
}

void CommandBuffer::check() {
  if (!commandPool)
    runtime_dep(CommandBuffer, CommandPool);
  if (!pipeline)
    runtime_dep(CommandBuffer, Pipeline);
  if (!framebuffer)
    runtime_dep(CommandBuffer, Framebuffer);
}

void CommandBuffer::close() { sync.destroy(); }