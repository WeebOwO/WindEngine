#include "runtime/render/renderer.h"

#include <cstdint>
#include <limits>
#include <memory>
#include <vcruntime_string.h>
#include <vector>

#include "GLFW/glfw3.h"

#include "runtime/io/io.h"
#include "runtime/render/buffer.h"
#include "runtime/render/context.h"
#include "runtime/render/shader.h"
#include "runtime/render/swapchain.h"
#include "runtime/render/utils.h"
#include "runtime/render/window.h"

static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace wind {
class RenderImpl {
public:
    RenderImpl(Window& window)
        : m_window(window), m_swapchain(window.width(), window.height(), MAX_FRAMES_IN_FLIGHT),
          m_vertexBuffer(CreateBuffer()) {
        CreateRenderPass();
        CreateFrameBuffer();
        CreateGraphicPipeline();
        AllocCmdBuffer();
        CreateFence();
        CreateSemaphore();
    }

    void DrawFrame() {
        auto& device = utils::GetRHIDevice();

        if (device.waitForFences(m_cmdAvaliableFences[m_currentFrame], true,
                                 std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
            std::cout << "image present failed\n";
        };

        device.resetFences(m_cmdAvaliableFences[m_currentFrame]);

        auto result =
            device.acquireNextImageKHR(m_swapchain.swapchain, std::numeric_limits<uint32_t>::max(),
                                       m_imageAvalilable[m_currentFrame]);

        if (result.result != vk::Result::eSuccess) {
            std::cout << "acquire next image failed!" << std::endl;
        }

        auto index = result.value;
        m_cmdBuffers[m_currentFrame].reset();

        RecordCmdBuffer(m_cmdBuffers[index], index);

        vk::SubmitInfo                        submitInfo;
        std::array<vk::PipelineStageFlags, 1> waitStages = {
            vk::PipelineStageFlagBits::eColorAttachmentOutput};

        submitInfo.setCommandBuffers(m_cmdBuffers[m_currentFrame])
            .setWaitSemaphores(m_imageAvalilable[m_currentFrame])
            .setSignalSemaphores(m_imageFinished[m_currentFrame])
            .setWaitDstStageMask(waitStages);

        RenderContext::GetInstace().graphicsQueue.submit(submitInfo,
                                                         m_cmdAvaliableFences[m_currentFrame]);

        vk::PresentInfoKHR presentInfo;
        presentInfo.setImageIndices(index)
            .setSwapchains(m_swapchain.swapchain)
            .setWaitSemaphores(m_imageFinished[m_currentFrame]);

        if (RenderContext::GetInstace().presentQueue.presentKHR(presentInfo) !=
            vk::Result::eSuccess) {
            std::cout << "image present failed\n";
        }
        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    ~RenderImpl() {
        auto& device       = utils::GetRHIDevice();
        auto& graphicsPool = RenderContext::GetInstace().graphicsCmdPool;

        for (auto& buffer : m_frameBuffers) {
            device.destroyFramebuffer(buffer);
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
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
    void   CreateRenderPass();
    void   CreateGraphicPipeline();
    void   CreateFrameBuffer();
    void   CreateFence();
    void   CreateSemaphore();
    Buffer CreateBuffer();
    void   AllocCmdBuffer();

    void RecordCmdBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

    const std::vector<Vertex> vertices = {{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    uint32_t m_currentFrame{0};

    Window&   m_window;
    SwapChain m_swapchain;

    vk::RenderPass m_basepass;

    vk::Pipeline       m_currentPipeline;
    vk::PipelineLayout m_currentLayout;

    std::vector<vk::Framebuffer> m_frameBuffers;

    std::vector<vk::CommandBuffer>              m_cmdBuffers;
    std::array<vk::Fence, MAX_FRAMES_IN_FLIGHT> m_cmdAvaliableFences;

    std::array<vk::Semaphore, MAX_FRAMES_IN_FLIGHT> m_imageAvalilable;
    std::array<vk::Semaphore, MAX_FRAMES_IN_FLIGHT> m_imageFinished;

    Buffer m_vertexBuffer;
};

void RenderImpl::CreateGraphicPipeline() {
    // Shader
    std::vector<char> vertexShaderCode   = io::ReadFile("simple_shader.vert.spv");
    std::vector<char> fragmentShaderCode = io::ReadFile("simple_shader.frag.spv");

    std::cout << vertexShaderCode.size() << " " << fragmentShaderCode.size() << std::endl;

    Shader shader = Shader(vertexShaderCode, fragmentShaderCode);

    m_currentLayout = [&]() {
        vk::PipelineLayoutCreateInfo createInfo;
        return utils::CreatePipelineLayout(createInfo);
    }();

    m_currentPipeline =
        utils::ChooseDefaultPipeline(0, shader, m_basepass, m_currentLayout, m_swapchain);
}

void RenderImpl::CreateRenderPass() {
    vk::AttachmentDescription attachmentDescription{};

    attachmentDescription.setFormat(m_swapchain.swapchainInfo.surfaceFormat.format)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setSamples(vk::SampleCountFlagBits::e1);

    vk::AttachmentReference attachmentReference;
    attachmentReference.setAttachment(0).setLayout(vk::ImageLayout::eColorAttachmentOptimal);

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
    auto& device = utils::GetRHIDevice();
    m_frameBuffers.resize(m_swapchain.images.size());
    for (size_t i = 0; const auto& view : m_swapchain.imageViews) {
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
        .setCommandBufferCount(MAX_FRAMES_IN_FLIGHT)
        .setLevel(vk::CommandBufferLevel::ePrimary);
    m_cmdBuffers = RenderContext::GetInstace().device.allocateCommandBuffers(allocateInfo);
}

void RenderImpl::CreateFence() {
    // createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (auto& fence : m_cmdAvaliableFences) {
        vk::FenceCreateInfo createInfo;
        createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        fence = RenderContext::GetInstace().device.createFence(createInfo);
    }
}

void RenderImpl::CreateSemaphore() {
    auto& device = utils::GetRHIDevice();
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vk::SemaphoreCreateInfo info;
        m_imageAvalilable[i] = device.createSemaphore(info);
        m_imageFinished[i]   = device.createSemaphore(info);
    }
}

Buffer RenderImpl::CreateBuffer() {
    auto& device = RenderContext::GetInstace().device;

    size_t size = sizeof(vertices[0]) * vertices.size();
    auto   flagbits =
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    Buffer vertexBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer, flagbits);

    void* data = device.mapMemory(vertexBuffer.memory, 0, vertexBuffer.size);
    memcpy(data, vertices.data(), size);
    device.unmapMemory(m_vertexBuffer.memory);
    return vertexBuffer;
}

void RenderImpl::RecordCmdBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) {
    // simple base pass, can multithread
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    commandBuffer.begin(beginInfo);
    {
        vk::RenderPassBeginInfo renderPassBeginInfo;
        vk::Rect2D              renderArea;
        vk::ClearValue          clearValue;
        clearValue.setColor(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f});

        renderArea.setOffset({0, 0}).setExtent(m_swapchain.swapchainInfo.imageExtent);

        renderPassBeginInfo.setRenderPass(m_basepass)
            .setRenderArea(renderArea)
            .setClearValues(clearValue)
            .setFramebuffer(m_frameBuffers[imageIndex]);

        commandBuffer.beginRenderPass(renderPassBeginInfo, {});
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_currentPipeline);
        std::vector<vk::Buffer>     vertexBuffers = {m_vertexBuffer.buffer};
        std::vector<vk::DeviceSize> offsets       = {0};
        commandBuffer.bindVertexBuffers(0, 1, vertexBuffers.data(), offsets.data());
        commandBuffer.draw(vertices.size(), 1, 0, 0);

        commandBuffer.endRenderPass();
    }

    commandBuffer.end();
}

Renderer::Renderer(Window& window) : m_impl(std::make_unique<RenderImpl>(window)) {}

Renderer::~Renderer() = default;

void Renderer::DrawFrame() { m_impl->DrawFrame(); }

} // namespace wind