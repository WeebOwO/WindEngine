#include "runtime/render/renderer.h"

#include <memory>
#include <cstdint>

#include "GLFW/glfw3.h"

#include "runtime/io/io.h"
#include "runtime/render/context.h"
#include "runtime/render/shader.h"
#include "runtime/render/swapchain.h"
#include "runtime/render/utils.h"
#include "runtime/render/window.h"

namespace wind {
class RenderImpl {
public:
    RenderImpl(Window& window) : m_window(window), m_swapchain(window.width(), window.height()) {
        CreateGraphicPipeline();
    }
    ~RenderImpl() {
        auto& device = RenderContext::GetInstace().device;
        device.destroyRenderPass(m_renderpass);
        device.destroyPipelineLayout(m_currentLayout);
        device.destroyPipeline(m_currentPipeline);
    }
private:
    Window&   m_window;
    SwapChain m_swapchain;

    vk::Pipeline m_currentPipeline;
    vk::PipelineLayout m_currentLayout;
    vk::RenderPass m_renderpass;

    void CreateRenderPass();
    void CreateGraphicPipeline(); 
    void CreateGraphicPipelineLayout();
};

void RenderImpl::CreateGraphicPipeline() {
    CreateGraphicPipelineLayout();
    CreateRenderPass();

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;

    // 1. Vertex Input
    vk::PipelineVertexInputStateCreateInfo inputState;
    pipelineCreateInfo.setPVertexInputState(&inputState);

    // 2. Input Assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
    inputAssemblyCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList)
                           .setPrimitiveRestartEnable(false);
    pipelineCreateInfo.setPInputAssemblyState(&inputAssemblyCreateInfo);
    
    // 3. Shader stage
    std::vector<char> vertexShaderCode   = io::ReadFile("simple_shader.vert.spv");
    std::vector<char> fragmentShaderCode = io::ReadFile("simple_shader.frag.spv");

    std::cout << vertexShaderCode.size() << " " << fragmentShaderCode.size() << std::endl;    

    Shader shader = Shader(vertexShaderCode, fragmentShaderCode);

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageCreateInfos;
    shaderStageCreateInfos[0].setStage(vk::ShaderStageFlagBits::eVertex)
                         .setModule(shader.vertexShaderModule())
                         .setPName("main");
    shaderStageCreateInfos[1].setStage(vk::ShaderStageFlagBits::eFragment)
                             .setModule(shader.fragmentShaderModule())
                             .setPName("main");
    pipelineCreateInfo.setStages(shaderStageCreateInfos);

    // 4. Viewport and Rasterization 
    vk::PipelineViewportStateCreateInfo viewportCreateinfo;
    vk::Viewport viewport {0, 0,(float)m_window.width(), (float)m_window.height(), 0, 1};
    viewportCreateinfo.setViewports(viewport);

    vk::Rect2D rect{{0, 0}, {m_window.width(), m_window.height()}};
    viewportCreateinfo.setPScissors(&rect);
    pipelineCreateInfo.setPViewportState(&viewportCreateinfo);

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.setRasterizerDiscardEnable(true)
                                .setCullMode(vk::CullModeFlagBits::eBack)
                                .setFrontFace(vk::FrontFace::eCounterClockwise)
                                .setPolygonMode(vk::PolygonMode::eFill)
                                .setLineWidth(1);

    pipelineCreateInfo.setPRasterizationState(&rasterizationStateCreateInfo);

    // 5. multisample
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.setSampleShadingEnable(false)
                               .setRasterizationSamples(vk::SampleCountFlagBits::e1);
    pipelineCreateInfo.setPMultisampleState(&multisampleStateCreateInfo);

    // 6. Test - setencil, depth

    // 7. Color blending
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    vk::PipelineColorBlendAttachmentState colorBlendAttachment;

    colorBlendAttachment.setBlendEnable(false)
                        .setColorWriteMask(vk::ColorComponentFlagBits::eR | 
                        vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colorBlendStateCreateInfo.setLogicOpEnable(false)
                             .setAttachments(colorBlendAttachment);
    pipelineCreateInfo.setPColorBlendState(&colorBlendStateCreateInfo);

    // 8. Add renderpass and pipelineLayout
    pipelineCreateInfo.setLayout(m_currentLayout)   
                      .setRenderPass(m_renderpass);

    m_currentPipeline = utils::CreateGraphicsPipelines(pipelineCreateInfo);
}

void RenderImpl::CreateGraphicPipelineLayout() {
    vk::PipelineLayoutCreateInfo createInfo;
    m_currentLayout = utils::CreatePipelineLayout(createInfo);
}

void RenderImpl::CreateRenderPass() {
    vk::AttachmentDescription attachmentDescription {};

    attachmentDescription.setFormat(m_swapchain.swapchainInfo.surfaceFormat.format)
                         .setInitialLayout(vk::ImageLayout::eUndefined)
                         .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
                         .setLoadOp(vk::AttachmentLoadOp::eClear)
                         .setStoreOp(vk::AttachmentStoreOp::eStore)
                         .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                         .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                         .setSamples(vk::SampleCountFlagBits::e1);

    vk::AttachmentReference attachmentReference;
    attachmentReference.setAttachment(0)
                       .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpassDescription;
    subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                      .setColorAttachments(attachmentReference)
                      .setColorAttachmentCount(1);

    vk::RenderPassCreateInfo createInfo;   
    createInfo.setSubpassCount(1)
              .setSubpasses(subpassDescription)
              .setAttachmentCount(1)
              .setAttachments(attachmentDescription);

    m_renderpass = utils::CreateRenderPass(createInfo);
}

Renderer::Renderer(Window& window) : m_impl(std::make_unique<RenderImpl>(window)) {}

Renderer::~Renderer() = default;

} // namespace wind