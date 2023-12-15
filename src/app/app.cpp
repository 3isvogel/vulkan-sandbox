#include "app/vulkan/pipeline.hpp"
#include "vulkan/vulkan_core.h"
#include <app/app.hpp>
#include <app/vulkan/commandbuffer.hpp>
#include <cstdint>
#include <lib/log.hpp>

MainApp::MainApp(int width, int height, std::string name)
    : width(width), height(height), name(name) {}

void MainApp::run() {
  window = Window().create(width, height, name.c_str());
  initVulkan();
  mainLoop();
  cleanup();
}

void MainApp::initVulkan() {
  instance = Instance().setName(name.c_str()).build();
  surface = Surface().setBase(instance, window).build();
  // TODO: bind logical to surface directly
  physicalDevice = PhysicalDevice().bind(surface);
  logicalDevice = LogicalDevice().bind(physicalDevice).build();
  swapChain = SwapChain().connect(logicalDevice).build();
  renderPass = RenderPass().bind(swapChain).build();
  Pipeline::setPath("shaders/");
  pipeline = Pipeline()
                 .setRenderPass(renderPass)
                 .setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                 .setDynamicStates()
                 .setVertStage("triangleVert.spv")
                 .setFragStage("triangleFrag.spv")
                 .build();
  framebuffer = Framebuffer().bind(renderPass).build();
  commandPool = CommandPool().bind(logicalDevice).build();
  commandBuffer = CommandBuffer()
                      .bind(pipeline)
                      .bind(commandPool)
                      .attach(framebuffer)
                      .build();
  sync = Sync().connect(swapChain).build();
}

void MainApp::mainLoop() {
  // Prevens app from closing
  logInfo("Starting \"%s\"", name.c_str());
  while (window.running()) {
    glfwPollEvents();
    drawFrame();
  }
  logDebug("Waiting for device to complete");
  logicalDevice.waitIdle();

  logInfo("Stopping \"%s\"", name.c_str());
}

void MainApp::cleanup() {
  sync.destroy();
  commandPool.destroy();
  framebuffer.destroy();
  pipeline.destroy();
  renderPass.destroy();
  swapChain.destroy();
  logicalDevice.destroy();
  surface.destroy();
  instance.destroy();
  window.destroy();
}

void MainApp::drawFrame() {
  uint32_t imageIndex;
  sync.acquireNextImage(imageIndex);

  commandBuffer.reset();
  commandBuffer.record(imageIndex);

  sync.submitCommand(commandBuffer.get());
  sync.present(imageIndex);
}