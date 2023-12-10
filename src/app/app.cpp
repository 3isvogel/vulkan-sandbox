#include <app/app.hpp>
#include <lib/log.hpp>

MainApp::MainApp() {}
MainApp::MainApp(int width, int height, const char name[])
    : width(width), height(height), name(name) {}

void MainApp::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window = glfwCreateWindow(width, height, name, nullptr, nullptr);
  if (window == nullptr) {
    e_runtime("Failed to create GLFW window");
  }
  logDebug("GLFW window: created \"%s\" (%dx%d)", name, width, height);
}

void MainApp::initVulkan() {
  createInstance();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createImageViews();
  createGraphicPipelines();
}

void MainApp::mainLoop() {
  // Prevens app from closing
  logInfo("Starting \"%s\"", name);
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
  logInfo("Stopping \"%s\"", name);
}

void MainApp::cleanup() {
  for (auto imageView : swapChain.imageViews)
    vkDestroyImageView(device, imageView, nullptr);
  logDebug("Image views: destroyed");
  vkDestroySwapchainKHR(device, swapChain.chain, nullptr);
  logDebug("SwapChain: destroyed");
  vkDestroyDevice(device, nullptr);
  logDebug("Logical device: destroyed");
  vkDestroySurfaceKHR(instance, surface, nullptr);
  logDebug("GLFW surface: destroyed");
  vkDestroyInstance(instance, nullptr);
  logDebug("Vulkan instance: destroyed");
  glfwDestroyWindow(window);
  logDebug("GLFW Window: destroyed");
  glfwTerminate();
}