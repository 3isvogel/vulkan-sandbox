#pragma once
#include "vulkan.hpp"

Status createGraphicsPipeline(String vertShaderFilename,
                              String fragShaderFilename, VkDevice device,
                              const EngineConf &conf);