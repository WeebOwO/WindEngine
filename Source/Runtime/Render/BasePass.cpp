#include "BasePass.h"

#include <array>
#include <stdint.h>

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Shader.h"
#include "Runtime/Render/RHI/Pipeline.h"

#include "Runtime/Scene/Scene.h"

namespace wind {
void BasePass::Setup() {
    auto& device = renderBackend.GetDevice();
    // 1. CreateRenderPass
    vk::RenderPassCreateInfo  renderPasscreateInfo;
    vk::AttachmentDescription colorAttachment{};

    colorAttachment.setFormat(renderBackend.GetSwapChainSurfaceFormat())
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setSamples(vk::SampleCountFlagBits::e1);

    vk::AttachmentReference colorRef;
    colorRef.setAttachment(0).setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpassDescription;
    subpassDescription.setColorAttachmentCount(1)
        .setColorAttachments(colorRef)
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    renderPasscreateInfo.setAttachments(colorAttachment)
        .setAttachmentCount(1)
        .setSubpassCount(1)
        .setSubpasses(subpassDescription);

    passNode.renderPassHandle = device.createRenderPass(renderPasscreateInfo);
    // 2. Set render area
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();
    passNode.renderArea.setExtent({width, height}).setOffset({0, 0});

    // 2. Create FrameBuffer
    vk::FramebufferCreateInfo  framebufferCreateInfo;
    std::vector<vk::ImageView> attachments{};

    framebufferCreateInfo.setAttachmentCount(attachments.size())
        .setLayers(1)
        .setRenderPass(passNode.renderPassHandle)
        .setWidth(width)
        .setHeight(height)
        .setAttachments(attachments);

    uint32_t presentCnts = renderBackend.GetPresentImageCnt();
    resourceNode.framebuffers.resize(presentCnts);
    for (size_t i = 0; i < presentCnts; ++i) {
        std::vector<vk::ImageView> attachments;
        for (auto& colorAttachment : resourceNode.colorAttachments) {
            attachments.push_back(colorAttachment.GetNativeView(ImageView::NATIVE));
        }
        for (auto& depthAttachments : resourceNode.depthAttachments) {
            attachments.push_back(depthAttachments.GetNativeView(ImageView::DEPTH_ONLY));
        }
        attachments.push_back(renderBackend.AcquireSwapchainImage(i, ImageUsage::COLOR_ATTACHMENT)
                                  .GetNativeView(ImageView::NATIVE));

        vk::FramebufferCreateInfo createInfo;
        createInfo.setAttachmentCount(attachments.size())
            .setAttachments(attachments)
            .setWidth(width)
            .setHeight(height)
            .setLayers(1)
            .setRenderPass(passNode.renderPassHandle);
        resourceNode.framebuffers[i] = device.createFramebuffer(createInfo);
    }

    // 3. Set render pipeline
    GraphicsShader shader = GraphicsShader("triangle.vert.spv", "triangle.frag.spv",
                                           Vertex::GetInputBindingDescription(),
                                           Vertex::GetVertexInputAttributeDescriptions());
    vk::PipelineLayoutCreateInfo pipelineLayoutCraeteInfo;
    
    passNode.pipelineLayout = device.createPipelineLayout(pipelineLayoutCraeteInfo);
    passNode.pipelineType = vk::PipelineBindPoint::eGraphics;
    passNode.pipeline = CreateGraphicsPipeline(shader, passNode.renderPassHandle, passNode.pipelineLayout);

    // 4. Set clear value
    vk::ClearValue clearColor;
    clearColor.setColor(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f});
    passNode.clearValues = clearColor;

    WIND_CORE_INFO("{} is setup", Name());
}

void BasePass::Exec() {
    auto& backend      = RenderBackend::GetInstance();
    auto& currentFrame = backend.GetCurrentFrame();
    auto& gameObjects  = Scene::GetWorld().GetWorldGameObjects();

    resourceNode.presentImageIndex = backend.GetCurrentImageIndex();
    auto cmdBuffer  = currentFrame.Commands;

    vk::RenderPassBeginInfo renderPassBeginInfo;

    cmdBuffer.BeginRenderPass(passNode, resourceNode);
    cmdBuffer.BindPipeline(passNode);
    
    // set draw call
    for (const auto& gameObject : gameObjects) {
        gameObject.model->Bind(cmdBuffer);
        gameObject.model->Draw(cmdBuffer);
    }

    cmdBuffer.EndRenderPass();
}
} // namespace wind