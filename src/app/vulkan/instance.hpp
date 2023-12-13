#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/validation/debugger.hpp>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

class Instance {
public:
  Instance();
  void destroy();
  Instance &setName(const char *name);
  Instance &build();
  inline VkInstance const get() { return instance; }

private:
  std::string name;

  std::vector<VkExtensionProperties> queryExtensionProperties();
  std::vector<const char *> getRequiredExtensions();
  bool requireExtensionIfAvailable(const char *ext);

  VkInstance instance;
  Validation validation;
  std::vector<const char *> extensions;
};