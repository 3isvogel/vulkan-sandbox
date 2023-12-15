#pragma once
#include "app/vulkan/additional_types.hpp"
#include <app/vulkan/physicaldevice.hpp>
#include <app/vulkan/queuefamily.hpp>
#include <app/vulkan/surface.hpp>
#include <vulkan/vulkan_core.h>

class LogicalDevice {

public:
  void destroy();
  LogicalDevice &bind(Surface &surface);
  LogicalDevice &build();
  inline const VkDevice get() { return device; }
  inline Surface *getSurface() { return surface; }
  inline QueueFamily *getQueue() { return &queues; }
  inline VkPhysicalDevice getPhysicalDevice() { return physicalDevice.get(); }
  inline SwapChainSupportDetails getSupportDetails() {
    return physicalDevice.getSupportDetails();
  }

  inline void waitIdle() { vkDeviceWaitIdle(device); }

private:
  void check();

private:
  VkDevice device;
  PhysicalDevice physicalDevice;
  Surface *surface = nullptr;
  QueueFamily queues;
};