#include "app/vulkan/physicaldevice.hpp"
#include <app/vulkan/instance.hpp>
#include <app/vulkan/logicaldevice.hpp>
#include <app/vulkan/queuefamily.hpp>

LogicalDevice &LogicalDevice::bind(PhysicalDevice &physicalDevice) {

  this->physicalDevice = physicalDevice;
  this->queues.bind(physicalDevice.getSurface()).find(physicalDevice.get());
  pass;
}

LogicalDevice &LogicalDevice::build() {

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = queues.getIndeces();

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority};
    queueCreateInfos.push_back(queueCreateInfo);
  }

  auto deviceExtensions = physicalDevice.extensions();

  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pQueueCreateInfos = queueCreateInfos.data(),
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pEnabledFeatures = &physicalDevice.features(),
      .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
      .ppEnabledExtensionNames = deviceExtensions.data()};

  // physicalDevice.getSurface().getInstance().legacyPopulateDeviceSpecific(
  // createInfo);

  if (vkCreateDevice(physicalDevice.get(), &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    e_runtime("Failed to create logical device");
  }
  logDebug("Logical device: created");

#define X(name)                                                                \
  vkGetDeviceQueue(device, queues.name##Family.value(), 0,                     \
                   &queues.name##Queue);                                       \
  logDebug("Device queue (" #name "): attached");
  FAMILIES_LIST
#undef X

  logDebug("Queue handler: retrieved");
  pass;
}

void LogicalDevice::destroy() {
  vkDestroyDevice(device, nullptr);
  logDebug("Logical device: destroyed");
}