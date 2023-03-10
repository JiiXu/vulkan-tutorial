#pragma once

#include <memory>
#include <vector>

#include "game_object.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

namespace lve {

class FirstApp {
 private:
  static constexpr int width = 800;
  static constexpr int height = 600;
  Window window{ width, height, "Hello Vulkan!" };

  Device device{ window };
  std::unique_ptr< SwapChain > swapChain;
  std::unique_ptr< Pipeline > pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector< VkCommandBuffer > commandBuffers;
  std::vector< GameObject > gameObjects;

  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void freeCommandBuffers();
  void drawFrame();
  void loadGameObjects();
  void renderGameObjects( VkCommandBuffer );

  std::vector< Model::Triangle > sierpinskiSplit( Model::Triangle );
  std::vector< Model::Triangle > sierpinski(
      unsigned char, std::vector< Model::Triangle > );

  void recreateSwapChain();
  void recordCommandBuffer( int );

 public:
  FirstApp();
  ~FirstApp();
  FirstApp( const FirstApp& ) = delete;
  FirstApp& operator=( const FirstApp& ) = delete;

  void run();
};

}  // namespace lve
