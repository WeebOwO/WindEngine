#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "runtime/render/shader.h"
#include "runtime/render/swapchain.h"
#include "runtime/render/buffer.h"

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

namespace wind::utils {
vk::Device&         GetRHIDevice();
vk::PhysicalDevice& GetRHIPhysicalDevice();

vk::ShaderModule   CreateShaderModule(const std::vector<char>& shaderCode);
vk::Pipeline       CreateGraphicsPipelines(const vk::GraphicsPipelineCreateInfo& createinfo);
vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& createinfo);
vk::RenderPass     CreateRenderPass(vk::RenderPassCreateInfo createInfo);

vk::Pipeline ChooseDefaultPipeline(uint32_t index, Shader& shader, vk::RenderPass renderpass,
                                   vk::PipelineLayout layout, SwapChain& swapchain);

void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
} // namespace wind::utils

