#pragma once
#include "vulkan.hpp"
#include <GLFW/glfw3.h>

// Create an instance with all required layer and extensions
Status createInstance(VkInstance &instance, String title,
                      const EngineConf &conf);

// check support for required validation layers
bool checkValidationLayerSupport(Names validationLayers);

// enumerate all available extensions
const std::vector<VkExtensionProperties> enumerateIstanceExtensions();