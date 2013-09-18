#include "vertex_array_buffer.hpp"
#include "glfunctions.hpp"

#include "../base/assert.hpp"

VertexArrayBuffer::VertexArrayBuffer(uint32_t indexBufferSize, uint32_t dataBufferSize)
  : m_VAO(0)
  , m_dataBufferSize(dataBufferSize)
{
  m_VAO = GLFunctions::glGenVertexArray();
  m_indexBuffer.Reset(new IndexBuffer(indexBufferSize));
}

VertexArrayBuffer::~VertexArrayBuffer()
{
  Unbind();

  buffers_map_t::iterator it = m_buffers.begin();
  for (; it != m_buffers.end(); ++it)
    it->second.Destroy();

  m_buffers.clear();
  GLFunctions::glDeleteVertexArray(m_VAO);
}

void VertexArrayBuffer::Bind()
{
  GLFunctions::glBindVertexArray(m_VAO);
}

void VertexArrayBuffer::Unbind()
{
  GLFunctions::glBindVertexArray(0);
}

void VertexArrayBuffer::Render()
{
  Bind();
  GLFunctions::glDrawElements(m_indexBuffer->GetCurrentSize());
}

void VertexArrayBuffer::BuildVertexArray(ReferencePoiner<GpuProgram> program)
{
  if (m_buffers.empty())
    return;

  program->Bind();
  Bind();

  buffers_map_t::iterator it = m_buffers.begin();
  for (; it != m_buffers.end(); ++it)
  {
    const BindingInfo & binding = it->first;
    ReferencePoiner<DataBuffer> buffer = it->second.GetWeakPointer();
    buffer->Bind();

    for (uint16_t i = 0; i < binding.GetCount(); ++i)
    {
      BindingDecl const & decl = binding.GetBindingDecl(i);
      int8_t attributeLocation = program->GetAttributeLocation(decl.m_attributeName);
      assert(attributeLocation != -1);
      GLFunctions::glEnableVertexAttribute(attributeLocation);
      GLFunctions::glVertexAttributePointer(attributeLocation,
                                            decl.m_componentCount,
                                            decl.m_componentType,
                                            false,
                                            decl.m_stride,
                                            decl.m_offset);
    }
    buffer->Unbind();
  }

  m_indexBuffer->Bind();
  Unbind();
  m_indexBuffer->Unbind();
  program->Unbind();
}

ReferencePoiner<GLBuffer> VertexArrayBuffer::GetBuffer(const BindingInfo & bindingInfo)
{
  buffers_map_t::iterator it = m_buffers.find(bindingInfo);
  if (it == m_buffers.end())
  {
    OwnedPointer<DataBuffer> buffer(new DataBuffer(bindingInfo.GetElementSize(), m_dataBufferSize));
    m_buffers.insert(make_pair(bindingInfo, buffer));
    return buffer.GetWeakPointer<GLBuffer>();
  }

  return it->second.GetWeakPointer<GLBuffer>();
}

uint16_t VertexArrayBuffer::GetAvailableIndexCount() const
{
  return m_indexBuffer->GetAvailableSize();
}

uint16_t VertexArrayBuffer::GetAvailableVertexCount() const
{
  if (m_buffers.empty())
    return m_dataBufferSize;

#ifdef DEBUG
  buffers_map_t::const_iterator it = m_buffers.begin();
  uint16_t prev = it->second->GetAvailableSize();
  for (; it != m_buffers.end(); ++it)
    ASSERT(prev == it->second->GetAvailableSize(), ());
#endif

  return m_buffers.begin()->second->GetAvailableSize();
}

uint16_t VertexArrayBuffer::GetStartIndexValue() const
{
  if (m_buffers.empty())
    return 0;

#ifdef DEBUG
  buffers_map_t::const_iterator it = m_buffers.begin();
  uint16_t prev = it->second->GetCurrentSize();
  for (; it != m_buffers.end(); ++it)
    ASSERT(prev == it->second->GetCurrentSize(), ());
#endif

  return m_buffers.begin()->second->GetCurrentSize();
}

void VertexArrayBuffer::UploadIndexes(uint16_t * data, uint16_t count)
{
  ASSERT(count <= m_indexBuffer->GetAvailableSize(), ());
  m_indexBuffer->UploadData(data, count);
}
