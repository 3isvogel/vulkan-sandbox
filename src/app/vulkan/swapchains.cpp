#include <app/app.hpp>
#include <app/vulkan/swapchains.hpp>
#include <lib/log.hpp>
#include <vector>

void retrieveSwapChainImages(VkSwapchainKHR swapChain,
                             std::vector<VkImage> &swapChainImages);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface) {

  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);
  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

// NOTE: It's called "chooseBest..." because it selects the "best one" and
// fallbacks to 0 when no other is available, may implement later a more
// advanced selectino method
VkSurfaceFormatKHR chooseBestSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats)
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return availableFormat;

  return availableFormats[0];
}

bool tryEnableTripleBuffering = false;
VkPresentModeKHR chooseBestSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {

  if (tryEnableTripleBuffering)
    for (auto const &availablePresentMode : availablePresentModes)
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        return availablePresentMode;

  return VK_PRESENT_MODE_FIFO_KHR;
}

// Just query GLFW UwU
VkExtent2D chooseBestSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                                GLFWwindow *window) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

void MainApp::createSwapChain() {
  SwapChainSupportDetails swapChainSupport =
      querySwapChainSupport(physicalDevice, surface);

  VkSurfaceFormatKHR surfaceFormat =
      chooseBestSwapSurfaceFormat(swapChainSupport.formats);

  VkPresentModeKHR presentMode =
      chooseBestSwapPresentMode(swapChainSupport.presentModes);

  VkExtent2D extent =
      chooseBestSwapExtent(swapChainSupport.capabilities, window);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount;

#ifdef ADDITIONAL_SWAPCHAIN_IMAGES
  imageCount += ADDITIONAL_SWAPCHAIN_IMAGES;
#endif

  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount)
    imageCount = swapChainSupport.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface,
      .minImageCount = imageCount,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT};

  QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &(swapChain.chain)) !=
      VK_SUCCESS) {
    e_runtime("Failed to create swapchain");
  }
  logDebug("SwapChain: created");

  vkGetSwapchainImagesKHR(device, swapChain.chain, &imageCount, nullptr);
  swapChain.images.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain.chain, &imageCount,
                          swapChain.images.data());

  swapChain.imageFormat = surfaceFormat.format;
  swapChain.extent = extent;

  logDebug("SwapChain info: retrieved");
}

void MainApp::createImageViews() {
  swapChain.imageViews.resize(swapChain.images.size());
  for (size_t i = 0; i < swapChain.images.size(); i++) {
    VkImageViewCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = swapChain.images[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapChain.imageFormat,
        .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .b = VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};

    if (vkCreateImageView(device, &createInfo, nullptr,
                          &(swapChain.imageViews[i])) != VK_SUCCESS) {
      e_runtime("Failed to create image view");
    }
  }
  logDebug("Image views: created");
}