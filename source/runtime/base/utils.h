#pragma once

#include <vector>

#include "runtime/render/shader.h"
#include "runtime/render/swapchain.h"

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
void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property,
                  vk::Buffer& buffer, vk::DeviceMemory& deviceMemory);
} // namespace wind::utils
