#include "model.hpp"

#include <cassert>
#include <cstring>

namespace lve {

Model::Model( Device& _device, std::vector< Vertex >& vertices )
    : device{ _device } {
  createVertexBuffers( vertices );
}

Model::~Model() {
  vkDestroyBuffer( device.device(), vertexBuffer, nullptr );
  vkFreeMemory( device.device(), vertexBufferMemory, nullptr );
}

void Model::createVertexBuffers( const std::vector< Vertex >& vertices ) {
  vertexCount = static_cast< uint32_t >( vertices.size() );

  assert( vertexCount >= 3 && "Vertex count must be at least 3" );

  VkDeviceSize bufferSize = sizeof( vertices[0] ) * vertexCount;

  device.createBuffer(
      bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      vertexBuffer, vertexBufferMemory );

  void* data;

  vkMapMemory( device.device(), vertexBufferMemory, 0, bufferSize, 0, &data );

  // memcpy will copy the data from the cpu (host) onto the gpu (device)
  // because the HOST_COHERENT_BIT bit is set, the memory of the device
  // will automatically be flushed to keep the data on the host and the device
  // synced. If this bit were unset, we would have to do this ourselves.
  memcpy( data, vertices.data(), static_cast< size_t >( bufferSize ) );
  vkUnmapMemory( device.device(), vertexBufferMemory );
}

void Model::draw( VkCommandBuffer commandBuffer ) {
  vkCmdDraw( commandBuffer, vertexCount, 1, 0, 0 );
}

void Model::bind( VkCommandBuffer commandBuffer ) {
  VkBuffer buffers[] = { vertexBuffer };
  VkDeviceSize offsets[] = { 0 };

  vkCmdBindVertexBuffers( commandBuffer, 0, 1, buffers, offsets );
}

std::vector< VkVertexInputBindingDescription >
Model::Vertex::getBindingDescriptions() {
  std::vector< VkVertexInputBindingDescription > bindingDescriptions( 1 );
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof( Vertex );
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescriptions;
}

std::vector< VkVertexInputAttributeDescription >
Model::Vertex::getAttributeDescriptions() {
  std::vector< VkVertexInputAttributeDescription > attributeDescriptions( 1 );
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = 0;

  return attributeDescriptions;
}

std::vector< Model::Vertex > Model::Triangle::getVertices() {
  return std::vector< Model::Vertex >{ a, b, c };
}

}  // namespace lve
