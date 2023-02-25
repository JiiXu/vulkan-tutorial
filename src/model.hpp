#pragma once

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace lve {

class Model {
 public:
  struct Vertex {
    glm::vec2 position;

    static std::vector< VkVertexInputBindingDescription >
    getBindingDescriptions();
    static std::vector< VkVertexInputAttributeDescription >
    getAttributeDescriptions();
  };

  struct Triangle {
    Vertex a;
    Vertex b;
    Vertex c;

    std::vector< Vertex > getVertices();
  };

  Model( Device&, std::vector< Vertex >& );
  ~Model();
  Model( const Model& ) = delete;
  Model& operator=( const Model& ) = delete;

  void bind( VkCommandBuffer );
  void draw( VkCommandBuffer );

 private:
  Device& device;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;

  void createVertexBuffers( const std::vector< Vertex >& );
};

}  // namespace lve
