#pragma once
#include <lib/log.hpp>
#include <vulkan/vulkan.h>

#define CALLBACKPRINT(severity, callBackName)                                  \
  logPrint(severity, #callBackName, 0,                                         \
           SET2E(FG(YELLOW_CODE), BG(BLACK_CODE)) "Validation:" RESET " %s",   \
           pCallbackData->pMessage)
#define DEBUGCALLBACK(severity) CALLBACKPRINT(severity, debugCallback)

// is static, it goes into HPP
static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {

  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    DEBUGCALLBACK(LOG_LEVEL_ERROR);
  else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    DEBUGCALLBACK(LOG_LEVEL_WARNING);
  else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    DEBUGCALLBACK(LOG_LEVEL_INFO);
  else
    DEBUGCALLBACK(LOG_LEVEL_DEBUG);

  return VK_FALSE;
}

#undef CALLBACKPRINT