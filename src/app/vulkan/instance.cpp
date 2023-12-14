#include <app/vulkan/instance.hpp>
#include <app/vulkan/validation/callbacks.hpp>

Instance::Instance() {}

Instance &Instance::setName(const char *name) {
  this->name = name;
  pass;
}

Instance &Instance::build() {
  getRequiredExtensions();
  VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                            .pApplicationName = name.c_str(),
                            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                            .pEngineName = "No Engine",
                            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                            .apiVersion = VK_API_VERSION_1_0};

  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
  };

#ifdef __APPLE__
  //   Following
  // https://vulkan.lunarg.com/doc/view/1.3.216.0/mac/getting_started.html#user-content-encountered-vk_error_incompatible_driver
  // It is mandatory to enable this extensions
  requireExtensionIfAvailable(
      VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
  if (requireExtensionIfAvailable(
          VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(enabledExtensions.size());
  createInfo.ppEnabledExtensionNames = enabledExtensions.data();

  logDebug("Required extensions [%i]", enabledExtensions.size());
  for (const auto &extension : enabledExtensions) {
    logDebug("| %s", extension);
  }

  validation.callback(debugCallback).populateDebugMessenger(createInfo);

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    e_runtime("failed to create Vulkan instance");
  }

  validation.bind(instance);

  logDebug("Vulkan instance: created \"%s\"", name.c_str());
  pass;
}

std::vector<VkExtensionProperties> Instance::queryExtensionProperties() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());
  return extensions;
}

void Instance::getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);
  for (auto const &ext : extensions)
    enabledExtensions.push_back(ext);

  Validation::require(enabledExtensions);
}

bool Instance::requireExtensionIfAvailable(const char *ext) {

  auto availableExt = queryExtensionProperties();

  for (const auto &extProp : availableExt) {
    if (strcmp(ext, extProp.extensionName) == 0) {
      enabledExtensions.push_back(ext);
      return true;
    }
  }
  return false;
}

void Instance::destroy() {
  validation.destroy();
  vkDestroyInstance(instance, nullptr);
  logDebug("Vulkan instance: destroyed");
}