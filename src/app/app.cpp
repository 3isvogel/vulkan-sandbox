#include "app.hpp"
#include "engine/devices.hpp"
#include "engine/instance.hpp"
#include "engine/pipeline.hpp"
#include "engine/surface.hpp"
#include "engine/swapchain.hpp"
#include "lib/log.hpp"
#include <cstdint>
#include <cstdio>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGT = 600;
const char *TITLE = "Vulkan test";

Status EngineApp::run() {
  YEET(initWindow());
  YEET(initVulkan());
  YEET(mainLoop());
  return OK;
}

Status EngineApp::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGT, TITLE, nullptr, nullptr);
  if (window == nullptr) {
    logDebug("Failed to create window");
    return Status::FAIL;
  }

  return Status::OK;
}

void EngineApp::config() {
  conf.validation.enable =
#ifdef BUILD_RELEASE
      false;
#else
      true;
#endif
  conf.validation.layers.emplace_back("VK_LAYER_KHRONOS_validation");

#ifdef __APPLE__
  // Add extension infos required only by MacOS w/ latest MoltenVK
  // instance
  conf.extensions.instance.emplace_back("VK_KHR_portability_enumeration");
  conf.extensions.instance.emplace_back(
      "VK_KHR_get_physical_device_properties2");

  // Add extension infos required only by MacOS w/ latest MoltenVK
  // device
  conf.extensions.device.emplace_back("VK_KHR_portability_subset");
#endif
  conf.extensions.device.emplace_back("VK_KHR_swapchain");

  // preferred image format
  conf.surface.format = VK_FORMAT_R8G8B8A8_SRGB;
  conf.surface.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

  // preferred present mode
  conf.swap.present = VK_PRESENT_MODE_MAILBOX_KHR;

  conf.path.shaders = String("shaders/");

  setSeed();
}

Status EngineApp::initVulkan() {
  config();
  YEET(createInstance(instance, TITLE, conf));
  YEET(createSurface(surface, instance, window));
  YEET(pickPhysicalDevice(physicalDevice, instance, surface, conf));
  YEET(createLogicalDevice(device, physicalDevice, surface, conf));
  YEET(createSwapChain(swapChain, surface, window, physicalDevice, device,
                       conf));
  getSwapchainImages(swapChainImages, imageCount, swapChain, device);
  YEET(createImageViews(swapChainIMageViews, swapChainImages, device, conf));
  YEET(createGraphicsPipeline("testVert", "testFrag", device, conf));
  bindQueueHandler();

  return OK;
}

Status EngineApp::mainLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
  return OK;
}

void EngineApp::cleanup() {
  logDebug("Cleaning...");
  for (auto imageView : swapChainIMageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }
  vkDestroySwapchainKHR(device, swapChain, nullptr);
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
  glfwDestroyWindow(window);

  glfwTerminate();
}

void EngineApp::bindQueueHandler() {
  indices = findQueueFamilies(physicalDevice, surface);

  // initialize queue handlers
#define x(f)                                                                   \
  vkGetDeviceQueue(device, CON(indices.f, Family), 0, &CON(f, Queue));         \
  logDebug("Queue family binding %p -> %i (%s)", CON(f, Queue),                \
           CON(indices.f, Family), STR(f));
  QUEUE_HANDLER_LIST
#undef x
}

#include <stdio.h>
Status EngineApp::setSeed() {
  this->seed = 0;
  FILE *fp = fopen("/dev/random", "rb");
  if (fp == NULL)
    logWarning("failed to access /dev/random, ignoring seed creation");
  else if (fread(&(this->seed), sizeof(uint64_t), 1, fp) != 1)
    logWarning("failed to access /dev/random, ignoring seed creation");
  fclose(fp);
  return Status::OK;
}