#pragma once
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <app/vulkan/renderpass.hpp>
#include <app/vulkan/swapchains.hpp>
#include <string>

class Pipeline {
public:
  Pipeline(VkDevice device, SwapChain swapChain, RenderPass renderPass);
  Pipeline &setVertStage(const std::string &filename);
  Pipeline &setFragStage(const std::string &filename);
  Pipeline &setDynamicStates();
  Pipeline &setStaticStates();
  Pipeline &setInputAssembly(VkPrimitiveTopology topology,
                             VkBool32 restartEnable = VK_FALSE);
  Pipeline &build();
  // Pipeline &setViewPort(); // not yet implementingd
  void destroy();

  static void set_path(const std::string &path) { base_path = path; }

private:
  Pipeline &addDynamicState(VkDynamicState state);

private:
  static std::string base_path;

  VkDevice device;
  SwapChain swapChain;

  VkShaderModule fragModule = VK_NULL_HANDLE;
  VkShaderModule vertModule = VK_NULL_HANDLE;

  VkPipelineShaderStageCreateInfo shaderStages[2];

  std::vector<VkDynamicState> dynamicStates;
  VkPipelineDynamicStateCreateInfo dynamicState;

  struct Config {
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;

    VkViewport viewport;
    VkRect2D scissor;

    VkPipelineViewportStateCreateInfo viewportState;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlending;
  } config{};

  VkPipelineLayout pipelineLayout;
  VkGraphicsPipelineCreateInfo pipelineInfo;
  RenderPass renderPass;

  VkPipeline graphicsPipeline;
};