#include "app/vulkan/renderpass.hpp"
#include "vulkan/vulkan_core.h"
#include <app/vulkan/framebuffer.hpp>
#include <app/vulkan/imageview.hpp>

Framebuffer &Framebuffer::setBase(ImageView &imageView,
                                  RenderPass &renderPass) {
  this->renderPass = &renderPass;
  this->imageView = &imageView;
  pass;
}

Framebuffer &Framebuffer::build() {
  auto imageViews = imageView->get();
  auto swapChain = renderPass->getsSwapChain();
  framebuffers.resize(imageView->get().size());

  size_t i = 0;
  for (; i < imageView->get().size(); i++) {
    VkImageView attachments[] = {imageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass->get(),
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = swapChain->getExtent().width,
        .height = swapChain->getExtent().height,
        .layers = 1};
    if (vkCreateFramebuffer(swapChain->getDevice()->get(), &framebufferInfo,
                            nullptr, &framebuffers[i]) != VK_SUCCESS) {
      e_runtime("Failed to create framebuffer");
    }
  }
  logDebug("Framebuffers: created [%i]", i);
  pass;
}

void Framebuffer::destroy() {
  auto device = renderPass->getsSwapChain()->getDevice()->get();
  for (auto framebuffer : framebuffers)
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  logDebug("Framebuffers: destroyed");
}