#pragma once
#include "app/vulkan/commandbuffer.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <app/vulkan/framebuffer.hpp>
#include <app/vulkan/imageview.hpp>
#include <app/vulkan/instance.hpp>
#include <app/vulkan/logicaldevice.hpp>
#include <app/vulkan/physicaldevice.hpp>
#include <app/vulkan/pipeline.hpp>
#include <app/vulkan/queuefamily.hpp>
#include <app/vulkan/renderpass.hpp>
#include <app/vulkan/surface.hpp>
#include <app/vulkan/swapchain.hpp>
#include <app/vulkan/sync.hpp>
#include <app/vulkan/window.hpp>
#include <vector>

class MainApp {
public:
  MainApp(std::string name) : name(name){};
  MainApp(int width, int height, std::string name);
  void run();

private:
  const uint32_t width = 800, height = 600;
  std::string name;

  RenderPass renderPass;
  Pipeline pipeline;
  Window window;
  Instance instance;
  Surface surface;
  PhysicalDevice physicalDevice;
  LogicalDevice logicalDevice;
  Sync sync;
  SwapChain swapChain;
  Framebuffer framebuffer;
  CommandPool commandPool;
  CommandBuffer commandBuffer;

private:
  // Initialize window (this allow for displaying)
  void initWindow();
  void initVulkan();
  void mainLoop();
  void cleanup();
  void drawFrame();
};