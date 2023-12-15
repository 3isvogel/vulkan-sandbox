#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/physicaldevice.hpp>
#include <app/vulkan/queuefamily.hpp>
#include <app/vulkan/surface.hpp>

class LogicalDevice {

public:
  void destroy();
  LogicalDevice &bind(PhysicalDevice &physicalDevice);
  LogicalDevice &build();
  inline const VkDevice get() { return device; }
  inline PhysicalDevice *getPhysicalDevice() { return physicalDevice; }
  inline QueueFamily getQueue() { return queues; }

  inline void waitIdle() { vkDeviceWaitIdle(device); }

private:
  void check();

private:
  VkDevice device;
  PhysicalDevice *physicalDevice = nullptr;
  QueueFamily queues;
};