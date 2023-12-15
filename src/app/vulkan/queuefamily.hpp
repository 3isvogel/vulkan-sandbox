#pragma once
#include <app/vulkan/surface.hpp>
#include <optional>
#include <set>
#include <vulkan/vulkan_core.h>

// MF managed to put X macro inside his code
#define FAMILIES_LIST                                                          \
  X(graphics)                                                                  \
  X(present)

class QueueFamily {
public:
  QueueFamily &bind(Surface *surface);
  QueueFamily &find(VkPhysicalDevice physicalDevice);
  bool isComplete();
  std::set<uint32_t> getIndeces();

private:
  void check();

private:
  std::vector<VkQueueFamilyProperties> query(VkPhysicalDevice device);
  std::vector<VkQueueFamilyProperties> queueFamilies;

  Surface *surface = nullptr;

public:
#define X(name)                                                                \
  std::optional<uint32_t> name##Family;                                        \
  VkQueue name##Queue;
  FAMILIES_LIST
#undef X
};