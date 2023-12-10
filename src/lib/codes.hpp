#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
struct errorcode {
  VkResult resultCode;
  const char *meaning;
};

extern struct errorcode ErrorCodes[26];

const char *meanErrorCode(VkResult code);