#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace wind::utils {
    vk::ShaderModule CreateShaderModule(const std::vector<char>& shaderCode);
    vk::Pipeline CreateGraphicsPipelines(const vk::GraphicsPipelineCreateInfo& createinfo);
    vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& createinfo);
    vk::RenderPass CreateRenderPass(vk::RenderPassCreateInfo createInfo);
}