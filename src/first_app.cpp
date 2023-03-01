#include "first_app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <set>
#include <stdexcept>

struct SimplePushConstantData {
  glm::mat2 transform{ 1.f };
  glm::vec2 offset;
  alignas( 16 ) glm::vec3 color;
};

namespace lve {

void FirstApp::run() {
  while ( !window.shouldClose() ) {
    glfwPollEvents();
    drawFrame();
    vkDeviceWaitIdle( device.device() );
  }
}

FirstApp::FirstApp() {
  loadGameObjects();
  createPipelineLayout();
  recreateSwapChain();
  createCommandBuffers();
}

FirstApp::~FirstApp() {
  vkDestroyPipelineLayout( device.device(), pipelineLayout, nullptr );
}

void FirstApp::createPipelineLayout() {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof( SimplePushConstantData );

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;

  // set layouts are used to push information other than vertices to shaders
  pipelineLayoutInfo.pSetLayouts = nullptr;

  // push constants are a way to send limited data to shader programs
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if ( vkCreatePipelineLayout(
           device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout ) !=
       VK_SUCCESS )
    throw std::runtime_error( "Failed to create pipeline layout" );
}

void FirstApp::createPipeline() {
  assert(
      swapChain != nullptr && "Cannot create pipeline before swap chain! " );
  assert(
      pipelineLayout != nullptr &&
      "Cannot create pipeline before pipeline layout!" );

  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo( pipelineConfig );
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

  if ( swapChain == nullptr ) {
    swapChain = std::make_unique< SwapChain >( device, extent );
  } else {
    swapChain =
        std::make_unique< SwapChain >( device, extent, std::move( swapChain ) );
    if ( swapChain->imageCount() != commandBuffers.size() ) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }

  // check if renderpasses are compatible; if they are, we don't need to
  // recreate the pipeline
  createPipeline();
}

void FirstApp::freeCommandBuffers() {
  vkFreeCommandBuffers(
      device.device(), device.getCommandPool(),
      static_cast< float >( commandBuffers.size() ), commandBuffers.data() );
  commandBuffers.clear();
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

  if ( vkBeginCommandBuffer( commandBuffers[imageIndex], &beginInfo ) !=
       VK_SUCCESS )
    throw std::runtime_error( "command buffer failed to begin recording" );

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = swapChain->getRenderPass();
  renderPassInfo.framebuffer = swapChain->getFrameBuffer( imageIndex );
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

  std::array< VkClearValue, 2 > clearValues{};
  clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.f };
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

  VkViewport viewport{};
  viewport.x = 0.f;
  viewport.y = 0.f;
  viewport.width =
      static_cast< float >( swapChain->getSwapChainExtent().width );
  viewport.height =
      static_cast< float >( swapChain->getSwapChainExtent().height );
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;
  VkRect2D scissor{ { 0, 0 }, swapChain->getSwapChainExtent() };
  vkCmdSetViewport( commandBuffers[imageIndex], 0, 1, &viewport );
  vkCmdSetScissor( commandBuffers[imageIndex], 0, 1, &scissor );

  renderGameObjects( commandBuffers[imageIndex] );

  vkCmdEndRenderPass( commandBuffers[imageIndex] );

  if ( vkEndCommandBuffer( commandBuffers[imageIndex] ) != VK_SUCCESS )
    throw std::runtime_error( "failed to record command buffer" );
}

void FirstApp::renderGameObjects( VkCommandBuffer commandBuffer ) {
  pipeline->bind( commandBuffer );

  for ( auto& object: gameObjects ) {
    object.transform2d.rotation =
        glm::mod( object.transform2d.rotation + 0.01f, glm::two_pi< float >() );

    SimplePushConstantData push{};
    push.offset = object.transform2d.translation;
    push.color = object.color;
    push.transform = object.transform2d.mat2();

    vkCmdPushConstants(
        commandBuffer, pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
        sizeof( SimplePushConstantData ), &push );

    object.model->bind( commandBuffer );
    object.model->draw( commandBuffer );
  }
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

  if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
       window.wasResized() ) {
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

void FirstApp::loadGameObjects() {
  /*
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
*/

  std::vector< Model::Vertex > vertices{ { { 0.f, -0.5f }, { 1.f, 0.f, 0.f } },
                                         { { 0.5f, 0.5f }, { 0.f, 1.f, 0.f } },
                                         { { -0.5f, 0.5f },
                                           { 0.f, 0.f, 1.f } } };

  auto model = std::make_shared< Model >( device, vertices );
  auto triangle = GameObject::createGameObject();
  triangle.model = model;
  triangle.color = { 0.1f, 0.8f, 0.1f };
  triangle.transform2d.translation.x = 0.2f;
  triangle.transform2d.scale = { 2.f, 0.5f };
  triangle.transform2d.rotation = 0.25f * glm::two_pi< float >();
  gameObjects.push_back( std::move( triangle ) );
}

}  // namespace lve
