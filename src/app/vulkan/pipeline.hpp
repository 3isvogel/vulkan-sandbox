#pragma once
#include <app/vulkan/renderpass.hpp>
#include <string>
#include <vulkan/vulkan_core.h>

class Pipeline {
public:
  void destroy();
  Pipeline &setRenderPass(RenderPass &renderPass);
  Pipeline &setVertStage(const std::string &filename);
  Pipeline &setFragStage(const std::string &filename);
  Pipeline &setDynamicStates();
  Pipeline &setStaticStates(VkViewport &viewport, VkRect2D &scissor);
  Pipeline &setInputAssembly(VkPrimitiveTopology topology,
                             VkBool32 restartEnable = VK_FALSE);
  Pipeline &build();
  VkPipeline get() { return graphicsPipeline; }
  inline RenderPass *getRenderPass() { return renderPass; }
  inline bool getDynamic() { return isDynamic; }

  static void setPath(const std::string &path) { basePath = path; }

private:
  void check();
  Pipeline &addDynamicState(VkDynamicState state);
  VkShaderModule createShaderModule(LogicalDevice *device,
                                    const std::string &filename);

private:
  bool isDynamic = false;

  RenderPass *renderPass = nullptr;

  std::string fragShader = "";
  std::string vertShader = "";

  VkPipelineShaderStageCreateInfo shaderStages[2] = {};

  static std::string basePath;
  VkPipelineVertexInputStateCreateInfo vertexInputInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssembly;

  VkPipelineViewportStateCreateInfo viewportState;
  VkPipelineRasterizationStateCreateInfo rasterizer;
  VkPipelineMultisampleStateCreateInfo multisampling;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlending;

  std::vector<VkDynamicState> dynamicStates;
  VkPipelineDynamicStateCreateInfo dynamicState;

  VkPipelineLayout pipelineLayout;
  VkGraphicsPipelineCreateInfo pipelineInfo;

  VkPipeline graphicsPipeline;
};