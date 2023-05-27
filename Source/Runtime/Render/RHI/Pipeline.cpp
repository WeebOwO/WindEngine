#include "runtime/render/rhi/pipeline.h"

#include "runtime/render/rhi/backend.h"

namespace wind {
vk::Pipeline CreateGraphicsPipeline(const GraphicsShader& shader, vk::RenderPass renderpass, vk::PipelineLayout pipelinelayout) {
    // 1. vertex input
    vk::PipelineVertexInputStateCreateInfo inputStateCreateInfo;
    
    auto bindingDescription    = shader.GetVertexInputBindingDescription();
    auto attributeDescriptions = shader.GetVertexInputAttributeDescriptions();

    inputStateCreateInfo
        .setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()))
        .setVertexBindingDescriptionCount(1)
        .setPVertexBindingDescriptions(&bindingDescription)
        .setPVertexAttributeDescriptions(attributeDescriptions.data());

    // 2. input Assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
    inputAssemblyCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(false);

    // 3. shader stage
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageCreateInfos;
    shaderStageCreateInfos[0]
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(shader.GetVertexShaderModule())
        .setPName("main");

    shaderStageCreateInfos[1]
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(shader.GetFragmentShaderModule())
        .setPName("main");

    // 4. viewport and Rasterization
    vk::PipelineViewportStateCreateInfo viewportCreateinfo;
    const auto [viewportWidth, viewportHeight] = RenderBackend::GetInstance().GetSurfaceExtent();

    vk::Viewport viewport{0, 0, (float)viewportWidth, (float)viewportHeight, 0, 1};
    viewportCreateinfo.setViewports(viewport);

    vk::Rect2D rect{{0, 0}, {viewportWidth, viewportHeight}};
    viewportCreateinfo.setPScissors(&rect).setScissorCount(1);

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.setRasterizerDiscardEnable(false)
        .setDepthClampEnable(false)
        .setCullMode(vk::CullModeFlagBits::eNone)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1);

    // 5. multisample
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.setSampleShadingEnable(false).setRasterizationSamples(
        vk::SampleCountFlagBits::e1);

    // 6. test - setencil, depth
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
    depthStencilStateCreateInfo.setDepthTestEnable(true)
        .setDepthWriteEnable(true)
        .setDepthBoundsTestEnable(false)
        .setStencilTestEnable(false)
        .setMinDepthBounds(0.0f)
        .setMaxDepthBounds(1.0f)
        .setDepthCompareOp(vk::CompareOp::eLess);

    // 7.color blending
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    vk::PipelineColorBlendAttachmentState colorBlendAttachment;

    colorBlendAttachment.setBlendEnable(false).setColorWriteMask(
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colorBlendStateCreateInfo.setLogicOpEnable(false).setAttachments(colorBlendAttachment);

    // 8 dynamic state create
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    std::array dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    dynamicStateCreateInfo.setDynamicStates(dynamicStates);
    // 9 pipeline create
    auto&                        device = RenderBackend::GetInstance().GetDevice();
    vk::GraphicsPipelineCreateInfo createInfo;
    createInfo.setPVertexInputState(&inputStateCreateInfo)
        .setPInputAssemblyState(&inputAssemblyCreateInfo)
        .setStages(shaderStageCreateInfos)
        .setPRasterizationState(&rasterizationStateCreateInfo)
        .setPMultisampleState(&multisampleStateCreateInfo)
        .setPDepthStencilState(&depthStencilStateCreateInfo)
        .setPColorBlendState(&colorBlendStateCreateInfo)
        .setPViewportState(&viewportCreateinfo)
        .setLayout(pipelinelayout)
        .setRenderPass(renderpass);

    return device.createGraphicsPipeline({}, createInfo).value;
}
} // namespace wind
