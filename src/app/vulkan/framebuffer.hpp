#pragma once
#include "app/vulkan/renderpass.hpp"
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/imageview.hpp>
#include <app/vulkan/swapchain.hpp>

class Framebuffer {
public:
  void destroy();
  Framebuffer &setBase(ImageView &imageView, RenderPass &renderPass);
  Framebuffer &build();
  inline const VkFramebuffer get(uint32_t i) { return framebuffers[i]; }

private:
  RenderPass renderPass;
  ImageView imageView;
  std::vector<VkFramebuffer> framebuffers;
};