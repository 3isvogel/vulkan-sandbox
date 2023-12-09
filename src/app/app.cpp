#include "app.hpp"

#include <GLFW/glfw3.h>
#include <app/vulkan/helper.hpp>
#include <app/vulkan/validation/enable.hpp>
#include <lib/log.hpp>
#include <stdexcept>
#include <vulkan/vulkan.h>

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
}

void MainApp::mainLoop() {
  // Prevens app from closing
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void MainApp::cleanup() {
  vkDestroyInstance(instance, nullptr);
  logDebug("Vulkan instance: destroyed");
  glfwDestroyWindow(window);
  logDebug("GLFW Window: destroyed");
  glfwTerminate();
}

void MainApp::createInstance() {
  // checkExtensionSupport();

  enableValidationLayerOnValidation();

  auto extensions = getRequiredExtensions();

  VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                            .pApplicationName = name,
                            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                            .pEngineName = "No Engine",
                            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                            .apiVersion = VK_API_VERSION_1_0};

  VkInstanceCreateInfo createInfo{.sType =
                                      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                  .pApplicationInfo = &appInfo,
                                  .enabledLayerCount = 0,
                                  .pNext = nullptr};

#ifdef __APPLE__
  //   Following
  // https://vulkan.lunarg.com/doc/view/1.3.216.0/mac/getting_started.html#user-content-encountered-vk_error_incompatible_driver
  // It is mandatory to enable this extensions
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

  createInfo.enabledExtensionCount = (uint32_t)extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();

  populateDebugMessengerOnValidation(createInfo);

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    e_runtime("failed to create Vulkan instance");
  }
}

void MainApp::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    e_runtime("No GPUs found with vulkan support");
  }
  logDebug("Found %d GPUs", deviceCount);

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice = device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    e_runtime("No suitable GPU found");
  }
}