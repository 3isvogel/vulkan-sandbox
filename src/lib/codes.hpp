#pragma once

#include "vulkan/vulkan_core.h"
#include <string>
struct errorcode {
  VkResult resultCode;
  std::string meaning;
};

extern struct errorcode ErrorCodes[26];