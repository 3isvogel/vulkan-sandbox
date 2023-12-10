#pragma once

#include "vulkan/vulkan_core.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/queues.hpp>

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
  struct SwapChain {
    VkSwapchainKHR chain;
    std::vector<VkImage> images;
    VkFormat imageFormat;
    VkExtent2D extent;
    std::vector<VkImageView> imageViews;
  } swapChain;

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
  void createGraphicPipelines();
};