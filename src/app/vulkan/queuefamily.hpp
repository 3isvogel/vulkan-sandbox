#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/physicaldevice.hpp>
#include <app/vulkan/surface.hpp>
#include <optional>
#include <set>

// MF managed to put X macro inside his code
#define FAMILIES_LIST                                                          \
  X(graphics)                                                                  \
  X(present)

class QueueFamily {
public:
  QueueFamily &bind(Surface &surface);
  QueueFamily &find(VkPhysicalDevice physicalDevice);
  bool isComplete();
  std::set<uint32_t> getIndeces();

private:
  std::vector<VkQueueFamilyProperties> query(VkPhysicalDevice device);
  std::vector<VkQueueFamilyProperties> queueFamilies;

  Surface surface;

public:
#define X(name)                                                                \
  std::optional<uint32_t> name##Family;                                        \
  VkQueue name##Queue;
  FAMILIES_LIST
#undef X
};