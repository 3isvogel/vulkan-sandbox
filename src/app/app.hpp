#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/pipeline.hpp>
#include <app/vulkan/queues.hpp>
#include <app/vulkan/renderpass.hpp>
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
  // VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
#define X(name) VkQueue name##Queue;
  FAMILIES_LIST
#undef X
  SwapChain swapChain;

  RenderPass renderPass;
  std::vector<Pipeline> pipelines;

private:
  // Initialize window (this allow for displaying)
  void initWindow();
  void initVulkan();
  void mainLoop();
  void cleanup();

  // Vulkan setup
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createGraphicPipelines();
};