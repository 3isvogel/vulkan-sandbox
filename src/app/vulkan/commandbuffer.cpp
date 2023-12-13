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
  this->commandPool = commandPool;
  allocInfo.commandPool = commandPool.get();
  pass;
}

CommandBuffer &CommandBuffer::bind(Pipeline &pipeline) {
  this->pipeline = pipeline;
  pass;
}

CommandBuffer &CommandBuffer::build() {
  if (vkAllocateCommandBuffers(commandPool.getDevice().get(), &allocInfo,
                               &commandBuffer) != VK_SUCCESS) {
    e_runtime("Failed to allocate command buffers");
  }
  logDebug("Command buffers: allocated");
  pass;
}

CommandBuffer &CommandBuffer::record(uint32_t bufferId) {

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    e_runtime("Failed to begin recording command buffer");
  }

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 0.0f}}};

  auto renderPass = pipeline.getRenderPass();

  renderPass.setFramebuffer(framebuffer.get(bufferId));

  auto renderPassBeginInfo = renderPass.getInfo();

  vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline.get());

  updateDynamicStates(renderPass);

  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    e_runtime("Failed to record command buffer");
  }
  pass;
}

void CommandBuffer::updateDynamicStates(RenderPass &renderPass) {
  auto extent = renderPass.getsSwapChain().getExtent();
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  scissor.extent = extent;
}

CommandBuffer &CommandBuffer::attach(Framebuffer &framebuffer) {
  this->framebuffer = framebuffer;
  pass;
}