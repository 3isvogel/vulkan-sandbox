#pragma once

#include <app/vulkan/validation/enable.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/pipeline.hpp>
#include <app/vulkan/queues.hpp>
#include <app/vulkan/renderpass.hpp>
#include <app/vulkan/sync.hpp>
#include <vector>

class MainApp {
public:
  MainApp();
  MainApp(int width, int height, const char name[]);

  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  const uint32_t width = 800, height = 600;
  const char *name = "Vulkan app";
  GLFWwindow *window;
  VkInstance instance;
  VkSurfaceKHR surface;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
#define X(name) VkQueue name##Queue;
  FAMILIES_LIST
#undef X
  SwapChain swapChain;

  struct Config {
    VkViewport viewport;
    VkRect2D scissor;
  } config{};

  RenderPass renderPass;
  Pipeline pipeline;
  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;

  Sync sync;
  Validation validation;

private:
  // Initialize window (this allow for displaying)
  void initWindow();
  void initVulkan();
  void mainLoop();
  void cleanup();
  void drawFrame();

  // Vulkan setup
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createGraphicPipelines();
  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffer();
  void createSyncObjects();
  void recordCommandBuffer(uint32_t imageIndex);

  void upateDynamicViewport();
};