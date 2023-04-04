#include "runtime/render/renderer.h"

#include <cstdint>
#include <memory>
#include <vcruntime_string.h>
#include <vector>

#include "GLFW/glfw3.h"

#include "runtime/base/macro.h"
#include "runtime/base/utils.h"
#include "runtime/io/io.h"

#include "runtime/render/context.h"
#include "runtime/render/shader.h"
#include "runtime/render/swapchain.h"
#include "runtime/render/window.h"

#include "runtime/resource/scene.h"

static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace wind {
class RenderImpl {
public:
    RenderImpl(Window& window)
        : m_window(window), m_swapchain(window.width(), window.height(), MAX_FRAMES_IN_FLIGHT) {
        CreateRenderPass();
        CreateFrameBuffer();
        CreateGraphicPipeline();
        AllocCmdBuffer();
        CreateSyncObjects();
    }

    void DrawFrame() {
        auto& device = utils::GetRHIDevice();

        if (device.waitForFences(m_cmdAvaliableFences[m_frameIndex], true,
                                 std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
            WIND_CORE_ERROR("image present failed\n");
        };

        device.resetFences(m_cmdAvaliableFences[m_frameIndex]);

        auto result =
            device.acquireNextImageKHR(m_swapchain.swapchain, std::numeric_limits<uint32_t>::max(),
                                       m_imageAvalilable[m_frameIndex]);

        if (result.result != vk::Result::eSuccess) {
            WIND_CORE_ERROR("acquire next image failed!");
        }

        auto index = result.value;
        m_cmdBuffers[m_frameIndex].reset();

        ForwardPass(m_cmdBuffers[m_frameIndex], index);
        RenderPresent(index);

        m_frameIndex = (m_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    ~RenderImpl() {
        auto& device       = utils::GetRHIDevice();
        auto& graphicsPool = utils::GetRHIGraphicsCmdPool();

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
    struct simplePushData {
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    void CreateRenderPass();
    void CreateGraphicPipeline();
    void CreateFrameBuffer();
    void CreateSyncObjects();
    void AllocCmdBuffer();

    void     CreateFrameData();
    void     ForwardPass(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
    void     RenderPresent(uint32_t imageIndex);
    uint32_t m_frameIndex{0};

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
};

void RenderImpl::CreateGraphicPipeline() {
    // Shader
    std::vector<char> vertexShaderCode   = io::ReadFile("simple_shader.vert.spv");
    std::vector<char> fragmentShaderCode = io::ReadFile("simple_shader.frag.spv");

    Shader shader = Shader(vertexShaderCode, fragmentShaderCode);

    m_currentLayout = [&]() {
        vk::PipelineLayoutCreateInfo createInfo;
        vk::PushConstantRange        pushConstantRange;

        // pushConstantRange
        //     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
        //     .setSize(sizeof(simplePushData))
        //     .setOffset(0);

        // createInfo.setPushConstantRangeCount(1).setPushConstantRanges(pushConstantRange);

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

void RenderImpl::CreateSyncObjects() {
    auto& device = utils::GetRHIDevice();

    for (auto& fence : m_cmdAvaliableFences) {
        vk::FenceCreateInfo createInfo;
        createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        fence = device.createFence(createInfo);
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vk::SemaphoreCreateInfo info;
        m_imageAvalilable[i] = device.createSemaphore(info);
        m_imageFinished[i]   = device.createSemaphore(info);
    }
}

void RenderImpl::AllocCmdBuffer() {
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.setCommandPool(utils::GetRHIGraphicsCmdPool())
        .setCommandBufferCount(MAX_FRAMES_IN_FLIGHT)
        .setLevel(vk::CommandBufferLevel::ePrimary);
    m_cmdBuffers = utils::GetRHIDevice().allocateCommandBuffers(allocateInfo);
}

void RenderImpl::ForwardPass(vk::CommandBuffer commandBuffer, uint32_t imageIndex) {
    // simple base pass, can multithread
    auto&                      graphicsQueue = utils::GetRHIGraphicsQueue();
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    auto& gameObjects = Scene::GetWorld().GetWorldGameObjects();

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
        // draw all the object in the scene
        for (const auto& objects : gameObjects) {
            objects.model->Bind(commandBuffer);
            objects.model->Draw(commandBuffer);
        }
        commandBuffer.endRenderPass();
    }
    commandBuffer.end();

    // submit
    vk::SubmitInfo                        submitInfo;
    std::array<vk::PipelineStageFlags, 1> waitStages = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};

    submitInfo.setCommandBuffers(commandBuffer)
        .setWaitSemaphores(m_imageAvalilable[imageIndex])
        .setSignalSemaphores(m_imageFinished[imageIndex])
        .setWaitDstStageMask(waitStages);

    // submit part
    graphicsQueue.submit(submitInfo, m_cmdAvaliableFences[imageIndex]);
}

void RenderImpl::RenderPresent(uint32_t imageIndex) {
    auto&              presentQueue = utils::GetRHIPresentQueue();
    vk::PresentInfoKHR presentInfo;
    presentInfo.setImageIndices(imageIndex)
        .setSwapchains(m_swapchain.swapchain)
        .setWaitSemaphores(m_imageFinished[m_frameIndex]);

    if (presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
        WIND_CORE_ERROR("image present failed");
    }
}

Renderer::Renderer(Window& window) : m_impl(std::make_unique<RenderImpl>(window)) {}

Renderer::~Renderer() = default;

void Renderer::DrawFrame() { m_impl->DrawFrame(); }

} // namespace wind