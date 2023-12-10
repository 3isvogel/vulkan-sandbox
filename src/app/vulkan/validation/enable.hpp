#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

void requireValidationLayerOnValidation(std::vector<const char *> &extensions);
void enableValidationLayerOnValidation();
void populateDebugMessengerOnValidation(VkInstanceCreateInfo &createInfo);

void legacyPopulateDeviceSpecificValidationOnValidation(
    VkDeviceCreateInfo &createInfo);
void setupDebugMessengerOnValidation(VkInstance instance);
void destroyDebugMessengerOnValidation(VkInstance instance);