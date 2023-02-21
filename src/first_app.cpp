#include "first_app.hpp"

#include <stdexcept>

namespace lve {

void FirstApp::run() {
  while ( !window.shouldClose() ) {
    glfwPollEvents();
  }
}

FirstApp::FirstApp() {
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}

FirstApp::~FirstApp() {
  vkDestroyPipelineLayout( device.device(), pipelineLayout, nullptr );
}

void FirstApp::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;

  // set layouts are used to push information other than vertices to shaders
  pipelineLayoutInfo.pSetLayouts = nullptr;

  // push constants are a way to send limited data to shader programs
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if ( vkCreatePipelineLayout(
           device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout ) !=
       VK_SUCCESS ) {
    throw std::runtime_error( "Failed to create pipeline layout" );
  }
}

void FirstApp::createPipeline() {
  auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(
      swapChain.width(), swapChain.height() );
  pipelineConfig.renderPass = swapChain.getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;

  pipeline = std::make_unique< Pipeline >(
      device, "assets/shaders/simple_shader.vert.spv",
      "assets/shaders/simple_shader.frag.spv", pipelineConfig );
}

void FirstApp::createCommandBuffers() {}

void FirstApp::drawFrame() {}

}  // namespace lve
