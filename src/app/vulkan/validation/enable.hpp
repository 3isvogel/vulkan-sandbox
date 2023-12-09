#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

extern const bool enableValidationLayers;
extern const std::vector<const char *> validationLayers;

extern void
requireValidationLayerOnValidation(std::vector<const char *> &extensions);
extern void enableValidationLayerOnValidation();
extern void
populateDebugMessengerOnValidation(VkInstanceCreateInfo &createInfo);
void setupDebugMessengerOnValidation(VkInstance instance);
void destroyDebugMessengerOnValidation(VkInstance instance);