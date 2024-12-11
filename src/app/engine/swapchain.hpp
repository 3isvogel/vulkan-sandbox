#pragma once

#include "vulkan.hpp"
#include <GLFW/glfw3.h>

void getSwapchainImages(std::vector<VkImage> &swapChainImages,
                        uint32_t &imageCount, VkSwapchainKHR swapChain,
                        VkDevice device);

Status createImageViews(std::vector<VkImageView> &swapChainIMageViews,
                        std::vector<VkImage> &swapChainImages, VkDevice device,
                        const EngineConf &conf);

Status createSwapChain(VkSwapchainKHR &swapChain, VkSurfaceKHR surface,
                       GLFWwindow *window, VkPhysicalDevice physicalDevice,
                       VkDevice device, EngineConf &conf);