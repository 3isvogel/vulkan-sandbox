#pragma once
#include <lib/loader.hpp>
#include <lib/types.hpp>
#include <vulkan/vulkan.h>

#define QUEUE_HANDLER_LIST x(graphics) x(present)

#define x(n) int32_t n##Family = -1;
// index assigned to every queue family found
struct QueueFamilyIndices {
  // https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Physical_devices_and_queue_families#page_Queue-families
  // hah, fool, just use signed integers and make the program extremely unsafe
  // probably just using https://github.com/akrzemi1/markable

  // queue for graphics commands

  QUEUE_HANDLER_LIST

  // check that all values in the struct are populated
  bool isComplete();
};
#undef x

// required extensions
struct ExtensionConf {
  // required instance extensions
  Names instance;
  // required device extensions
  Names device;
};

// validation layer configuration
struct ValidationConf {
  // enable validation layers
  bool enable;
  // required layers
  Names layers;
};

// surface image format
struct SurfaceConf {
  // bit image format (e.g.: 8R8G8B, 8R8G8B8A)
  VkFormat format;
  // specific colorspace (e.g.: linear, sRGB, etc.)
  VkColorSpaceKHR colorSpace;
};

// swapchain configuration
struct SwapConf {
  // image presentation buffering
  VkPresentModeKHR present = VK_PRESENT_MODE_FIFO_KHR;
  // additional image transform
  VkSurfaceTransformFlagBitsKHR transform =
      VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;
  VkCompositeAlphaFlagBitsKHR compositeAlpha =
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  VkFormat imageFormat;
  VkExtent2D extent;
};

struct Path {
  String shaders;
};

// Main configuration struct, used to set parameters for the app
struct EngineConf {
  uint64_t seed;
  // validation layers
  ValidationConf validation;
  // extensions
  ExtensionConf extensions;
  // surface image format
  SurfaceConf surface;
  // swapchain and presentation modes
  SwapConf swap;
  Path path;
};

// handle pipeline descriptors
union Pipeline {
  VkPipelineShaderStageCreateInfo stages[2];
  struct {
    VkPipelineShaderStageCreateInfo vert;
    VkPipelineShaderStageCreateInfo frag;
  };
};