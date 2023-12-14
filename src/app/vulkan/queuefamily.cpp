#include <app/vulkan/physicaldevice.hpp>
#include <app/vulkan/queuefamily.hpp>
#include <app/vulkan/surface.hpp>

QueueFamily &QueueFamily::bind(Surface *surface) {
  this->surface = surface;
  pass;
}

QueueFamily &QueueFamily::find(VkPhysicalDevice physicalDevice) {

  auto queueFamilies = query(physicalDevice);

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface->get(),
                                         &presentSupport);

    if (presentSupport) {
      presentFamily = i;
    }

    if (isComplete()) {
      break;
    }
    i++;
  }
  pass;
}

bool QueueFamily::isComplete() {
#define X(name) &&name##Family.has_value()
  return true FAMILIES_LIST;
#undef X
}

std::vector<VkQueueFamilyProperties>
QueueFamily::query(VkPhysicalDevice device) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());
  return queueFamilies;
}

std::set<uint32_t> QueueFamily::getIndeces() {
#define X(name) name##Family.value(),
  std::set<uint32_t> uniqueQueueFamilies = {FAMILIES_LIST};
#undef X
  return uniqueQueueFamilies;
}