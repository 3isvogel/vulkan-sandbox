#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/app.hpp>
#include <app/vulkan/pipelines.hpp>
#include <lib/loader.hpp>
#include <lib/log.hpp>

std::string Pipeline::base_path = "";

VkShaderModule createShaderModule(VkDevice device,
                                  const std::vector<char> &code) {
  VkShaderModuleCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = code.size(),
      .pCode = reinterpret_cast<const uint32_t *>(code.data())};
  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    return nullptr;
  }
  return shaderModule;
}

Pipeline &Pipeline::createVertStage(const std::string &filename) {
  VkShaderModule vertModule =
      createShaderModule(device, readFile(base_path + filename));
  if (vertModule == nullptr) {
    logError("Shader module: %s", filename.data());
    e_runtime("Failed to create fragment module");
  }
  VkPipelineShaderStageCreateInfo vertShaderStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertModule,
      .pName = filename.c_str()};
  shaderStages[0] = vertShaderStageInfo;

  vkDestroyShaderModule(device, vertModule, nullptr);
  logDebug("Pipeline: setup vertex module");
  return *this;
}

Pipeline &Pipeline::createFragStage(const std::string &filename) {

  VkShaderModule fragModule =
      createShaderModule(device, readFile(base_path + filename));
  if (fragModule == nullptr) {
    logError("Shader module: %s", filename.data());
    e_runtime("Failed to create fragment module");
  }
  VkPipelineShaderStageCreateInfo fragShaderStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragModule,
      .pName = filename.c_str()};
  shaderStages[1] = fragShaderStageInfo;

  vkDestroyShaderModule(device, fragModule, nullptr);
  logDebug("Pipeline: setup fragment module");
  return *this;
}

Pipeline::Pipeline(VkDevice device) : device(device) {
  logDebug("Pipeline: createdd new empty pipeline");
}
Pipeline::Pipeline(VkDevice device, const std::string &fragShaderFileName,
                   const std::string &vertShaderFileName)
    : Pipeline(device) {
  this->createVertStage(vertShaderFileName).createFragStage(fragShaderFileName);
}

Pipeline::~Pipeline() {}

void MainApp::createGraphicPipelines() {
  Pipeline::set_path("shaders/");
  Pipeline testTrianglePipeline(device, "triangleFrag.spv", "triangleVert.spv");
}