#include "app/vulkan/logicaldevice.hpp"
#include "app/vulkan/queuefamily.hpp"
#include <app/vulkan/swapchain.hpp>
#include <lib/log.hpp>
#include <vector>

// NOTE: It's called "chooseBest..." because it selects the "best one" and
// fallbacks to 0 when no other is available, may implement later a more
// advanced selectino method
VkSurfaceFormatKHR SwapChain::bestFormat() {
  for (const auto &availableFormat : details.formats)
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return availableFormat;

  return details.formats[0];
}

SwapChain &SwapChain::connect(LogicalDevice &device, Window &window) {
  device = device;
  window = window;
  pass;
}

VkPresentModeKHR SwapChain::bestPresentMode() {
  if (tripleBuffering)
    for (auto const &availablePresentMode : details.presentModes)
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        logDebug("Triple buffering: requested");
        return availablePresentMode;
      }

  return VK_PRESENT_MODE_FIFO_KHR;
}

// Just query GLFW UwU
VkExtent2D SwapChain::bestExtent() {
  if (details.capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return details.capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window.get(), &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width = std::clamp(actualExtent.width,
                                    details.capabilities.minImageExtent.width,
                                    details.capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height, details.capabilities.minImageExtent.height,
        details.capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

SwapChain &SwapChain::build() {

  auto physicalDevice = device.getPhysicalDevice();
  auto surface = physicalDevice.getSurface();

  VkSurfaceFormatKHR surfaceFormat = bestFormat();
  VkPresentModeKHR presentMode = bestPresentMode();
  extent = bestExtent();

  uint32_t imageCount = details.capabilities.minImageCount;

#ifdef ADDITIONAL_SWAPCHAIN_IMAGES
  imageCount += ADDITIONAL_SWAPCHAIN_IMAGES;
#endif

  if (details.capabilities.maxImageCount > 0 &&
      imageCount > details.capabilities.maxImageCount)
    imageCount = details.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface.get(),
      .minImageCount = imageCount,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .preTransform = details.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = presentMode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr};

  QueueFamily indices = QueueFamily().bind(surface).find(physicalDevice.get());
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }

  if (vkCreateSwapchainKHR(device.get(), &createInfo, nullptr, &swapChain) !=
      VK_SUCCESS) {
    e_runtime("Failed to create swapchain");
  }
  logDebug("Swap chain: created");

  vkGetSwapchainImagesKHR(device.get(), swapChain, &imageCount, nullptr);
  images.resize(imageCount);
  vkGetSwapchainImagesKHR(device.get(), swapChain, &imageCount, images.data());
  imageFormat = surfaceFormat.format;

  return *this;
}

void SwapChain::destroy() {
  vkDestroySwapchainKHR(device.get(), swapChain, nullptr);
  logDebug("SwapChain: destroyed");
}