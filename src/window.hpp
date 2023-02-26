#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace lve {

class Window {
 private:
  void initWindow();

  int height;
  int width;
  bool windowResized;

  std::string windowName;

  GLFWwindow* window;

  static void framebufferResizeCallback( GLFWwindow*, int, int );

 public:
  Window( int, int, std::string );
  ~Window();
  Window( const Window& ) = delete;
  Window& operator=( const Window& ) = delete;

  bool shouldClose() { return glfwWindowShouldClose( window ); }
  bool wasResized() { return windowResized; }
  void resetResizedFlag() { windowResized = false; }

  VkExtent2D getExtent() {
    return { static_cast< uint32_t >( width ),
             static_cast< uint32_t >( height ) };
  }

  void createWindowSurface( VkInstance, VkSurfaceKHR* );
};

}  // namespace lve
