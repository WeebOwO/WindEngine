#include "Node.h"

#include "Runtime/Render/RHI/Backend.h"

#include "Runtime/Render/RenderGraph/RenderGraphBuilder.h"
#include "Runtime/Render/RenderGraph/RenderPass.h"

namespace wind {
void PassNode::Init(const std::string& passName, const std::vector<std::string>& inRoureces,
                    const std::vector<std::string>& outputResources) {
    // add this to mark different
    this->passName = passName;
    if (!inRoureces.empty()) {
        inRefCnt            = inRoureces.size();
        dependencyResources = {inRoureces.begin(), inRoureces.end()};
    }
    if (!outputResources.empty()) {
        outRefcnt             = outputResources.size();
        this->outputResources = {outputResources.begin(), outputResources.end()};
    }
}

PassNode::~PassNode() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    device.waitIdle();
    device.destroyRenderPass(renderPass);
    device.destroyFramebuffer(frameBuffer);
}

void PassNode::Init(const std::vector<std::string>& inRoureces,
                    const std::vector<std::string>& outputResources) {
    if (!inRoureces.empty()) {
        inRefCnt            = inRoureces.size();
        dependencyResources = {inRoureces.begin(), inRoureces.end()};
    }
    if (!outputResources.empty()) {
        outRefcnt             = outputResources.size();
        this->outputResources = {outputResources.begin(), outputResources.end()};
    }
}

void PassNode::DeclareColorAttachment(const std::string& name, const TextureDesc& textureDesc,
                                      TextureOps ops, vk::ImageLayout initialLayout,
                                      vk::ImageLayout finalLayout, ClearColor clearColor) {
    vk::AttachmentDescription colorAttachment{};
    auto                      format = textureDesc.format;

    colorAttachment.setInitialLayout(initialLayout)
        .setFinalLayout(finalLayout)
        .setFormat(format)
        .setLoadOp(ops.load)
        .setStoreOp(ops.store)
        .setStencilLoadOp(ops.stencilLoad)
        .setStencilStoreOp(ops.stencilStore)
        .setSamples(textureDesc.sampleCount);

    vk::ClearValue      temp;
    vk::ClearColorValue color;
    color.setFloat32(std::array<float, 4>{clearColor.r, clearColor.g, clearColor.b, clearColor.a});
    temp.setColor(color);
    colorClearValue.push_back(temp);
    colorAttachmentDescriptions.push_back(colorAttachment);
    colorTextureDescs[name] = textureDesc;
    outputResources.push_back(name);
}

void PassNode::DeclareDepthAttachment(const std::string& name, const TextureDesc& textureDesc,
                                      TextureOps loadops, vk::ImageLayout initialLayout,
                                      vk::ImageLayout   finalLayout,
                                      ClearDepthStencil clearDepthStencil) {

    depthAttachmentDescription.setInitialLayout(initialLayout)
        .setFinalLayout(finalLayout)
        .setFormat(textureDesc.format)
        .setLoadOp(loadops.load)
        .setStoreOp(loadops.store)
        .setStencilLoadOp(loadops.stencilLoad)
        .setStencilStoreOp(loadops.stencilStore)
        .setSamples(vk::SampleCountFlagBits::e1);
    depthClearValue.setDepthStencil({clearDepthStencil.depth, clearDepthStencil.stencil});

    depthTextureDesc[name] = textureDesc;
    outputResources.push_back(name);
}

void PassNode::ConstructResource(RenderGraphBuilder& graphBuilder) {
    for (const auto& [name, desc] : colorTextureDescs) {
        colorAttachments.push_back(graphBuilder.TryCreateRDGTexture(name, desc));
    }

    if (isWriteToDepth) {
        const auto [depthTextureName, desc] = *depthTextureDesc.begin();
        depthAttachment = graphBuilder.TryCreateRDGTexture(depthTextureName, desc);
    }

    CreateFrameBuffer(renderRect.width, renderRect.height);
}

void PassNode::CreateFrameBuffer(uint32_t width, uint32_t height) {
    auto&                      device = RenderBackend::GetInstance().GetDevice();
    vk::FramebufferCreateInfo  frameBufferCreateInfo;
    std::vector<vk::ImageView> views;

    for (const auto colorAttachment : colorAttachments) {
        views.push_back(colorAttachment->GetNativeView(ImageView::NATIVE));
    }

    if (isWriteToDepth) { 
        if(colorAttachmentDescriptions.empty()) {
            views.push_back(depthAttachment->GetNativeView(ImageView::DEPTH_ONLY)); 
        } else {
            views.push_back(depthAttachment->GetNativeView(ImageView::NATIVE));
        }
    }

    frameBufferCreateInfo.setRenderPass(renderPass)
        .setAttachmentCount(views.size())
        .setAttachments(views)
        .setWidth(width)
        .setHeight(height)
        .setLayers(1);

    frameBuffer = device.createFramebuffer(frameBufferCreateInfo);
}

void PassNode::CreateRenderPass() {
    auto& device = RenderBackend::GetInstance().GetDevice();

    std::vector<vk::AttachmentReference>        colorAttachmentReferences;
    vk::AttachmentReference                     depthAttachmentRenference;
    vk::SubpassDescription                      subpassDescription;
    std::pmr::vector<vk::AttachmentDescription> attachments = colorAttachmentDescriptions;

    for (size_t i = 0; i < colorAttachmentDescriptions.size(); ++i) {
        vk::AttachmentReference tempColorAttachmentReference;
        tempColorAttachmentReference.setAttachment(i).setLayout(
            vk::ImageLayout::eColorAttachmentOptimal);
        colorAttachmentReferences.emplace_back(tempColorAttachmentReference);
    }

    if (isWriteToDepth) {
        depthAttachmentRenference.setAttachment(colorAttachmentDescriptions.size())
            .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        subpassDescription.setColorAttachmentCount(colorAttachmentDescriptions.size())
            .setColorAttachments(colorAttachmentReferences)
            .setPDepthStencilAttachment(&depthAttachmentRenference)
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
        attachments.emplace_back(depthAttachmentDescription);
    } else {
        subpassDescription.setColorAttachments(colorAttachmentReferences)
            .setColorAttachmentCount(colorAttachmentDescriptions.size());
    }
    vk::RenderPassCreateInfo renderPassCreateInfo;
    renderPassCreateInfo.setSubpassCount(1)
        .setSubpasses(subpassDescription)
        .setAttachments(attachments)
        .setAttachmentCount(attachments.size());

    renderPass = device.createRenderPass(renderPassCreateInfo);
}

} // namespace wind