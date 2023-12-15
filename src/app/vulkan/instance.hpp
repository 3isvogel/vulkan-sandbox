#pragma once
#include <app/vulkan/validation/debugger.hpp>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

class Instance {
public:
  void destroy();
  Instance &setName(const char *name);
  Instance &build();
  inline VkInstance const get() { return instance; }

private:
  void check();

private:
  std::string name;

  std::vector<VkExtensionProperties> queryExtensionProperties();
  void getRequiredExtensions();
  bool requireExtensionIfAvailable(const char *ext);

  VkInstance instance;
  Validation validation;
  std::vector<const char *> enabledExtensions;
};