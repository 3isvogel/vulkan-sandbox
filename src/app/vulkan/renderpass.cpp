#include "app/vulkan/swapchains.hpp"
#include <app/app.hpp>
#include <app/vulkan/renderpass.hpp>
#include <lib/log.hpp>

RenderPass::RenderPass() = default;

// I have no idea what is going on so i'll link it for later
// https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/02_Graphics_pipeline_basics/03_Render_passes.html#_attachment_description
RenderPass::RenderPass(VkDevice device, SwapChain swapChain) : device(device) {
  VkAttachmentDescription colorAttachment{
      .format = swapChain.imageFormat,
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

  VkRenderPassCreateInfo renderPassInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &colorAttachment,
      .subpassCount = 1,
      .pSubpasses = &subpass};

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
      VK_SUCCESS) {
    e_runtime("Failed to create render pass");
  }
  logDebug("Render pass: created");
}

void RenderPass::destroy() { vkDestroyRenderPass(device, renderPass, nullptr); }