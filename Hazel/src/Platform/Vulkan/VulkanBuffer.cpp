#include "VulkanBuffer.h"

namespace Hazel {
  /////////////////////////////////////////////////////////////////////////////
  // VertexBuffer //////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
  {
  }

  VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
  {
  }

  VulkanVertexBuffer::~VulkanVertexBuffer()
  {
  }

  void VulkanVertexBuffer::Bind() const
  {
  }

  void VulkanVertexBuffer::UnBind() const
  {
  }

  void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  // IndexBuffer //////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
  {
  }

  VulkanIndexBuffer::~VulkanIndexBuffer()
  {
  }

  void VulkanIndexBuffer::Bind() const
  {
  }

  void VulkanIndexBuffer::UnBind() const
  {
  }
}
