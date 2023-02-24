#pragma once

#include <string>
#include <vector>

#include "device.hpp"

namespace lve {

struct PipelineConfigInfo {
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

  VkViewport viewport;
  VkRect2D scissor;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class Pipeline {
 private:
  // note: reference as member variable is unsafe, but acceptable here
  // since a pipeline must have a device to exist
  Device& device;

  // vulkan objects - typedeffed pointers
  VkPipeline graphicsPipeline;
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;

  static std::vector< char > readFile( const std::string& );

  void createGraphicsPipeline(
      const std::string&, const std::string&, const PipelineConfigInfo& );

  void createShaderModule( const std::vector< char >&, VkShaderModule* );

 public:
  Pipeline(
      Device&, const std::string&, const std::string&,
      const PipelineConfigInfo& );
  ~Pipeline();
  Pipeline( const Pipeline& ) = delete;
  Pipeline( const Pipeline&& ) = delete;

  static PipelineConfigInfo defaultPipelineConfigInfo( uint32_t, uint32_t );
  void bind( VkCommandBuffer );
};
}  // namespace lve
