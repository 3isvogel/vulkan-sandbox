#include "swapchain.hpp"
#include "devices.hpp"

void getSwapchainImages(std::vector<VkImage> &swapChainImages,
                        uint32_t &imageCount, VkSwapchainKHR swapChain,
                        VkDevice device) {
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                          swapChainImages.data());
}

Status createImageViews(std::vector<VkImageView> &swapChainIMageViews,
                        std::vector<VkImage> &swapChainImages, VkDevice device,
                        const EngineConf &conf) {

  swapChainIMageViews.resize(swapChainImages.size());

  for (size_t i = 0; i < swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = conf.swap.imageFormat;
    createInfo.components =
        (VkComponentMapping){.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                             .a = VK_COMPONENT_SWIZZLE_IDENTITY};

    createInfo.subresourceRange =
        (VkImageSubresourceRange){.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .baseMipLevel = 0,
                                  .levelCount = 1,
                                  .baseArrayLayer = 0,
                                  .layerCount = 1};

    VkResult result =
        vkCreateImageView(device, &createInfo, NULL, &swapChainIMageViews[i]);

    if (result != VK_SUCCESS) {
      logDebug("Failed to create imageView[%i]: %i", i, result);
      return Status::IMAGEVIEW_FAIL;
    }
  }

  logDebug("Imageview created (%i)", swapChainIMageViews.size());

  return Status::OK;
}

VkSurfaceFormatKHR
chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats,
                        const EngineConf &conf) {

  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == conf.surface.format &&
        availableFormat.colorSpace == conf.surface.colorSpace) {
      logDebug("Preferred format available");
      return availableFormat;
    }
  }

  logInfo("Preferred format not available, fallback to default");
  return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes,
    const EngineConf &conf) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == conf.swap.present) {
      logDebug("Preferred presentation mode available");
      return availablePresentMode;
    }
  }

  logInfo("Preferred presentation mode not available, fallback to default");
  return VK_PRESENT_MODE_FIFO_KHR;
}

#include <algorithm> // Necessary for std::clamp
#include <cstdint>   // Necessary for uint32_t
#include <limits>    // Necessary for std::numeric_limits

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
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

Status createSwapChain(VkSwapchainKHR &swapChain, VkSurfaceKHR surface,
                       GLFWwindow *window, VkPhysicalDevice physicalDevice,
                       VkDevice device, EngineConf &conf) {
  SwapChainSupportDetails swapChainSupport =
      querySwapChainSupport(physicalDevice, surface);

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupport.formats, conf);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes, conf);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

  // try asking for the minimum image count + 1 for double buffering, clamp to
  // the maximum
  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  // setup createInfo struct for swapchain creation
  VkSwapchainCreateInfoKHR createInfo{};

  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  // > 1 only in stereoscopy
  createInfo.imageArrayLayers = 1;
  // Drawing directly to image, for additional processing before presenting,
  // consider using "VK_IMAGE_USAGE_TRANSFER_DST_BIT"
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
  uint32_t queueFamilyIndices[] = {
      static_cast<uint32_t>(indices.graphicsFamily),
      static_cast<uint32_t>(indices.presentFamily)};

  if (indices.graphicsFamily != indices.presentFamily) {
    // Image usable across multiple queue to move swapchains' images from one
    // queue to another, this option is less performant, if this gets enabled a
    // warning will be raised as a notification, is possible to implement
    // explicit owner transfer if needed
    logWarning("Swapcahin will be set to concurrent mode, if you want to avoid "
               "this, explicit data ownership must be implemented");
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    // Explicit owning transfer to move swapchains' images from one queue to
    // another
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  // set pretransform
  if (conf.swap.transform == VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) {
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  } else {
    createInfo.preTransform = conf.swap.transform;
  }

  // set blending mode
  createInfo.compositeAlpha = conf.swap.compositeAlpha;

  // set presentmode and enable obscured - clipping for performance reasons
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  // only used when recreating swapchain on running app
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkResult result =
      vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);

  if (result != VK_SUCCESS) {
    logDebug("Swapchain creation failed: %i", result);
    return Status::SWAPCHAIN_FAIL;
  }

  logDebug("Swapchain created");
  conf.swap.imageFormat = surfaceFormat.format;
  conf.swap.extent = extent;
  return Status::OK;
}