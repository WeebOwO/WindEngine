#include "RenderPass.h"

#include "Runtime/Base/Io.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RenderGraph/RenderPass.h"
#include "Runtime/Resource/Mesh.h"


namespace wind {
RenderProcessBuilder& RenderProcessBuilder::SetShader(GraphicsShader graphicsShader) {
    vk::ShaderModule vertexShaderModule = graphicsShader.GetVertexShaderModule();
    vk::ShaderModule fragShaderModule   = graphicsShader.GetFragmentShaderModule();

    m_shaderStageCreateInfos.resize(2);

    m_shaderStageCreateInfos[0]
        .setModule(vertexShaderModule)
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setPName("main");
    m_shaderStageCreateInfos[1]
        .setModule(fragShaderModule)
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setPName("main");

    return *this;
}

RenderProcessBuilder& RenderProcessBuilder::SetShader(const std::string& vertexShaderFile,
                                                      const std::string& fragShaderFile) {
    auto& device = RenderBackend::GetInstance().GetDevice();

    auto vertexSpirvCode = io::ReadSpirvBinaryFile(vertexShaderFile);
    auto fragSpirvCode   = io::ReadSpirvBinaryFile(fragShaderFile);

    vk::ShaderModuleCreateInfo shaderModuleCrateInfo;
    shaderModuleCrateInfo.setPCode(vertexSpirvCode.data())
                         .setCodeSize(vertexSpirvCode.size() * sizeof(uint32_t));

    vk::ShaderModule vsModule = device.createShaderModule(shaderModuleCrateInfo);
    
    shaderModuleCrateInfo.setPCode(fragSpirvCode.data())
                         .setCodeSize(fragSpirvCode.size() * sizeof(uint32_t));                         
    vk::ShaderModule fsModule = device.createShaderModule(shaderModuleCrateInfo);

    m_shaderStageCreateInfos.resize(2);

    m_shaderStageCreateInfos[0]
        .setModule(vsModule)
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setPName("main");
    m_shaderStageCreateInfos[1]
        .setModule(fsModule)
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setPName("main");
    
    return *this;
}

RenderProcessBuilder& RenderProcessBuilder::SetBlendState(bool blendEnable) {
    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    m_colorBlendAttachment.setBlendEnable(VkBool32(blendEnable))
        .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                           vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

    m_PipelineColorBlendStateCreateInfo.
    setLogicOpEnable(false)
    .setAttachments(m_colorBlendAttachment);
    return *this;
}

RenderProcessBuilder&
RenderProcessBuilder::SetDepthSetencilTestState(bool depthTestEnable, bool depthWriteEnable,
                                                bool          stencilTestEnable,
                                                vk::CompareOp depthCompareMode) {

    m_depthStencilStateCreateInfo.setDepthTestEnable(depthTestEnable)
        .setDepthWriteEnable(depthWriteEnable)
        .setDepthBoundsTestEnable(false)
        .setStencilTestEnable(stencilTestEnable)
        .setMinDepthBounds(0.0f)
        .setMaxDepthBounds(1.0f)
        .setDepthCompareOp(depthCompareMode);

    return *this;
}

RenderProcessBuilder& RenderProcessBuilder::SetRenderPass(vk::RenderPass renderPass) {
    m_renderPass = renderPass;
    return *this;
}

std::shared_ptr<RenderProcess> RenderProcessBuilder::BuildGraphicProcess() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    // vertex input
    vk::PipelineVertexInputStateCreateInfo inputStateCreateInfo;

    auto vertexAttributeDescriptions = Vertex::GetVertexInputAttributeDescriptions();
    auto vertexInputBindings         = Vertex::GetInputBindingDescription();

    inputStateCreateInfo.setVertexAttributeDescriptions(vertexAttributeDescriptions)
        .setVertexAttributeDescriptionCount(vertexAttributeDescriptions.size())
        .setVertexBindingDescriptions(vertexInputBindings)
        .setVertexBindingDescriptionCount(1);

    // Input Assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
    inputAssemblyCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(false);

    // viewport and Rasterization
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

    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.setSampleShadingEnable(false).setRasterizationSamples(
        vk::SampleCountFlagBits::e1);

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    vk::PipelineLayout pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);

    vk::GraphicsPipelineCreateInfo createInfo;
    createInfo.setPVertexInputState(&inputStateCreateInfo)
        .setPInputAssemblyState(&inputAssemblyCreateInfo)
        .setStages(m_shaderStageCreateInfos)
        .setPRasterizationState(&rasterizationStateCreateInfo)
        .setPMultisampleState(&multisampleStateCreateInfo)
        .setPDepthStencilState(&m_depthStencilStateCreateInfo)
        .setPColorBlendState(&m_PipelineColorBlendStateCreateInfo)
        .setPViewportState(&viewportCreateinfo)
        .setLayout(pipelineLayout)
        .setRenderPass(m_renderPass);

    vk::Pipeline pipeline = device.createGraphicsPipeline({}, createInfo).value;

    return std::make_shared<RenderProcess>(pipeline, pipelineLayout,
                                           vk::PipelineBindPoint::eGraphics);
}
} // namespace wind