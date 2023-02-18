#ifndef FIRST_APP_H
#define FIRST_APP_H

#include "window.hpp"

namespace lve {

  class FirstApp {

    private:
      static constexpr int width = 800;
      static constexpr int height = 600;
      Window window{ width, height, "Hello Vulkan!" };


    public:

      void run();

  };

}



#endif
