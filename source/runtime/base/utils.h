#pragma once

#include <vector>
#include <functional>

#include "runtime/render/shader.h"
#include "runtime/render/swapchain.h"

namespace wind::utils {
template <typename T, typename U>
void RemoveNosupportedElems(std::vector<T>& elems, const std::vector<U>& supportedElems,
                            std::function<bool(const T&, const U&)> eq) {
    int i = 0;
    while (i < elems.size()) {
        if (std::find_if(supportedElems.begin(), supportedElems.end(), [&](const U& elem) {
                return eq(elems[i], elem);
            }) == supportedElems.end()) {
            elems.erase(elems.begin() + i);
        } else {
            i++;
        }
    }
}
// RHI related thing
vk::Device&         GetRHIDevice();
vk::PhysicalDevice& GetRHIPhysicalDevice();
vk::CommandPool&    GetRHIGraphicsCmdPool();
vk::Queue&          GetRHIGraphicsQueue();
vk::Queue&          GetRHIPresentQueue();

vk::ShaderModule   CreateShaderModule(const std::vector<char>& shaderCode);
vk::Pipeline       CreateGraphicsPipelines(const vk::GraphicsPipelineCreateInfo& createinfo);
vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& createinfo);
vk::RenderPass     CreateRenderPass(vk::RenderPassCreateInfo createInfo);

vk::Pipeline ChooseDefaultPipeline(uint32_t index, Shader& shader, vk::RenderPass renderpass,
                                   vk::PipelineLayout layout, SwapChain& swapchain);

vk::CommandBuffer BeginSingleTimeCommand();
vk::Format        FindDepthFormat();

void     EndSingleTimeCommands(vk::CommandBuffer);
uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

void TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout,
                           vk::ImageLayout newLayout);
void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property,
                  vk::Buffer& buffer, vk::DeviceMemory& deviceMemory);

void CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
                 vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image,
                 vk::DeviceMemory& memory);

} // namespace wind::utils
