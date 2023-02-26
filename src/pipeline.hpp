#pragma once

#include "device.hpp"

#include <string>
#include <vector>

namespace lve {

struct PipelineConfigInfo {
  PipelineConfigInfo( const PipelineConfigInfo& ) = delete;
  PipelineConfigInfo& operator=( const PipelineConfigInfo& ) = delete;

  VkPipelineViewportStateCreateInfo viewportInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  std::vector< VkDynamicState > dynamicStateEnables;
  VkPipelineDynamicStateCreateInfo dynamicStateInfo;
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
  Pipeline& operator=( const Pipeline& ) = delete;

  static void defaultPipelineConfigInfo( PipelineConfigInfo& );
  void bind( VkCommandBuffer );
};
}  // namespace lve
