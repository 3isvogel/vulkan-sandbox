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
  logDebug("GLFW window: created \"%s\" (%dx%d)", name, width, height);
}

void MainApp::initVulkan() {
  createInstance();
  logDebug("Vulkan instance: created \"%s\"", name);
  pickPhysicalDevice();
  createLogicalDevice();
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
  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr);
  logDebug("Vulkan instance: destroyed");
  glfwDestroyWindow(window);
  logDebug("GLFW Window: destroyed");
  glfwTerminate();
}