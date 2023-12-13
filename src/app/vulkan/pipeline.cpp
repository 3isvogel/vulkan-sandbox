#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/logicaldevice.hpp>
#include <app/vulkan/pipeline.hpp>
#include <lib/loader.hpp>
#include <lib/log.hpp>

std::string Pipeline::base_path = "shaders/";

VkShaderModule Pipeline::createShaderModule(const std::string &filename) {
  auto code = readFile(base_path + filename);
  VkShaderModuleCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = code.size(),
      .pCode = reinterpret_cast<const uint32_t *>(code.data())};
  VkShaderModule shaderModule;
  if (vkCreateShaderModule(renderPass.getsSwapChain().getDevice().get(),
                           &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    destroy();
    logError("Shader module: \"%s\" creation failed", filename.c_str());
    e_runtime("Failed to create shader module");
  }
  return shaderModule;
}

Pipeline &Pipeline::setRenderPass(RenderPass &renderPass) {

  this->renderPass = renderPass;
  this->device = renderPass.getsSwapChain().getDevice();

  // TODO: hardcoded for now, will be fixed later
  vertexInputInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 0,
      .pVertexBindingDescriptions = nullptr,
      .vertexAttributeDescriptionCount = 0,
      .pVertexAttributeDescriptions = nullptr};

  dynamicState = {.sType =
                      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};

  // TODO: hardcoded for now
  rasterizer = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      // Set to true to clamp fragment to viewport
      .depthClampEnable = VK_FALSE,
      // Set to true to disable output to framebuffer
      .rasterizerDiscardEnable = VK_FALSE,
      // Fill polygon, also posible to set it to other values (e.g.:
      // wireframes or points (requires GPU feature))
      .polygonMode = VK_POLYGON_MODE_FILL,
      // for values greater than 1 enable wideLines GPU feature
      .lineWidth = 1.0f,
      // TODO: make settable (backface culling)
      .cullMode = VK_CULL_MODE_BACK_BIT,
      // TODO: make settable (determine face direction by vertices order)
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      // Used for shadowmapping, not now
      .depthBiasEnable = VK_FALSE,
      // Optional
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f};

  multisampling = {.sType =
                       VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                   .sampleShadingEnable = VK_FALSE,
                   .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                   // Optional
                   .minSampleShading = 1.0f,
                   .pSampleMask = nullptr,
                   .alphaToCoverageEnable = VK_FALSE,
                   .alphaToOneEnable = VK_FALSE};

  colorBlendAttachment = {
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
      // TODO: configure later for color blending
      // https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/02_Graphics_pipeline_basics/02_Fixed_functions.html#_color_blending
      .blendEnable = VK_FALSE,
      // Optional
      .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD};

  colorBlending = {.sType =
                       VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                   .logicOpEnable = VK_FALSE,
                   // Optional
                   .logicOp = VK_LOGIC_OP_COPY,
                   .attachmentCount = 1,
                   .pAttachments = &colorBlendAttachment,
                   .blendConstants[0] = 0.0f,
                   .blendConstants[1] = 0.0f,
                   .blendConstants[2] = 0.0f,
                   .blendConstants[3] = 0.0f};

  viewportState = {.sType =
                       VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                   .viewportCount = 1,
                   .scissorCount = 1};

  shaderStages[0] = {};
  shaderStages[1] = {};

  pass;
}

// TODO: move all createion in .build()
Pipeline &Pipeline::setVertStage(const std::string &filename) {
  vertShader = filename;
  shaderStages[0] = {.sType =
                         VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                     .stage = VK_SHADER_STAGE_VERTEX_BIT,
                     .pName = "main"};

  // logDebug("Pipeline: vertex module: \"%s\"", filename.c_str());
  return *this;
}

Pipeline &Pipeline::setFragStage(const std::string &filename) {
  fragShader = filename;
  shaderStages[1] = {.sType =
                         VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                     .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                     .pName = "main"};

  // logDebug("Pipeline: fragment module: \"%s\"", filename.c_str());
  return *this;
}

void Pipeline::destroy() {
  vkDestroyPipeline(device.get(), graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(device.get(), pipelineLayout, nullptr);
  logDebug("Pipeline: destroyed");
}

Pipeline &Pipeline::addDynamicState(VkDynamicState state) {

  dynamicStates.push_back(state);

  return *this;
}

Pipeline &Pipeline::setDynamicStates() {
  addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
  addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

  return *this;
}

Pipeline &Pipeline::setStaticStates(VkViewport &viewport, VkRect2D &scissor) {
  viewportState.pViewports = &viewport;
  viewportState.pScissors = &scissor;

  return *this;
}

Pipeline &Pipeline::setInputAssembly(VkPrimitiveTopology topology,
                                     VkBool32 restartEnable) {
  inputAssembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = topology,
      .primitiveRestartEnable = restartEnable};

  return *this;
}

Pipeline &Pipeline::build() {

  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      // Optional
      .setLayoutCount = 0,
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
      .pNext = nullptr};

  if (vkCreatePipelineLayout(device.get(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    e_runtime("Failed to create pipeline layout");
  }

  if (!vertShader.empty()) {
    VkShaderModule vertModule = createShaderModule(vertShader);
    shaderStages[0].module = vertModule;
  }
  if (!fragShader.empty()) {
    VkShaderModule fragModule = createShaderModule(fragShader);
    shaderStages[1].module = fragModule;
  }

  // TODO: this is horrible, manage it better
  pipelineInfo = {.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                  .stageCount = 2,
                  .pStages = shaderStages,
                  .pVertexInputState = &vertexInputInfo,
                  .pInputAssemblyState = &inputAssembly,
                  .pViewportState = &viewportState,
                  .pRasterizationState = &rasterizer,
                  .pMultisampleState = &multisampling,
                  .pDepthStencilState = nullptr,
                  .pColorBlendState = &colorBlending,
                  // Separate this one to allow for both static and dynamic
                  .pDynamicState = &dynamicState,
                  .layout = pipelineLayout,
                  .renderPass = renderPass.get(),
                  // Possible to use other passes
                  .subpass = 0,
                  // Optional (when createing derivative pipelines)
                  .basePipelineHandle = VK_NULL_HANDLE,
                  .basePipelineIndex = -1};

  if (vkCreateGraphicsPipelines(device.get(), VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &graphicsPipeline) != VK_SUCCESS) {
    e_runtime("Failed to create vulkan pipeline");
  }
  logDebug("Pipeline: created");

  vkDestroyShaderModule(device.get(), shaderStages[0].module, nullptr);
  vkDestroyShaderModule(device.get(), shaderStages[1].module, nullptr);

  return *this;
}