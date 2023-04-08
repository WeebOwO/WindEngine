#include "runtime/resource/model.h"

#include "runtime/base/utils.h"
#include <array>

namespace wind {

Model::Model(const Builder& builder)
    : m_vertexCnt(builder.vertices.size()), m_indexCnt(builder.indices.size()) {
    CreateVertexBuffer(builder.vertices);
    CreateIndexBuffer(builder.indices);
}

Model::~Model() {
    auto& device = utils::GetRHIDevice();
    device.destroyBuffer(m_vertexBuffer);
    device.freeMemory(m_vertexBufferMemory);

    device.destroyBuffer(m_indexBuffer);
    device.freeMemory(m_indexBufferMemory);
}

void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices) {
    auto&          device     = utils::GetRHIDevice();
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCnt;

    vk::Buffer       stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    utils::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                        vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer,
                        stagingBufferMemory);

    void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    device.unmapMemory(stagingBufferMemory);

    utils::CreateBuffer(
        bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal, m_vertexBuffer, m_vertexBufferMemory);

    utils::CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void Model::CreateIndexBuffer(const std::vector<uint32_t>& indices) {
    auto&          device     = utils::GetRHIDevice();
    vk::DeviceSize bufferSize = sizeof(indices[0]) * m_indexCnt;

    vk::Buffer       stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    utils::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                        vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer,
                        stagingBufferMemory);

    void* data;
    data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    device.unmapMemory(stagingBufferMemory);
    utils::CreateBuffer(
        bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal, m_indexBuffer, m_indexBufferMemory);
    
    utils::CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

    device.destroyBuffer(stagingBuffer);
    device.freeMemory(stagingBufferMemory);
}

void Model::Bind(vk::CommandBuffer cmdBuffer) {
    std::array<vk::Buffer, 1>     buffers = {m_vertexBuffer};
    std::array<vk::DeviceSize, 1> offsets = {0};
    cmdBuffer.bindVertexBuffers(0, 1, buffers.data(), offsets.data());
    cmdBuffer.bindIndexBuffer(m_indexBuffer, 0, vk::IndexType::eUint32);
}

void Model::Draw(vk::CommandBuffer cmdBuffer) { 
    cmdBuffer.drawIndexed(m_indexCnt, 1, 0, 0, 0);    
}
} // namespace wind