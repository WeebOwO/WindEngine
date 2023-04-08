#pragma once

#include <stdint.h>
#include <vector>

#include "runtime/render/shader.h"
#include "runtime/render/swapchain.h"

namespace wind::utils {
// RHI related thing
vk::Device&         GetRHIDevice();
vk::PhysicalDevice& GetRHIPhysicalDevice();
vk::CommandPool& GetRHIGraphicsCmdPool();
vk::Queue& GetRHIGraphicsQueue();
vk::Queue& GetRHIPresentQueue();

vk::ShaderModule   CreateShaderModule(const std::vector<char>& shaderCode);
vk::Pipeline       CreateGraphicsPipelines(const vk::GraphicsPipelineCreateInfo& createinfo);
vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& createinfo);
vk::RenderPass     CreateRenderPass(vk::RenderPassCreateInfo createInfo);

vk::Pipeline ChooseDefaultPipeline(uint32_t index, Shader& shader, vk::RenderPass renderpass,
                                   vk::PipelineLayout layout, SwapChain& swapchain);

vk::CommandBuffer BeginSingleTimeCommand();

void EndSingleTimeCommands(vk::CommandBuffer);
uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

void TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property,
                  vk::Buffer& buffer, vk::DeviceMemory& deviceMemory);
} // namespace wind::utils
