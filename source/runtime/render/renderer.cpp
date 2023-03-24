#include "runtime/render/renderer.h"

#include <limits>
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
        AllocCmdBuffer();
        CreateFence();
        CreateSemaphore();
    }

    void DrawFrame() {
        auto& device = RenderContext::GetInstace().device;
        
        auto result = device.acquireNextImageKHR(m_swapchain.swapchain, std::numeric_limits<uint32_t>::max(), m_imageAvalilable);
        if(result.result != vk::Result::eSuccess) {
            std::cout << "acquire next image failed!" << std::endl;
        }

        auto index = result.value;

        m_cmdBuffer.reset();

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        
        m_cmdBuffer.begin(beginInfo);
        // simple base pass, can multithread
        {
            vk::RenderPassBeginInfo renderPassBeginInfo;
            vk::Rect2D renderArea;
            vk::ClearValue clearValue;
            clearValue.setColor(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f});

            renderArea.setOffset({0, 0})
                      .setExtent(m_swapchain.swapchainInfo.imageExtent);
        
            renderPassBeginInfo.setRenderPass(m_basepass)
                               .setRenderArea(renderArea)
                               .setClearValues(clearValue)
                               .setFramebuffer(m_frameBuffers[index]);

            m_cmdBuffer.beginRenderPass(renderPassBeginInfo, {});

            m_cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_currentPipeline);
            m_cmdBuffer.draw(3, 1, 0, 0);

            m_cmdBuffer.endRenderPass();
        }
        m_cmdBuffer.end();

        vk::SubmitInfo submitInfo;
        std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        submitInfo.setCommandBuffers(m_cmdBuffer)
                  .setWaitSemaphores(m_imageAvalilable)
                  .setSignalSemaphores(m_imageFinished)
                  .setWaitDstStageMask(waitStages);

        RenderContext::GetInstace().graphicsQueue.submit(submitInfo, m_cmdAvaliableFence);

        vk::PresentInfoKHR presentInfo;
        presentInfo.setImageIndices(index)
                   .setSwapchains(m_swapchain.swapchain)
                   .setWaitSemaphores(m_imageFinished);
        
        if(RenderContext::GetInstace().presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
             std::cout << "image present failed\n";
        }

        if(device.waitForFences(m_cmdAvaliableFence, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
            std::cout << "image present failed\n";
        };

        device.resetFences(m_cmdAvaliableFence);
    }
    ~RenderImpl() {
        auto& device = RenderContext::GetInstace().device;
        auto& graphicsPool = RenderContext::GetInstace().graphicsCmdPool;
        for(auto& buffer : m_frameBuffers) {
            device.destroyFramebuffer(buffer);
        }
        device.destroyFence(m_cmdAvaliableFence);
        device.destroySemaphore(m_imageFinished);
        device.destroySemaphore(m_imageAvalilable);
        device.freeCommandBuffers(graphicsPool, m_cmdBuffer);
        device.destroyRenderPass(m_basepass);
        device.destroyPipelineLayout(m_currentLayout);
        device.destroyPipeline(m_currentPipeline);
    }
private:
    Window&   m_window;
    SwapChain m_swapchain;

    vk::Pipeline m_currentPipeline;
    vk::PipelineLayout m_currentLayout;
    vk::RenderPass m_basepass;

    std::vector<vk::Framebuffer> m_frameBuffers;
    vk::CommandBuffer m_cmdBuffer;

    vk::Fence m_cmdAvaliableFence;
    
    vk::Semaphore m_imageAvalilable;
    vk::Semaphore m_imageFinished;

    void CreateRenderPass();
    void CreateGraphicPipeline(); 
    void CreateGraphicPipelineLayout();
    void CreateFrameBuffer();
    void CreateFence();
    void CreateSemaphore();

    void AllocCmdBuffer();
};

void RenderImpl::CreateGraphicPipeline() {
    CreateGraphicPipelineLayout();
    CreateRenderPass();
    CreateFrameBuffer();
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
    viewportCreateinfo.setPScissors(&rect)
                      .setScissorCount(1);
        
    pipelineCreateInfo.setPViewportState(&viewportCreateinfo);

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.setRasterizerDiscardEnable(false)
                                .setCullMode(vk::CullModeFlagBits::eBack)
                                .setFrontFace(vk::FrontFace::eClockwise)
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
                      .setRenderPass(m_basepass);

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
                         .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
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
    vk::SubpassDependency dependency;

    createInfo.setSubpassCount(1)
              .setSubpasses(subpassDescription)
              .setAttachmentCount(1)
              .setAttachments(attachmentDescription);
            
    m_basepass = utils::CreateRenderPass(createInfo);
}

void RenderImpl::CreateFrameBuffer() {
    auto& device = RenderContext::GetInstace().device;
    m_frameBuffers.resize(m_swapchain.images.size());
    for(size_t i = 0; const auto& view : m_swapchain.imageViews) {
        vk::FramebufferCreateInfo info;
        info.setAttachments(view)
            .setWidth(m_window.width())
            .setHeight(m_window.height())
            .setRenderPass(m_basepass)
            .setLayers(1);
        m_frameBuffers[i] = device.createFramebuffer(info);
        ++i;
    }   
}

void RenderImpl::AllocCmdBuffer() {
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.setCommandPool(RenderContext::GetInstace().graphicsCmdPool)
                .setCommandBufferCount(1)
                .setLevel(vk::CommandBufferLevel::ePrimary);
    m_cmdBuffer = RenderContext::GetInstace().device.allocateCommandBuffers(allocateInfo)[0];
}

void RenderImpl::CreateFence() {
    vk::FenceCreateInfo createInfo;
    // createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    m_cmdAvaliableFence = RenderContext::GetInstace().device.createFence(createInfo);
}

void RenderImpl::CreateSemaphore() {
    vk::SemaphoreCreateInfo info;
    m_imageAvalilable = RenderContext::GetInstace().device.createSemaphore(info);
    m_imageFinished = RenderContext::GetInstace().device.createSemaphore(info);
}

Renderer::Renderer(Window& window) : m_impl(std::make_unique<RenderImpl>(window)) {}

Renderer::~Renderer() = default;

void Renderer::DrawFrame() {
    m_impl->DrawFrame();
}

} // namespace wind