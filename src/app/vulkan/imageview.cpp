#include <app/vulkan/imageview.hpp>
#include <app/vulkan/swapchain.hpp>
#include <lib/log.hpp>

ImageView &ImageView::bind(SwapChain *swapChain) {
  this->swapChain = swapChain;
  pass;
}

ImageView &ImageView::build() {
  auto images = swapChain->getImages();
  views.resize(images.size());
  for (size_t i = 0; i < images.size(); i++) {
    VkImageViewCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = images[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapChain->getFormat(),
        .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .b = VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};

    if (vkCreateImageView(swapChain->getDevice()->get(), &createInfo, nullptr,
                          &views[i]) != VK_SUCCESS) {
      e_runtime("Failed to create image view");
    }
  }
  logDebug("Image views: created [%i]", views.size());
  pass;
}

void ImageView::destroy() {
  auto device = swapChain->getDevice()->get();
  for (auto imageView : views) {
    vkDestroyImageView(device, imageView, nullptr);
  }
  logDebug("Image views: destroyed");
}