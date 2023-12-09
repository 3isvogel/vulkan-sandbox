#pragma once
#include "vulkan/vulkan_core.h"
#include <lib/log.hpp>
#include <vulkan/vulkan.h>

// is static, it goes into HPP
static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {

  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    logError("VulkanVL: %s", pCallbackData->pMessage);
  else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    logWarning("VulkanVL: %s", pCallbackData->pMessage);
  else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    logInfo("VulkanVL: %s", pCallbackData->pMessage);
  else
    logDebug("VulkanVL: %s", pCallbackData->pMessage);

  return VK_FALSE;
}