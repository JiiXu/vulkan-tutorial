#pragma once

#include "window.hpp"
#include "lve_pipeline.hpp"

namespace lve {

  class FirstApp {

    private:
      static constexpr int width = 800;
      static constexpr int height = 600;
      Window window{ width, height, "Hello Vulkan!" };

      Pipeline pipeline{ "assets/shaders/simple_shader.vert.spv", "assets/shaders/simple_shader.frag.spv" };

    public:

      void run();

  };

}

