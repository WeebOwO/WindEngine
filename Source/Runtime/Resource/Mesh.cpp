#include "Mesh.h"

#include <memory>

namespace wind {
Model::Model(Builder builder)
    : m_vertexCnt(builder.vertices.size()), m_indexCnt(builder.indices.size()) {
    vk::DeviceSize vertexBufferSize = sizeof(builder.vertices[0]) * m_vertexCnt;
    vk::DeviceSize indexBufferSize  = sizeof(builder.indices[0]) * m_indexCnt;

    m_vertexBuffer = std::make_unique<Buffer>(vertexBufferSize, BufferUsage::VERTEX_BUFFER,
                                              MemoryUsage::CPU_TO_GPU);
    m_indexBuffer  = std::make_unique<Buffer>(indexBufferSize, BufferUsage::INDEX_BUFFER,
                                             MemoryUsage::CPU_TO_GPU);
    
    m_vertexBuffer->CopyData(reinterpret_cast<uint8_t*>(builder.vertices.data()), vertexBufferSize, 0);
    m_indexBuffer->CopyData(reinterpret_cast<uint8_t*>(builder.indices.data()), indexBufferSize, 0);
}

Model::~Model() {
    m_indexBuffer.reset();
    m_vertexBuffer.reset();
}

void Model::Bind(CommandBuffer cmdbuffer) {
    cmdbuffer.BindVertexBuffers(*m_vertexBuffer);
    cmdbuffer.BindIndexBufferUInt32(*m_indexBuffer);
}

void Model::Draw(CommandBuffer cmdbuffer) {
    cmdbuffer.DrawIndexed(m_indexCnt, 1);
}
} // namespace wind