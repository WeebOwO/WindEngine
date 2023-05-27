#include "Frame.h"

#include "Runtime/Render/Rhi/Backend.h"

namespace wind {
void VirtualFrameProvider::Init(size_t frameCount, size_t stageBufferSize) {
    auto& vulkanContext = RenderBackend::GetInstance();
    m_virtualFrames.reserve(frameCount);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.setCommandPool(vulkanContext.GetCommandPool())
        .setCommandBufferCount((uint32_t)frameCount)
        .setLevel(vk::CommandBufferLevel::ePrimary);

    auto commandBuffers =
        vulkanContext.GetDevice().allocateCommandBuffers(commandBufferAllocateInfo);

    for (size_t i = 0; i < frameCount; i++) {
        auto fence = vulkanContext.GetDevice().createFence(
            vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});

        m_virtualFrames.push_back(VirtualFrame{
            CommandBuffer{commandBuffers[i]},
            fence,
        });
    }
}

void VirtualFrameProvider::Destroy() {
    auto& vulkanContext = RenderBackend::GetInstance();
    for (const auto& virtualFrame : m_virtualFrames) {
        if ((bool)virtualFrame.CommandQueueFence)
            vulkanContext.GetDevice().destroyFence(virtualFrame.CommandQueueFence);
    }
    m_virtualFrames.clear();
}

void VirtualFrameProvider::StartFrame() {
    auto&      frame         = GetCurrentFrame();
    auto&      vulkanContext = RenderBackend::GetInstance();
    vk::Result waitFenceResult =
        vulkanContext.GetDevice().waitForFences(frame.CommandQueueFence, false, UINT64_MAX);
    assert(waitFenceResult == vk::Result::eSuccess);
    vulkanContext.GetDevice().resetFences(frame.CommandQueueFence);

    auto acquireNextImage = vulkanContext.GetDevice().acquireNextImageKHR(
        vulkanContext.GetSwapchain(), UINT64_MAX, vulkanContext.GetImageAvailableSemaphore());
    assert(acquireNextImage.result == vk::Result::eSuccess ||
           acquireNextImage.result == vk::Result::eSuboptimalKHR);
    m_presentImageIndex = acquireNextImage.value;

    frame.Commands.Begin();
}

void VirtualFrameProvider::EndFrame() {
    auto& frame         = GetCurrentFrame();
    auto& vulkanContext = RenderBackend::GetInstance();

    frame.Commands.End();

    std::array waitDstStageMask = {
        (vk::PipelineStageFlags)vk::PipelineStageFlagBits::eColorAttachmentOutput};

    vk::SubmitInfo submitInfo;
    submitInfo.setWaitSemaphores(vulkanContext.GetImageAvailableSemaphore())
        .setWaitDstStageMask(waitDstStageMask)
        .setSignalSemaphores(vulkanContext.GetRenderingFinishedSemaphore())
        .setCommandBuffers(frame.Commands.GetNativeHandle());

    RenderBackend::GetInstance().GetGraphicsQueue().submit(std::array{submitInfo},
                                                           frame.CommandQueueFence);
    vk::PresentInfoKHR presentInfo;
    presentInfo.setWaitSemaphores(vulkanContext.GetRenderingFinishedSemaphore())
        .setSwapchains(vulkanContext.GetSwapchain())
        .setImageIndices(m_presentImageIndex);

    auto presetSucceeded = vulkanContext.GetPresentQueue().presentKHR(presentInfo);
    assert(presetSucceeded == vk::Result::eSuccess);

    m_currentFrame = (m_currentFrame + 1) % m_virtualFrames.size();
}

VirtualFrame& VirtualFrameProvider::GetCurrentFrame() { return m_virtualFrames[m_currentFrame]; }

VirtualFrame& VirtualFrameProvider::GetNextFrame() {
    return m_virtualFrames[(m_currentFrame + 1) % m_virtualFrames.size()];
}

const VirtualFrame& VirtualFrameProvider::GetCurrentFrame() const {
    return m_virtualFrames[m_currentFrame];
}

const VirtualFrame& VirtualFrameProvider::GetNextFrame() const {
    return m_virtualFrames[(m_currentFrame + 1) % m_virtualFrames.size()];
}

size_t VirtualFrameProvider::GetFrameCount() const { return m_virtualFrames.size(); }

uint32_t VirtualFrameProvider::GetPresentImageIndex() const { return m_presentImageIndex; }
} // namespace wind