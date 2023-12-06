#include "GLFW/glfw3.h"
#include "lib/log.hpp"
#include "vulkan/vulkan_core.h"
#include <app/app.hpp>
#include <lib/core.h>
#include <stdexcept>

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

void createInstance(const char name[], VkInstance &instance);
void MainApp::initVulkan() { createInstance(name, instance); }

void MainApp::mainLoop() {
  // Prevens app from closing
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void MainApp::cleanup() {
  glfwDestroyWindow(window);
  logDebug("GLFW Window: destroyed");
  glfwTerminate();
}

#include <iostream>

void createInstance(const char name[], VkInstance &instance) {
  VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                            .pApplicationName = name,
                            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                            .pEngineName = "No Engine",
                            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                            .apiVersion = VK_API_VERSION_1_0};

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  for (int i = 0; i < glfwExtensionCount; i++)
    logDebug("Loaded Vulkan extentsion: %s", glfwExtensions[i]);

  VkInstanceCreateInfo createInfo{.sType =
                                      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                  .pApplicationInfo = &appInfo,
                                  .enabledExtensionCount = glfwExtensionCount,
                                  .ppEnabledExtensionNames = glfwExtensions,
                                  .enabledLayerCount = 0};

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error(_EX_ "failed to create Vulkan instance");
  }
}