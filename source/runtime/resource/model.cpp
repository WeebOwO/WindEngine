#include "runtime/resource/model.h"

#include "runtime/base/utils.h"
#include <array>

namespace wind {
Model::Model(const std::vector<Vertex>& vertices) : m_vertexCnt(vertices.size()) {
    CreateVertexBuffer(vertices);
}

Model::~Model() {
    auto& device = utils::GetRHIDevice();
    device.destroyBuffer(m_vertexBuffer);
    device.freeMemory(m_vertexBufferMemory);
}

void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
    auto&          device     = utils::GetRHIDevice();
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCnt;
    utils::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eVertexBuffer,
                        vk::MemoryPropertyFlagBits::eHostVisible |
                            vk::MemoryPropertyFlagBits::eHostCoherent,
                        m_vertexBuffer, m_vertexBufferMemory);
    void* data;
    data = device.mapMemory(m_vertexBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    device.unmapMemory(m_vertexBufferMemory);
}

void Model::bind(vk::CommandBuffer cmdBuffer) {
    std::array<vk::Buffer, 1>     buffers = {m_vertexBuffer};
    std::array<vk::DeviceSize, 1> offsets = {0};
    cmdBuffer.bindVertexBuffers(0, 1, buffers.data(), offsets.data());
}

void Model::Draw(vk::CommandBuffer cmdBuffer) { cmdBuffer.draw(m_vertexCnt, 1, 0, 0); }
} // namespace wind