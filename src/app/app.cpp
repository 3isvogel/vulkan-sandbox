#include "app.hpp"

#include <GLFW/glfw3.h>
#include <app/vulkan_settings.hpp>
#include <lib/log.hpp>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

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
  createInstance(instance, name);
  logDebug("Vulkan instance: created \"%s\"", name);
  setupDebugMessenger(instance, debugMessenger);
}

void MainApp::mainLoop() {
  // Prevens app from closing
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void MainApp::cleanup() {

  destroyDebugMessenger(instance, debugMessenger);

  vkDestroyInstance(instance, nullptr);
  logDebug("Vulkan instance: destroyed");
  glfwDestroyWindow(window);
  logDebug("GLFW Window: destroyed");
  glfwTerminate();
}