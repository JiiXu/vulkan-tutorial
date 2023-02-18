#include "window.hpp"

namespace lve {

  Window::Window( int _width, int _height, std::string _windowName )
    : width{ _width }, height{ _height }, windowName{ _windowName }
  {

    initWindow();

  }

  Window::~Window() {

    glfwDestroyWindow( window );
    glfwTerminate();

  }

  void Window::initWindow() {

    // initialize glfw window
    glfwInit();

    // do not create an OpenGL context
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

    // set the window to not be resizable (as we will handle resizing ourselves)
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

    // create the window
    // note: the fourth parameter is for fullscreen, the fifth is related only to OpenGL contexts
    window = glfwCreateWindow( width, height, windowName.c_str(), nullptr, nullptr );


  }


}
