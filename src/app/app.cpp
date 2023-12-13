#include "app/vulkan/commandpool.hpp"
#include "app/vulkan/instance.hpp"
#include "app/vulkan/logicaldevice.hpp"
#include "app/vulkan/pipeline.hpp"
#include "app/vulkan/renderpass.hpp"
#include "app/vulkan/window.hpp"
#include "vulkan/vulkan_core.h"
#include <app/app.hpp>
#include <app/vulkan/commandbuffer.hpp>
#include <cstdint>
#include <lib/log.hpp>

MainApp::MainApp() {}
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
  physicalDevice = PhysicalDevice().bind(surface);
  logicalDevice = LogicalDevice().bind(physicalDevice).build();
  swapChain = SwapChain().connect(logicalDevice, window).build();
  imageView = ImageView().bind(swapChain).build();
  renderPass = RenderPass().bind(swapChain).build();
  pipeline = Pipeline().setRenderPass(renderPass).build();
  framebuffer = Framebuffer().setBase(imageView, renderPass).build();
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
  imageView.destroy();
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