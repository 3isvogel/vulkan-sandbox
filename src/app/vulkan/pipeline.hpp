#pragma once
#include "app/vulkan/logicaldevice.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/renderpass.hpp>
#include <app/vulkan/swapchain.hpp>
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
  inline RenderPass &getRenderPass() { return renderPass; }

  static void set_path(const std::string &path) { base_path = path; }

private:
  Pipeline &addDynamicState(VkDynamicState state);
  VkShaderModule createShaderModule(const std::string &filename);

private:
  RenderPass renderPass;
  LogicalDevice device;

  std::string fragShader = "";
  std::string vertShader = "";

  VkPipelineShaderStageCreateInfo shaderStages[2];

  static std::string base_path;
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