#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
  // VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;

private:
  // Initialize window (this allow for displaying)
  void initWindow();
  void initVulkan();
  void mainLoop();
  void cleanup();

  // Vulkan setup
  void createInstance();
  void pickPhysicalDevice();
  void createLogicalDevice();
};