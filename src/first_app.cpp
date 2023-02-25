#include "first_app.hpp"

#include <array>
#include <stdexcept>

namespace lve {

void FirstApp::run() {
  while ( !window.shouldClose() ) {
    glfwPollEvents();
    drawFrame();

    // this line avoids having a crash on exit from the window should we close
    // it during a renderPass
    vkDeviceWaitIdle( device.device() );
  }
}

FirstApp::FirstApp() {
  loadModels();
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
       VK_SUCCESS )
    throw std::runtime_error( "Failed to create pipeline layout" );
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

void FirstApp::createCommandBuffers() {

  // resize the commandBuffers vector to have as many as we have framebuffers
  // (most probably 2 or 3)
  commandBuffers.resize( swapChain.imageCount() );

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  // command buffers can be primary or secondary; primary buffers can be
  // submitted to a queue for execution but cannot be called by secondary
  // command buffers, and vice versa
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  // command pools are opaque objects from which memory is allocated to
  // command buffers
  allocInfo.commandPool = device.getCommandPool();

  allocInfo.commandBufferCount =
      static_cast< uint32_t >( commandBuffers.size() );

  if ( vkAllocateCommandBuffers(
           device.device(), &allocInfo, commandBuffers.data() ) != VK_SUCCESS )
    throw std::runtime_error( "failed to initialize commandbuffers" );

  for ( int i = 0; i < commandBuffers.size(); ++i ) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if ( vkBeginCommandBuffer( commandBuffers[i], &beginInfo ) != VK_SUCCESS )
      throw std::runtime_error( "command buffer failed to begin recording" );

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain.getRenderPass();
    renderPassInfo.framebuffer = swapChain.getFrameBuffer( i );
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

    std::array< VkClearValue, 2 > clearValues{};
    clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };
    clearValues[1].depthStencil = { 1.f, 0 };
    renderPassInfo.clearValueCount =
        static_cast< uint32_t >( clearValues.size() );
    renderPassInfo.pClearValues = clearValues.data();

    // VK_SUBPASS_CONTENTS_INLINE somehow signals that no secondary command
    // buffers will be used for secondary commands. The alternative is
    // VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS that signifies use of
    // secondary command buffers to execute secondary commands
    vkCmdBeginRenderPass(
        commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    pipeline->bind( commandBuffers[i] );

    model->bind( commandBuffers[i] );
    model->draw( commandBuffers[i] );

    vkCmdEndRenderPass( commandBuffers[i] );

    if ( vkEndCommandBuffer( commandBuffers[i] ) != VK_SUCCESS )
      throw std::runtime_error( "failed to record command buffer" );
  }

}

void FirstApp::drawFrame() {

  uint32_t imageIndex;
  auto result = swapChain.acquireNextImage( &imageIndex );

  if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
    throw std::runtime_error( "failed to acquire swapchain image" );

  result = swapChain.submitCommandBuffers(
      &commandBuffers[imageIndex], &imageIndex );

  if ( result != VK_SUCCESS )
    throw std::runtime_error( "failed to submit command buffers" );
}


void FirstApp::loadModels() {

  std::vector< Model::Vertex > vertices{ { { 0.f, -0.5f } },
                                         { { 0.5f, 0.5f } },
                                         { { -0.5f, 0.5f } } };

  model = std::make_unique< Model >( device, vertices );

}

}  // namespace lve
