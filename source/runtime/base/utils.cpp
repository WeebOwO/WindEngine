#include "runtime/base/utils.h"

#include <cstdint>

#include "runtime/base/macro.h"
#include "runtime/render/context.h"
#include "runtime/resource/vertex.h"

namespace wind::utils {
vk::Device&         GetRHIDevice() { return RenderContext::GetInstace().device; }
vk::PhysicalDevice& GetRHIPhysicalDevice() { return RenderContext::GetInstace().physicalDevice; }
vk::CommandPool&    GetRHIGraphicsCmdPool() { return RenderContext::GetInstace().graphicsCmdPool; }
vk::Queue& GetRHIGraphicsQueue() {return RenderContext::GetInstace().graphicsQueue;}
vk::Queue& GetRHIPresentQueue() {return RenderContext::GetInstace().presentQueue;}

vk::ShaderModule    CreateShaderModule(const std::vector<char>& shaderCode) {
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

void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocateInfo;
    auto&                         rhi = RenderContext::GetInstace();
    allocateInfo.setCommandBufferCount(1)
        .setCommandPool(rhi.graphicsCmdPool)
        .setLevel(vk::CommandBufferLevel::ePrimary);
    auto                       immCmdBuffer = rhi.device.allocateCommandBuffers(allocateInfo)[0];
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    immCmdBuffer.begin(beginInfo);
    {
        vk::BufferCopy copyRegion;
        copyRegion.setDstOffset(0).setSize(size).setSrcOffset(0);
        immCmdBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
    }
    immCmdBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(immCmdBuffer);
    rhi.graphicsQueue.submit(submitInfo);
    rhi.device.waitIdle();

    rhi.device.freeCommandBuffers(rhi.graphicsCmdPool, immCmdBuffer);
}

void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property,
                  vk::Buffer& buffer, vk::DeviceMemory& deviceMemory) {

    auto& device    = utils::GetRHIDevice();
    auto& phyDevice = utils::GetRHIPhysicalDevice();
    // create buffer
    vk::BufferCreateInfo createInfo;
    createInfo.setSize(size)
              .setUsage(usage)
              .setSharingMode(vk::SharingMode::eExclusive);

    buffer = device.createBuffer(createInfo);
    // query memory info
    auto properties   = phyDevice.getMemoryProperties();
    auto requirements = device.getBufferMemoryRequirements(buffer);

    uint32_t index = -1;
    for (uint32_t i = 0; i < properties.memoryTypeCount; ++i) {
        if (((1 << i) & requirements.memoryTypeBits) &&
            ((properties.memoryTypes[i].propertyFlags & property) == property)) {
            index = i;
            break;
        }
    }

    vk::MemoryAllocateInfo allocateInfo;
    allocateInfo.setMemoryTypeIndex(index).setAllocationSize(requirements.size);

    deviceMemory = device.allocateMemory(allocateInfo);

    device.bindBufferMemory(buffer, deviceMemory, 0);
}
} // namespace wind::utils