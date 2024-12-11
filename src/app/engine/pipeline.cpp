#include "pipeline.hpp"
#include "lib/types.h"
#include "lib/types.hpp"
#include <lib/loader.hpp>

Status createShaderModule(VkShaderModule &shaderModule, const Bytes &code,
                          VkDevice device);

Status createGraphicsPipeline(String vertShaderFilename,
                              String fragShaderFilename, VkDevice device,
                              const EngineConf &conf) {
  // TODO: make global
  String shaders_path(conf.path.shaders);

  // load code for pipeline creation
  Bytes vertShaderCode;
  Bytes fragShaderCode;

  YEET(readFile(vertShaderCode, shaders_path + vertShaderFilename + ".spv"));
  YEET(readFile(fragShaderCode, shaders_path + fragShaderFilename + ".spv"));

  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;

  YEET(createShaderModule(vertShaderModule, vertShaderCode, device));
  YEET(createShaderModule(fragShaderModule, fragShaderCode, device));

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  // you can use different entrypoint to the same shader module
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  // HELP: as stated in
  // https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Shader_modules#page_Shader-stage-creation,
  // at this stage is possible to specify constant values, making optimization
  // at time of module createion, resulting in better performances

  // Destroy handlers
  vkDestroyShaderModule(device, vertShaderModule, nullptr);
  vkDestroyShaderModule(device, fragShaderModule, nullptr);

  return Status::OK;
}

Status createShaderModule(VkShaderModule &shaderModule, const Bytes &code,
                          VkDevice device) {
  VkShaderModuleCreateInfo createInfo{};

  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkResult result =
      vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
  if (result != VK_SUCCESS) {
    logDebug("Failed to create shader module: %i", result);
    return Status::SHADER_MODULE_FAIL;
  }

  return Status::OK;
};