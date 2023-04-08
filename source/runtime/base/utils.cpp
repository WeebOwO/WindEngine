#include "runtime/base/utils.h"

#include <cstdint>

#include "runtime/base/macro.h"
#include "runtime/render/context.h"
#include "runtime/resource/vertex.h"

namespace wind::utils {
vk::Device&         GetRHIDevice() { return RenderContext::GetInstace().device; }
vk::PhysicalDevice& GetRHIPhysicalDevice() { return RenderContext::GetInstace().physicalDevice; }
vk::CommandPool&    GetRHIGraphicsCmdPool() { return RenderContext::GetInstace().graphicsCmdPool; }
vk::Queue&          GetRHIGraphicsQueue() { return RenderContext::GetInstace().graphicsQueue; }
vk::Queue&          GetRHIPresentQueue() { return RenderContext::GetInstace().presentQueue; }

vk::ShaderModule CreateShaderModule(const std::vector<char>& shaderCode) {
    vk::ShaderModuleCreateInfo createInfo;
    vk::ShaderModule           shaderModule;
    createInfo.setCodeSize(shaderCode.size())
        .setPCode(reinterpret_cast<const uint32_t*>(shaderCode.data()));
    shaderModule = RenderContext::GetInstace().device.createShaderModule(createInfo);
    return shaderModule;
}

vk::Pipeline CreateGraphicsPipelines(const vk::GraphicsPipelineCreateInfo& createinfo) {
    auto result = RenderContext::GetInstace().device.createGraphicsPipeline(nullptr, createinfo);
    if (result.result != vk::Result::eSuccess) {}
    return result.value;
}

vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& createinfo) {
    return RenderContext::GetInstace().device.createPipelineLayout(createinfo);
}

vk::RenderPass CreateRenderPass(vk::RenderPassCreateInfo createInfo) {
    return RenderContext::GetInstace().device.createRenderPass(createInfo);
}

vk::CommandBuffer BeginSingleTimeCommand() {
    auto&                         device = utils::GetRHIDevice();
    vk::CommandBufferAllocateInfo allocateInfo;

    allocateInfo.setCommandPool(utils::GetRHIGraphicsCmdPool())
        .setCommandBufferCount(1)
        .setLevel(vk::CommandBufferLevel::ePrimary);

    vk::CommandBuffer cmdBuffer = device.allocateCommandBuffers(allocateInfo)[0];

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuffer.begin(beginInfo);
    return cmdBuffer;
}

vk::Pipeline ChooseDefaultPipeline(uint32_t index, Shader& shader, vk::RenderPass renderpass,
                                   vk::PipelineLayout layout, SwapChain& swapchain) {
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;

    // 1. Vertex Input
    vk::PipelineVertexInputStateCreateInfo inputStateCreateInfo;

    auto bindingDescription    = Vertex::GetInputBindingDescription();
    auto attributeDescriptions = Vertex::GetVertexInputAttributeDescriptions();

    inputStateCreateInfo
        .setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()))
        .setVertexBindingDescriptionCount(1)
        .setPVertexBindingDescriptions(&bindingDescription)
        .setPVertexAttributeDescriptions(attributeDescriptions.data());

    pipelineCreateInfo.setPVertexInputState(&inputStateCreateInfo);

    // 2. Input Assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
    inputAssemblyCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(false);
    pipelineCreateInfo.setPInputAssemblyState(&inputAssemblyCreateInfo);

    // 3. Shader stage
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageCreateInfos;
    shaderStageCreateInfos[0]
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(shader.vertexShaderModule())
        .setPName("main");
    shaderStageCreateInfos[1]
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(shader.fragmentShaderModule())
        .setPName("main");
    pipelineCreateInfo.setStages(shaderStageCreateInfos);

    // 4. Viewport and Rasterization
    vk::PipelineViewportStateCreateInfo viewportCreateinfo;
    const auto [viewportWidth, viewportHeight] = swapchain.GetViewPortSize();

    vk::Viewport viewport{0, 0, (float)viewportWidth, (float)viewportHeight, 0, 1};
    viewportCreateinfo.setViewports(viewport);

    vk::Rect2D rect{{0, 0}, {viewportWidth, viewportHeight}};
    viewportCreateinfo.setPScissors(&rect).setScissorCount(1);

    pipelineCreateInfo.setPViewportState(&viewportCreateinfo);

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.setRasterizerDiscardEnable(false)
        .setCullMode(vk::CullModeFlagBits::eFront)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1);

    pipelineCreateInfo.setPRasterizationState(&rasterizationStateCreateInfo);

    // 5. multisample
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.setSampleShadingEnable(false).setRasterizationSamples(
        vk::SampleCountFlagBits::e1);
    pipelineCreateInfo.setPMultisampleState(&multisampleStateCreateInfo);

    // 6. Test - setencil, depth

    // 7. Color blending
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    vk::PipelineColorBlendAttachmentState colorBlendAttachment;

    colorBlendAttachment.setBlendEnable(false).setColorWriteMask(
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colorBlendStateCreateInfo.setLogicOpEnable(false).setAttachments(colorBlendAttachment);
    pipelineCreateInfo.setPColorBlendState(&colorBlendStateCreateInfo);

    // 8. Add renderpass and pipelineLayout
    pipelineCreateInfo.setLayout(layout).setRenderPass(renderpass);

    return utils::CreateGraphicsPipelines(pipelineCreateInfo);
}

uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    auto& phyDevice     = utils::GetRHIPhysicalDevice();
    auto  memProperties = phyDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    return -1;
}

void EndSingleTimeCommands(vk::CommandBuffer cmdBuffer) {
    cmdBuffer.end();

    auto& rhi = RenderContext::GetInstace();
    // submit
    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmdBuffer);
    rhi.graphicsQueue.submit(submitInfo);
    rhi.device.waitIdle();
    rhi.device.freeCommandBuffers(rhi.graphicsCmdPool, cmdBuffer);
}

void TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout,
                           vk::ImageLayout newLayout) {
    vk::CommandBuffer cmdBuffer = utils::BeginSingleTimeCommand();

    vk::ImageMemoryBarrier    barrier;
    vk::ImageSubresourceRange range;

    range.setLevelCount(1)
         .setBaseArrayLayer(0)
         .setLayerCount(1)
         .setBaseMipLevel(0)
         .setAspectMask(vk::ImageAspectFlagBits::eColor);

    vk::PipelineStageFlags souceStage, destinationStage;
    if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
        souceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
               .setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        souceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    
    barrier.setImage(image)
        .setOldLayout(oldLayout)
        .setNewLayout(newLayout)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setSubresourceRange(range);

    cmdBuffer.pipelineBarrier(souceStage,
                              destinationStage, {}, {}, nullptr, barrier);
    EndSingleTimeCommands(cmdBuffer);
}

void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height) {
    vk::CommandBuffer cmdBuffer = BeginSingleTimeCommand();

    vk::BufferImageCopy region;
    vk::ImageSubresourceLayers subsource;

    subsource.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setMipLevel(0)
            .setLayerCount(1);

    region.setBufferImageHeight(0)
            .setBufferOffset(0)
            .setImageOffset(0)
            .setImageExtent({width, height, 1})
            .setBufferRowLength(0)
            .setImageSubresource(subsource);

    cmdBuffer.copyBufferToImage(buffer, image,
                                vk::ImageLayout::eTransferDstOptimal,
                                region);
    
    EndSingleTimeCommands(cmdBuffer);
}

void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
    vk::CommandBuffer immCmdBuffer = BeginSingleTimeCommand();

    vk::BufferCopy copyRegion;
    copyRegion.setDstOffset(0).setSize(size).setSrcOffset(0);
    immCmdBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

    EndSingleTimeCommands(immCmdBuffer);
}

void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property,
                  vk::Buffer& buffer, vk::DeviceMemory& deviceMemory) {

    auto& device    = utils::GetRHIDevice();
    auto& phyDevice = utils::GetRHIPhysicalDevice();
    // create buffer
    vk::BufferCreateInfo createInfo;
    createInfo.setSize(size).setUsage(usage).setSharingMode(vk::SharingMode::eExclusive);

    buffer = device.createBuffer(createInfo);
    // query memory info
    auto properties   = phyDevice.getMemoryProperties();
    auto requirements = device.getBufferMemoryRequirements(buffer);

    uint32_t index = FindMemoryType(requirements.memoryTypeBits, property);

    vk::MemoryAllocateInfo allocateInfo;
    allocateInfo.setMemoryTypeIndex(index).setAllocationSize(requirements.size);

    deviceMemory = device.allocateMemory(allocateInfo);

    device.bindBufferMemory(buffer, deviceMemory, 0);
}
} // namespace wind::utils