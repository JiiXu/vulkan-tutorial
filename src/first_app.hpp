#pragma once

#include <memory>
#include <vector>

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
  SwapChain swapChain{ device, window.getExtent() };
  std::unique_ptr< Model > model;
  std::unique_ptr< Pipeline > pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector< VkCommandBuffer > commandBuffers;

  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();
  void loadModels();

 public:
  FirstApp();
  ~FirstApp();
  FirstApp( const FirstApp& ) = delete;
  FirstApp& operator=( const FirstApp& ) = delete;

  void run();

};

}  // namespace lve
