#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace wind {
struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    
    static vk::VertexInputBindingDescription GetInputBindingDescription() {
        vk::VertexInputBindingDescription vertexInputBindingDescription{};
        vertexInputBindingDescription.setBinding(0)
            .setStride(sizeof(Vertex))
            .setInputRate(vk::VertexInputRate::eVertex);
        return vertexInputBindingDescription;
    };

    static std::array<vk::VertexInputAttributeDescription, 2>
    GetVertexInputAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 2> vertexInputAttributeDescription;
        vertexInputAttributeDescription[0]
            .setBinding(0)
            .setLocation(0)
            .setFormat(vk::Format::eR32G32Sfloat)
            .setOffset(offsetof(Vertex, pos));

        vertexInputAttributeDescription[1]
            .setBinding(0)
            .setLocation(1)
            .setFormat(vk::Format::eR32G32B32Sfloat)
            .setOffset(offsetof(Vertex, color));
        return vertexInputAttributeDescription;
    }
};

} // namespace wind