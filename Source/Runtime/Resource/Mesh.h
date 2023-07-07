#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "Runtime/Render/Rhi/Buffer.h"
#include "Runtime/Render/Rhi/CommandBuffer.h"
#include "Runtime/Resource/Material.h"

namespace wind {
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texcoord;

    static vk::VertexInputBindingDescription GetInputBindingDescription() {
        vk::VertexInputBindingDescription vertexInputBindingDescription{};
        vertexInputBindingDescription.setBinding(0)
            .setStride(sizeof(Vertex))
            .setInputRate(vk::VertexInputRate::eVertex);
        return vertexInputBindingDescription;
    };

    static std::vector<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescriptions() {
        std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescription(5);

        vertexInputAttributeDescription[0]
            .setBinding(0)
            .setLocation(0)
            .setFormat(vk::Format::eR32G32B32Sfloat)
            .setOffset(offsetof(Vertex, position));

        vertexInputAttributeDescription[1]
            .setBinding(0)
            .setLocation(1)
            .setFormat(vk::Format::eR32G32B32Sfloat)
            .setOffset(offsetof(Vertex, normal));

        vertexInputAttributeDescription[2]
            .setBinding(0)
            .setLocation(2)
            .setFormat(vk::Format::eR32G32B32Sfloat)
            .setOffset(offsetof(Vertex, tangent));

        vertexInputAttributeDescription[3]
            .setBinding(0)
            .setLocation(3)
            .setFormat(vk::Format::eR32G32B32Sfloat)
            .setOffset(offsetof(Vertex, bitangent));

        vertexInputAttributeDescription[4]
            .setBinding(0)
            .setLocation(4)
            .setFormat(vk::Format::eR32G32Sfloat)
            .setOffset(offsetof(Vertex, texcoord));

        return vertexInputAttributeDescription;
    }
};

struct Model {
public:
    struct Builder {
        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;
        Material              material;
    };

    Model(Builder builder);
    ~Model();

    void Bind(CommandBuffer& cmdbuffer);
    void Draw(CommandBuffer& cmdbuffer);

    [[nodiscard]] auto GetVerTexBuffer() { return m_vertexBuffer; }
    [[nodiscard]] auto GetIndexBuffer() { return m_indexBuffer; }

    void SetMaterial(const Material& material) {m_material = material;}

private:
    std::shared_ptr<Buffer> m_vertexBuffer{nullptr};
    std::shared_ptr<Buffer> m_indexBuffer{nullptr};
    Material                m_material;
    uint32_t                m_vertexCnt{0};
    uint32_t                m_indexCnt{0};
    bool                    m_isDynamic{false};
};

struct MeshBatch {};

} // namespace wind