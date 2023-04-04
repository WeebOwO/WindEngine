#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "runtime/resource/vertex.h"

namespace wind {
class Model {
public:
    struct Builder {
        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;
    };

    Model(const Builder& builder);

    ~Model();

    void Bind(vk::CommandBuffer cmdBuffer);
    void Draw(vk::CommandBuffer cmdBuffer);

private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(const std::vector<uint32_t>& indices);

    vk::Buffer       m_vertexBuffer;
    vk::DeviceMemory m_vertexBufferMemory;
    uint32_t         m_vertexCnt;

    vk::Buffer       m_indexBuffer;
    vk::DeviceMemory m_indexBufferMemory;
    uint32_t         m_indexCnt;
};
} // namespace wind