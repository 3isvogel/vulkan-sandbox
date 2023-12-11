#include <app/app.hpp>
#include <app/vulkan/commandbuffer.hpp>
#include <lib/log.hpp>
#include <stdexcept>
#include <vector>

void MainApp::recordCommandBuffer(uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      // Optional
      .flags = 0,
      .pInheritanceInfo = nullptr};

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    e_runtime("Failed to begin recording command buffer");
  }

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 0.0f}}};

  VkRenderPassBeginInfo renderPassInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = renderPass.get(),
      .framebuffer = swapChain.framebuffers[imageIndex],
      .renderArea = {.offset = {0, 0}, .extent = swapChain.extent},
      // What to use when defined (VK_ATTACHMENT_LOAD_OP_CLEAR) to clear screen
      .clearValueCount = 1,
      .pClearValues = &clearColor};

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline.get());

  upateDynamicViewport();

  vkCmdSetViewport(commandBuffer, 0, 1, &config.viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &config.scissor);

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    e_runtime("Failed to record command buffer");
  }
}