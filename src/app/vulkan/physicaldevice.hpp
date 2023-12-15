#pragma once
// TODO: Should move SwapChainSupportDetails up
#include <app/vulkan/additional_types.hpp>
#include <app/vulkan/surface.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

class PhysicalDevice {
public:
  PhysicalDevice &bind(Surface *surface);

  inline VkPhysicalDevice get() { return physicalDevice; }
  inline Surface *getSurface() { return surface; }
  inline SwapChainSupportDetails getSupportDetails() {
    return querySwapChainSupport(physicalDevice);
  }
  inline std::vector<const char *> extensions() { return deviceExtensions; };

  // TODO: not actually used for now, fix
  inline VkPhysicalDeviceFeatures &features() { return deviceFeatures; }

private:
  void enforceExtensionsRequirements();
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
  bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
  bool checkExtensionSupport(VkPhysicalDevice physicalDevice);
  std::vector<VkExtensionProperties>
  queryExtensionProperties(VkPhysicalDevice physicalDevice);

private:
  std::vector<const char *> deviceExtensions;
  VkPhysicalDeviceFeatures deviceFeatures{};
  VkPhysicalDevice physicalDevice;
  Surface *surface = nullptr;

private:
  static std::vector<const char *> enforcingExtensions;
};
