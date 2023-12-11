#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/pipeline.hpp>
#include <app/vulkan/swapchains.hpp>

void recordCommandBuffer(VkCommandBuffer &commandBuffer, uint32_t imageIndex,
                         VkRenderPass &renderPass, SwapChain &swapChain,
                         Pipeline pipeline);