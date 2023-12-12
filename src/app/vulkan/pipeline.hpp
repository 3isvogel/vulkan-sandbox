#pragma once
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/renderpass.hpp>
#include <app/vulkan/swapchains.hpp>
#include <string>

class Pipeline {
public:
  Pipeline() = default;
  Pipeline(VkDevice device, SwapChain swapChain, RenderPass renderPass);
  Pipeline &setVertStage(const std::string &filename);
  Pipeline &setFragStage(const std::string &filename);
  Pipeline &setDynamicStates();
  Pipeline &setStaticStates(VkViewport &viewport, VkRect2D &scissor);
  Pipeline &setInputAssembly(VkPrimitiveTopology topology,
                             VkBool32 restartEnable = VK_FALSE);
  Pipeline &build();
  VkPipeline &get() { return graphicsPipeline; }
  void destroy();

  static void set_path(const std::string &path) { base_path = path; }

private:
  Pipeline &addDynamicState(VkDynamicState state);
  VkShaderModule createShaderModule(const std::string &filename);

private:
  VkDevice device;
  SwapChain swapChain;

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
  RenderPass renderPass;

  VkPipeline graphicsPipeline;
};