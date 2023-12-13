#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/additional_types.hpp>
#include <app/vulkan/instance.hpp>
#include <app/vulkan/surface.hpp>
#include <vector>

class PhysicalDevice {
public:
  PhysicalDevice &bind(Surface &surface);

  inline VkPhysicalDevice get() { return physicalDevice; }
  inline Surface &getSurface() { return surface; }
  inline std::vector<const char *> extensions() { return deviceExtensions; };

  // TODO: not actually used for now, fix
  inline VkPhysicalDeviceFeatures &features() { return deviceFeatures; }

private:
  void enforceExtensionsRequirements();
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

private:
  bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
  bool checkExtensionSupport(VkPhysicalDevice physicalDevice);
  std::vector<VkExtensionProperties>
  queryExtensionProperties(VkPhysicalDevice physicalDevice);

  std::vector<const char *> deviceExtensions;
  VkPhysicalDeviceFeatures deviceFeatures{};
  VkPhysicalDevice physicalDevice;
  Surface surface;

private:
  static std::vector<const char *> enforcingExtensions;
};
