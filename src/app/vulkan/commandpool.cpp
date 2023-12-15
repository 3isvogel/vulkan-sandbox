#include "app/vulkan/logicaldevice.hpp"
#include "app/vulkan/queuefamily.hpp"
#include "lib/macros.hpp"
#include <app/vulkan/commandpool.hpp>

CommandPool &CommandPool::bind(LogicalDevice &device) {
  this->device = &device;
  pass;
}

CommandPool &CommandPool::build() {
  check();
  QueueFamily queueFamilyIndices =
      QueueFamily()
          .bind(device->getPhysicalDevice()->getSurface())
          .find(device->getPhysicalDevice()->get());

  VkCommandPoolCreateInfo poolInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()};

  if (vkCreateCommandPool(device->get(), &poolInfo, nullptr, &commandPool) !=
      VK_SUCCESS) {
    e_runtime("Failed to create command pool");
  }
  logDebug("Command pool: created");
  pass;
}

void CommandPool::destroy() {
  vkDestroyCommandPool(device->get(), commandPool, nullptr);
  logDebug("Command pool: destroyed");
}

void CommandPool::check() {
  if (!device)
    runtime_dep(CommandPool, LogicalDevice);
}