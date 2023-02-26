#include "first_app.hpp"

#include <array>
#include <set>
#include <stdexcept>

namespace lve {

void FirstApp::run() {

  while ( !window.shouldClose() ) {
    glfwPollEvents();
    drawFrame();
    vkDeviceWaitIdle( device.device() );
  }

}

FirstApp::FirstApp() {

  loadModels();
  createPipelineLayout();
  recreateSwapChain();
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
      swapChain->width(), swapChain->height() );
  pipelineConfig.renderPass = swapChain->getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;

  pipeline = std::make_unique< Pipeline >(
      device, "assets/shaders/simple_shader.vert.spv",
      "assets/shaders/simple_shader.frag.spv", pipelineConfig );

}

void FirstApp::recreateSwapChain() {

  auto extent = window.getExtent();

  while ( extent.width == 0 || extent.height == 0 ) {
    extent = window.getExtent();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle( device.device() );
  swapChain = nullptr;
  swapChain = std::make_unique< SwapChain >( device, extent );
  createPipeline();

}

void FirstApp::createCommandBuffers() {

  // resize the commandBuffers vector to have as many as we have framebuffers
  // (most probably 2 or 3)
  commandBuffers.resize( swapChain->imageCount() );

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

}

void FirstApp::recordCommandBuffer( int imageIndex ) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if ( vkBeginCommandBuffer( commandBuffers[imageIndex], &beginInfo ) != VK_SUCCESS )
      throw std::runtime_error( "command buffer failed to begin recording" );

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain->getRenderPass();
    renderPassInfo.framebuffer = swapChain->getFrameBuffer( imageIndex );
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

    std::array< VkClearValue, 2 > clearValues{};
    clearValues[0].color = { 0.f, 0.f, 0.f, 0.f };
    clearValues[1].depthStencil = { 1.f, 0 };
    renderPassInfo.clearValueCount =
        static_cast< uint32_t >( clearValues.size() );
    renderPassInfo.pClearValues = clearValues.data();

    // VK_SUBPASS_CONTENTS_INLINE somehow signals that no secondary command
    // buffers will be used for secondary commands. The alternative is
    // VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS that signifies use of
    // secondary command buffers to execute secondary commands
    vkCmdBeginRenderPass(
        commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

    pipeline->bind( commandBuffers[imageIndex] );

    model->bind( commandBuffers[imageIndex] );
    model->draw( commandBuffers[imageIndex] );

    vkCmdEndRenderPass( commandBuffers[imageIndex] );

    if ( vkEndCommandBuffer( commandBuffers[imageIndex] ) != VK_SUCCESS )
      throw std::runtime_error( "failed to record command buffer" );

}

void FirstApp::drawFrame() {

  uint32_t imageIndex;
  auto result = swapChain->acquireNextImage( &imageIndex );

  if ( result == VK_ERROR_OUT_OF_DATE_KHR ) {
    recreateSwapChain();
    return;
  }

  if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
    throw std::runtime_error( "failed to acquire swapchain image" );

  recordCommandBuffer( imageIndex );
  result = swapChain->submitCommandBuffers(
      &commandBuffers[imageIndex], &imageIndex );

  if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasResized() ) {
    window.resetResizedFlag();
    recreateSwapChain();
    return;
  }

  if ( result != VK_SUCCESS )
    throw std::runtime_error( "failed to submit command buffers" );

}

std::vector< Model::Triangle > FirstApp::sierpinskiSplit( Model::Triangle t ) {

  float ax = t.a.position.x;
  float ay = t.a.position.y;
  float bx = t.b.position.x;
  float by = t.b.position.y;
  float cx = t.c.position.x;
  float cy = t.c.position.y;

  // calculate midpoints
  float abx = ( ax + bx ) / 2;
  float aby = ( ay + by ) / 2;
  float bcx = ( bx + cx ) / 2;
  float bcy = ( by + cy ) / 2;
  float cax = ( cx + ax ) / 2;
  float cay = ( cy + ay ) / 2;

  return std::vector< Model::Triangle >{
    { { { ax, ay }, { 0.f, 0.5f, 0.5f } },
      { { abx, aby }, { 0.f, 1.f, 0.f } },
      { { cax, cay }, { 0.f, 0.f, 1.f } } },
    { { { abx, aby }, { 0.f, 0.5f, 0.5f } },
      { { bx, by }, { 0.f, 1.f, 0.f } },
      { { bcx, bcy }, { 0.f, 0.f, 1.f } } },
    { { { cax, cay }, { 0.f, 0.5f, 0.5f } },
      { { bcx, bcy }, { 0.f, 1.f, 0.f } },
      { { cx, cy }, { 0.f, 0.f, 1.f } } }
  };

}

std::vector< Model::Triangle > FirstApp::sierpinski(
    unsigned char depth, std::vector< Model::Triangle > triangles ) {
  if ( depth == 0 ) return triangles;

  // this allocates a bunch of memory just to keep the original triangles out of
  // the final set
  std::vector< Model::Triangle > out;

  for ( auto triangle: triangles ) {
    std::vector< Model::Triangle > newTriangles = sierpinskiSplit( triangle );
    out.insert( out.begin(), newTriangles.begin(), newTriangles.end() );
  }

  return sierpinski( depth - 1, out );
}

void FirstApp::loadModels() {
  std::vector< Model::Triangle > initialTriangle{

    { { { 0.f, -0.5f } }, { { 0.5f, 0.8f } }, { { -0.7f, 0.5f } } }
  };

  std::vector< Model::Triangle > triangles = sierpinski( 3, initialTriangle );
  std::vector< Model::Vertex > vertices( triangles.size() * 3 );

  int j = 0;
  for ( int i = 0; i < triangles.size(); ++i ) {
    // positions
    vertices[j] = triangles[i].a;
    vertices[j + 1] = triangles[i].b;
    vertices[j + 2] = triangles[i].c;

    j += 3;
  }

  model = std::make_unique< Model >( device, vertices );
}

}  // namespace lve
