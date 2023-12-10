#pragma once
#include <vulkan/vulkan.hpp>

extern const bool enableValidationLayers;
extern const std::vector<const char *> validationLayers;

extern void
requireValidationLayerOnValidation(std::vector<const char *> &extensions);
extern void enableValidationLayerOnValidation();
extern void
populateDebugMessengerOnValidation(VkInstanceCreateInfo &createInfo);

extern void legacyPopulateDeviceSpecificValidationOnValidation(
    VkDeviceCreateInfo &createInfo);
extern void setupDebugMessengerOnValidation(VkInstance instance);
extern void destroyDebugMessengerOnValidation(VkInstance instance);