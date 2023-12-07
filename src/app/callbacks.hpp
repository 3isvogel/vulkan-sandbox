#pragma once
#include <lib/log.hpp>
#include <vulkan/vulkan.h>

// is static, it goes into HPP
static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {

  logWarning("VulkanVL: %s", pCallbackData->pMessage);

  return VK_FALSE;
}