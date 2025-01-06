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

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;

  // Specify fixed: input assembly

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  // HELP: different ways to manage inputs are listed
  // https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions#page_Input-assembly
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  // by using 0xFFFF or 0xFFFFFFFF (-1) you can reset the vertex strip, allowing
  // to use optimization with more complex structures
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // TODO: Now always baked in pipeline the two states, only handle these if
  // required managing dynamic states, handle additional when required
  // NOTE: Both viewport and scissor dyn states result in no performance
  // penalty, so use them without too much concern
  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  // Only draw in a subsection of the window (e.g.: split screen)
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)conf.swap.extent.width;
  viewport.height = (float)conf.swap.extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // After drawing, cut a section of the draw
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = conf.swap.extent;

  // Specify used dynamic states
  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  // Specify fixed: rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  // Clamp values in near far plane (require features)
  rasterizer.depthClampEnable = VK_FALSE;
  // Discard geometry
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  // Draing mode: points, lines, fill << COOL
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  // over 1.0f requires: wideLines feat
  rasterizer.lineWidth = 1.0f;
  // Without specifying normals, determine front and back from vertex order,
  // cull back faces
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  // Other fancy feats
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f;          // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

  // Blend polygone edges for better visuals
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;          // Optional
  multisampling.pSampleMask = nullptr;            // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE;      // Optional

  // Destroy shader handlers after pipeline creation
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