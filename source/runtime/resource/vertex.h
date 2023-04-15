#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

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

    static std::array<vk::VertexInputAttributeDescription, 5>
    GetVertexInputAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 5> vertexInputAttributeDescription;
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

} // namespace wind