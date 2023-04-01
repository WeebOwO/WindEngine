#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "runtime/resource/vertex.h"

namespace wind {
class Model {
public:
    Model(const std::vector<Vertex>& vertices);
    ~Model();

    void bind(vk::CommandBuffer cmdBuffer);
    void Draw(vk::CommandBuffer cmdBuffer);

private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);

    vk::Buffer m_vertexBuffer;
    vk::DeviceMemory m_vertexBufferMemory;
    uint32_t m_vertexCnt;
};
} // namespace wind