#include "instance.hpp"
#include <lib/log.hpp>
#include <vector>

// required extensions, provided by glfw
uint32_t glfwRequiredExtensionCount = 0;
const char **glfwRequiredExtensions;

// Create an instance with all required layer and extensions
Status createInstance(VkInstance &instance, String title,
                      const EngineConf &conf) {

  // check if validation layers are required and if all required layers are
  // available
  if (conf.validation.enable &&
      !checkValidationLayerSupport(conf.validation.layers)) {
    return Status::INSTANCE_CREATION_FAIL;
  }

  // sType is common to all structs, tag for the type of the structure, don't
  // mess it up, whatever is not initialized is nullptr/0

  // Value info about the app: only for report
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = title.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "YUE";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  // Informations needed for instance creation: mess these up and you are gonna
  // segfault
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Get get list of the required extensions for instance, these are provided by
  // GLFW and cannot be chosen, as they are required for the working of
  // application, so they are handled differently than other extensions
  glfwRequiredExtensions =
      glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

  // all required extensions, required by GLFW + specific
  Names requiredExtensions;

  // Populate required extension vector (dynamically, required by GLFW and
  // specific)
  for (uint32_t i = 0; i < glfwRequiredExtensionCount; i++) {
    requiredExtensions.emplace_back(glfwRequiredExtensions[i]);
  }

  for (const auto &extension : conf.extensions.instance) {
    requiredExtensions.emplace_back(extension);
  }

#ifdef __APPLE__
  // Add extension infos required only by MacOS w/ latest MoltenVK
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  logDebug("Instance extension required:");
  for (const auto &extension : requiredExtensions) {
    logDebug("  | %s", extension);
  }

  // Populate structure field
  createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  // Will later be used for validation layer
  if (conf.validation.enable) {
    for (const char *const &layer : conf.validation.layers) {
      logDebug("Validation layer required: %s", layer);
    }
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(conf.validation.layers.size());
    createInfo.ppEnabledLayerNames = conf.validation.layers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

  if (result != VK_SUCCESS) {
    logDebug("Failed to create instance: %i", result);
    return Status::INSTANCE_CREATION_FAIL;
  }

  logDebug("Instance created");
  return OK;
}

bool checkValidationLayerSupport(Names validationLayers) {

  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers) {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      logError("Required \"%s\" is not available", layerName);
      return false;
    }
  }

  return true;
}

const std::vector<VkExtensionProperties> enumerateIstanceExtensions() {

  uint32_t availExtensionCount = 0;
  std::vector<VkExtensionProperties> availExtensions;

  vkEnumerateInstanceExtensionProperties(nullptr, &availExtensionCount,
                                         nullptr);
  availExtensions = std::vector<VkExtensionProperties>(availExtensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &availExtensionCount,
                                         availExtensions.data());
  logDebug("Instance available extensions:");
  for (const auto &extension : availExtensions) {
    logDebug("  | %s", extension);
  }
  return availExtensions;
}