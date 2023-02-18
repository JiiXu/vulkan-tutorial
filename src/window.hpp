#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>

#include <string>

namespace lve
{

  class Window {

    private:

      void initWindow();

      const int height;
      const int width;

      std::string windowName;

      GLFWwindow* window;

    public:

      Window( int, int, std::string );
      ~Window();

      bool shouldClose() { return glfwWindowShouldClose( window ); }

  };

}

#endif
