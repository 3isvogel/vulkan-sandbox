#include <app/vulkan/instance.hpp>
#include <app/vulkan/physicaldevice.hpp>
#include <app/vulkan/queuefamily.hpp>
#include <app/vulkan/surface.hpp>
#include <lib/log.hpp>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

// TODO: move this
#define DEVICE_EXTENSIONS_LIST X(VK_KHR_SWAPCHAIN_EXTENSION_NAME)

std::vector<const char *> PhysicalDevice::enforcingExtensions = {
    "VK_KHR_portability_subset"};

PhysicalDevice &PhysicalDevice::bind(Surface *surface) {
  this->surface = surface;
  auto instance = surface->getInstance();

#define X(name) deviceExtensions.push_back(name);
  DEVICE_EXTENSIONS_LIST
#undef X

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance->get(), &deviceCount, nullptr);
  if (deviceCount == 0)
    e_runtime("No GPUs found with Vulkan support");
  logDebug("GPUs Found: %d", deviceCount);
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance->get(), &deviceCount, devices.data());

  int i = 0;
  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice = device;
      logDebug("Physical device: selected [%i]", i++);
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    e_runtime("No suitable GPU found");
  }

  enforceExtensionsRequirements();

  pass;
}

std::vector<VkExtensionProperties>
PhysicalDevice::queryExtensionProperties(VkPhysicalDevice physicalDevice) {
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       extensions.data());
  return extensions;
}

// TODO: Better device selection
// Following
// https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/03_Physical_devices_and_queue_families.html#_base_device_suitability_checks
// Can implement device sorting by suitability or manual selection
bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice physicalDevice) {

  QueueFamily queueFamily = QueueFamily().bind(surface).find(physicalDevice);

  bool extensionSupported = checkExtensionSupport(physicalDevice);

  bool swapChainAdequate = false;
  if (extensionSupported) {
    SwapChainSupportDetails SwapChainSupport =
        querySwapChainSupport(physicalDevice);
    swapChainAdequate = !SwapChainSupport.formats.empty() &&
                        !SwapChainSupport.presentModes.empty();
  }

  return queueFamily.isComplete() && extensionSupported && swapChainAdequate;
}

bool PhysicalDevice::checkExtensionSupport(VkPhysicalDevice device) {
  auto availableExtensions = queryExtensionProperties(device);

  std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                           deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

// FIXME: this is associated with logical device
void PhysicalDevice::enforceExtensionsRequirements() {

  auto extensions = queryExtensionProperties(physicalDevice);

  for (const auto &extension : extensions)
    for (const auto &enforcingExtension : enforcingExtensions)
      if (strcmp(enforcingExtension, extension.extensionName) == 0)
        deviceExtensions.push_back(enforcingExtension);
}

SwapChainSupportDetails
PhysicalDevice::querySwapChainSupport(VkPhysicalDevice device) {

  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface->get(),
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->get(), &formatCount,
                                       nullptr);
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->get(), &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface->get(),
                                            &presentModeCount, nullptr);
  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface->get(), &presentModeCount, details.presentModes.data());
  }

  return details;
}