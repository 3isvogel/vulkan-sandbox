#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

class Pipeline {
public:
  Pipeline(VkDevice device);
  Pipeline(VkDevice device, const std::string &fragShaderFileName,
           const std::string &vertShaderFileName);
  Pipeline &createVertStage(const std::string &filename);
  Pipeline &createFragStage(const std::string &filename);
  ~Pipeline();

  static void set_path(const std::string &path) { base_path = path; }

private:
  VkDevice device;
  VkPipelineShaderStageCreateInfo shaderStages[2];
  static std::string base_path;
};