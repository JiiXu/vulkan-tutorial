#pragma once

#include "window.hpp"
#include "pipeline.hpp"

namespace lve {

  class FirstApp {

    private:
      static constexpr int width = 800;
      static constexpr int height = 600;
      Window window{ width, height, "Hello Vulkan!" };

      Device device{ window };

      Pipeline pipeline{
        device,
        "assets/shaders/simple_shader.vert.spv",
        "assets/shaders/simple_shader.frag.spv",
        Pipeline::defaultPipelineConfigInfo( width, height )
      };

    public:

      void run();

  };

}

