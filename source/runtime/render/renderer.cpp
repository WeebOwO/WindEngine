#include "runtime/render/renderer.h"

#include <limits>
#include <memory>
#include <cstdint>
#include <vcruntime_string.h>
#include <vector>

#include "GLFW/glfw3.h"

#include "runtime/io/io.h"
#include "runtime/render/context.h"
#include "runtime/render/shader.h"
#include "runtime/render/swapchain.h"
#include "runtime/render/utils.h"
#include "runtime/render/window.h"
#include "runtime/render/buffer.h"


static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace wind {
class RenderImpl {
public:
    RenderImpl(Window& window) : m_window(window), m_swapchain(window.width(), window.height(), MAX_FRAMES_IN_FLIGHT), 
    m_vertexBuffer(CreateVertexBuffer()) {
        CreateGraphicPipeline();
        AllocCmdBuffer();
        CreateFence();
        CreateSemaphore();
    }

    void DrawFrame() {
        auto& device = RenderContext::GetInstace().device;

        if(device.waitForFences(m_cmdAvaliableFences[m_currentFrame], true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
            std::cout << "image present failed\n";
        };

        device.resetFences(m_cmdAvaliableFences[m_currentFrame]);

        auto result = device.acquireNextImageKHR(m_swapchain.swapchain, std::numeric_limits<uint32_t>::max(), m_imageAvalilable[m_currentFrame]);
        if(result.result != vk::Result::eSuccess) {
            std::cout << "acquire next image failed!" << std::endl;
        }

        auto index = result.value;
        m_cmdBuffers[m_currentFrame].reset();

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        
        m_cmdBuffers[m_currentFrame].begin(beginInfo);
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

            m_cmdBuffers[m_currentFrame].beginRenderPass(renderPassBeginInfo, {});
            m_cmdBuffers[m_currentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, m_currentPipeline);
            std::vector<vk::Buffer> vertexBuffers = {m_vertexBuffer.buffer};
            std::vector<vk::DeviceSize> offsets = {0};
            m_cmdBuffers[m_currentFrame].bindVertexBuffers(0, 1, vertexBuffers.data(), offsets.data());
            m_cmdBuffers[m_currentFrame].draw(vertices.size(), 1, 0, 0);

            m_cmdBuffers[m_currentFrame].endRenderPass();
        }

        m_cmdBuffers[m_currentFrame].end();

        vk::SubmitInfo submitInfo;
        std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        submitInfo.setCommandBuffers(m_cmdBuffers[m_currentFrame])
                  .setWaitSemaphores(m_imageAvalilable[m_currentFrame])
                  .setSignalSemaphores(m_imageFinished[m_currentFrame])
                  .setWaitDstStageMask(waitStages);

        RenderContext::GetInstace().graphicsQueue.submit(submitInfo, m_cmdAvaliableFences[m_currentFrame]);

        vk::PresentInfoKHR presentInfo;
        presentInfo.setImageIndices(index)
                   .setSwapchains(m_swapchain.swapchain)
                   .setWaitSemaphores(m_imageFinished[m_currentFrame]);
        
        if(RenderContext::GetInstace().presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
             std::cout << "image present failed\n";
        }
        
        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    }

    ~RenderImpl() {
        auto& device = RenderContext::GetInstace().device;
        auto& graphicsPool = RenderContext::GetInstace().graphicsCmdPool;

        for(auto& buffer : m_frameBuffers) {
            device.destroyFramebuffer(buffer);
        }

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            device.destroyFence(m_cmdAvaliableFences[i]);
            device.destroySemaphore(m_imageFinished[i]);
            device.destroySemaphore(m_imageAvalilable[i]);
        }
        device.freeCommandBuffers(graphicsPool, m_cmdBuffers);
        device.destroyRenderPass(m_basepass);
        device.destroyPipelineLayout(m_currentLayout);
        device.destroyPipeline(m_currentPipeline);
    }

private:
    void CreateRenderPass();
    void CreateGraphicPipeline(); 
    void CreateGraphicPipelineLayout();
    void CreateFrameBuffer();
    void CreateFence();
    void CreateSemaphore();
    Buffer CreateVertexBuffer();

    void AllocCmdBuffer();

    const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    uint32_t m_currentFrame {0};
    Window&   m_window;
    SwapChain m_swapchain;

    vk::Pipeline m_currentPipeline;
    vk::PipelineLayout m_currentLayout;
    vk::RenderPass m_basepass;

    std::vector<vk::Framebuffer> m_frameBuffers;

    std::vector<vk::CommandBuffer> m_cmdBuffers;
    std::array<vk::Fence, MAX_FRAMES_IN_FLIGHT> m_cmdAvaliableFences;
    
    std::array<vk::Semaphore, MAX_FRAMES_IN_FLIGHT> m_imageAvalilable;
    std::array<vk::Semaphore, MAX_FRAMES_IN_FLIGHT> m_imageFinished;

    Buffer m_vertexBuffer;
};

void RenderImpl::CreateGraphicPipeline() {
    CreateGraphicPipelineLayout();
    CreateRenderPass();
    CreateFrameBuffer();
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;

    // 1. Vertex Input
    vk::PipelineVertexInputStateCreateInfo inputStateCreateInfo;

    auto bindingDescription = Vertex::GetInputBindingDescription();
    auto attributeDescriptions = Vertex::GetVertexInputAttributeDescriptions();

    inputStateCreateInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()))
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

Buffer RenderImpl::CreateVertexBuffer() {
    auto& device = RenderContext::GetInstace().device;

    size_t size = sizeof(vertices[0]) * vertices.size();
    auto flagbits = vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent;

    Buffer vertexBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer, flagbits);
    
    void* data = device.mapMemory(vertexBuffer.memory, 0, vertexBuffer.size);
    memcpy(data, vertices.data(), size);

    return vertexBuffer;
}

void RenderImpl::AllocCmdBuffer() {
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.setCommandPool(RenderContext::GetInstace().graphicsCmdPool)
                .setCommandBufferCount(MAX_FRAMES_IN_FLIGHT)
                .setLevel(vk::CommandBufferLevel::ePrimary);
    m_cmdBuffers = RenderContext::GetInstace().device.allocateCommandBuffers(allocateInfo);
}

void RenderImpl::CreateFence() {
    // createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for(auto& fence : m_cmdAvaliableFences) {
        vk::FenceCreateInfo createInfo;
        createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        fence = RenderContext::GetInstace().device.createFence(createInfo);
    }
}

void RenderImpl::CreateSemaphore() {
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vk::SemaphoreCreateInfo info;
        m_imageAvalilable[i] = RenderContext::GetInstace().device.createSemaphore(info);
        m_imageFinished[i] = RenderContext::GetInstace().device.createSemaphore(info);
    }
}

Renderer::Renderer(Window& window) : m_impl(std::make_unique<RenderImpl>(window)) {}

Renderer::~Renderer() = default;

void Renderer::DrawFrame() {
    m_impl->DrawFrame();
}

} // namespace wind