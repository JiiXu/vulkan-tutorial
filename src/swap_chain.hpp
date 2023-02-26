#pragma once

#include "device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace lve {

class SwapChain {
 public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  SwapChain( Device&, VkExtent2D );
  SwapChain( Device&, VkExtent2D, std::shared_ptr< SwapChain > );
  ~SwapChain();

  SwapChain( const SwapChain& ) = delete;
  SwapChain& operator=( const SwapChain& ) = delete;

  VkFramebuffer getFrameBuffer( int index ) {
    return swapChainFramebuffers[index];
  }

  float extentAspectRatio() {
    return static_cast< float >( swapChainExtent.width ) /
           static_cast< float >( swapChainExtent.height );
  }

  VkFormat findDepthFormat();

  VkResult acquireNextImage( uint32_t* );
  VkResult submitCommandBuffers( const VkCommandBuffer*, uint32_t* );

  VkRenderPass getRenderPass() { return renderPass; }
  VkImageView getImageView( int index ) { return swapChainImageViews[index]; }
  size_t imageCount() { return swapChainImages.size(); }
  VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
  VkExtent2D getSwapChainExtent() { return swapChainExtent; }
  uint32_t width() { return swapChainExtent.width; }
  uint32_t height() { return swapChainExtent.height; }

 private:
  std::shared_ptr< SwapChain > oldSwapChain;

  void createSwapChain();
  void createImageViews();
  void createDepthResources();
  void createRenderPass();
  void createFramebuffers();
  void createSyncObjects();
  void init();

  // Helper functions
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector< VkSurfaceFormatKHR >& );
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector< VkPresentModeKHR >& );
  VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR& );

  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;

  std::vector< VkFramebuffer > swapChainFramebuffers;
  VkRenderPass renderPass;

  std::vector< VkImage > depthImages;
  std::vector< VkDeviceMemory > depthImageMemorys;
  std::vector< VkImageView > depthImageViews;
  std::vector< VkImage > swapChainImages;
  std::vector< VkImageView > swapChainImageViews;

  Device& device;
  VkExtent2D windowExtent;

  VkSwapchainKHR swapChain;

  std::vector< VkSemaphore > imageAvailableSemaphores;
  std::vector< VkSemaphore > renderFinishedSemaphores;
  std::vector< VkFence > inFlightFences;
  std::vector< VkFence > imagesInFlight;
  size_t currentFrame = 0;
};

}  // namespace lve
