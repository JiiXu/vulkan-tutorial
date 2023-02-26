#include "window.hpp"

#include <stdexcept>

namespace lve {

Window::Window( int _width, int _height, std::string _windowName )
    : width{ _width }, height{ _height }, windowName{ _windowName } {
  initWindow();
}

Window::~Window() {
  glfwDestroyWindow( window );
  glfwTerminate();
}

void Window::createWindowSurface( VkInstance instance, VkSurfaceKHR* surface ) {
  // note: the nullptr here is an "allocator callback"
  if ( glfwCreateWindowSurface( instance, window, nullptr, surface ) !=
       VK_SUCCESS ) {
    throw std::runtime_error( "failed to create window surface" );
  }
}

void Window::initWindow() {
  // initialize glfw window
  glfwInit();

  // do not create an OpenGL context
  glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

  glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );

  // create the window
  // note: the fourth parameter is for fullscreen, the fifth is related only to
  // OpenGL contexts
  window =
      glfwCreateWindow( width, height, windowName.c_str(), nullptr, nullptr );
  glfwSetWindowUserPointer( window, this );
  glfwSetFramebufferSizeCallback( window, framebufferResizeCallback );

}

void Window::framebufferResizeCallback(
  GLFWwindow* window, int width, int height ) {

  auto resizedWindow = static_cast< Window* >( glfwGetWindowUserPointer( window ) );
  resizedWindow->windowResized = true;
  resizedWindow->width = width;
  resizedWindow->height = height;

}

}  // namespace lve
