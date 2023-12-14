#include "app/vulkan/swapchain.hpp"
#include "vulkan/vulkan_core.h"
#include <app/vulkan/renderpass.hpp>
#include <lib/log.hpp>

RenderPass::RenderPass() = default;

// I have no idea what is going on so i'll link it for later
// https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/02_Graphics_pipeline_basics/03_Render_passes.html#_attachment_description
RenderPass &RenderPass::bind(SwapChain &swapChain) {
  this->swapChain = swapChain;
  pass;
}

RenderPass &RenderPass::build() {
  VkAttachmentDescription colorAttachment{
      .format = swapChain.getFormat(),
      .samples = VK_SAMPLE_COUNT_1_BIT,
      // TODO: allow more advanced selecting
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

  // Useful for post processing (maybe?)
  // https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/02_Graphics_pipeline_basics/03_Render_passes.html#_subpasses_and_attachment_references
  VkAttachmentReference colorAttachmentRef{
      .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

  // This is really bad, I should structure this in a better way later
  VkSubpassDescription subpass{.pipelineBindPoint =
                                   VK_PIPELINE_BIND_POINT_GRAPHICS,
                               .colorAttachmentCount = 1,
                               .pColorAttachments = &colorAttachmentRef};

  // TODO: also change this
  VkSubpassDependency dependency{
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT};

  renderPassCreateInfo = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                          .attachmentCount = 1,
                          .pAttachments = &colorAttachment,
                          .subpassCount = 1,
                          .pSubpasses = &subpass,
                          .dependencyCount = 1,
                          .pDependencies = &dependency};

  if (vkCreateRenderPass(swapChain.getDevice().get(), &renderPassCreateInfo,
                         nullptr, &renderPass) != VK_SUCCESS) {
    e_runtime("Failed to create render pass");
  }
  logDebug("Render pass: created");

  info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = renderPass,
      .renderArea = {.offset = {0, 0}, .extent = swapChain.getExtent()},
      // What to use when defined (VK_ATTACHMENT_LOAD_OP_CLEAR) to clear screen
      .clearValueCount = 1,
      .pClearValues = &clear};

  pass;
}

void RenderPass::destroy() {
  vkDestroyRenderPass(swapChain.getDevice().get(), renderPass, nullptr);
  logDebug("Render pass: destroyed");
}

void RenderPass::setFramebuffer(VkFramebuffer frameBuffer) {
  info.framebuffer = frameBuffer;
}

RenderPass &RenderPass::setClearValue(VkClearValue clearValue) {
  clear = clearValue;
  pass;
}