#include "Node.h"

#include "Runtime/Render/RHI/Backend.h"

#include "Runtime/Render/RenderGraph/RenderGraphBuilder.h"

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
                                      vk::ImageLayout initialLayout, vk::ImageLayout finalLayout) {
    vk::AttachmentDescription colorAttachment{};
    auto                      format = RenderBackend::GetInstance().GetSwapChainSurfaceFormat();

    colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setFormat(format)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setSamples(vk::SampleCountFlagBits::e1);

    vk::ClearValue temp;
    vk::ClearColorValue color;
    color.setFloat32(std::array<float, 4>{0.1f, 0.1f, 0.1f, 0.1f});
    temp.setColor(color);
    colorClearValue.push_back(temp);
    colorAttachmentDescriptions.push_back(colorAttachment);
    colorTextureDescs[name] = textureDesc;
    outputResources.push_back(name);
}

void PassNode::DeclareDepthAttachment(const std::string& name, const TextureDesc& textureDesc,
                                      vk::ImageLayout initialLayout, vk::ImageLayout finalLayout) {

    depthAttachmentDescription.setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setFormat(textureDesc.format)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setSamples(vk::SampleCountFlagBits::e1);
    vk::ClearValue temp;
    depthClearValue.setDepthStencil({1.0f, 0});
    
    depthTextureDesc[name] = textureDesc;
    outputResources.push_back(name);
}

void PassNode::ConstructResource(RenderGraphBuilder& graphBuilder) {
    for (const auto& [name, desc] : colorTextureDescs) {
        colorAttachments.push_back(graphBuilder.CreateRDGTexture(name, desc));
    }

    const auto [depthTextureName, desc] = *depthTextureDesc.begin();
    depthAttachment = graphBuilder.CreateRDGTexture(depthTextureName, desc);
    
    CreateFrameBuffer(renderRect.width, renderRect.height);
}

void PassNode::CreateFrameBuffer(uint32_t width, uint32_t height) {
    auto&                      device = RenderBackend::GetInstance().GetDevice();
    vk::FramebufferCreateInfo  frameBufferCreateInfo;
    std::vector<vk::ImageView> views;

    for (const auto colorAttachment : colorAttachments) {
        views.push_back(colorAttachment->GetNativeView(ImageView::NATIVE));
    }

    views.push_back(depthAttachment->GetNativeView(ImageView::NATIVE));

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

    std::vector<vk::AttachmentReference> colorAttachmentReferences;
    vk::AttachmentReference              depthAttachmentRenference;

    for (size_t i = 0; i < colorAttachmentDescriptions.size(); ++i) {
        vk::AttachmentReference tempColorAttachmentReference;
        tempColorAttachmentReference.setAttachment(i).setLayout(
            vk::ImageLayout::eColorAttachmentOptimal);
        colorAttachmentReferences.emplace_back(tempColorAttachmentReference);
    }

    depthAttachmentRenference.setAttachment(colorAttachmentDescriptions.size())
        .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpassDescription;
    subpassDescription.setColorAttachmentCount(colorAttachmentDescriptions.size())
        .setColorAttachments(colorAttachmentReferences)
        .setPDepthStencilAttachment(&depthAttachmentRenference)
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    std::pmr::vector<vk::AttachmentDescription> attachments = colorAttachmentDescriptions;
    attachments.emplace_back(depthAttachmentDescription);

    vk::RenderPassCreateInfo renderPassCreateInfo;
    renderPassCreateInfo.setSubpassCount(1)
        .setSubpasses(subpassDescription)
        .setAttachments(attachments)
        .setAttachmentCount(attachments.size());

    renderPass = device.createRenderPass(renderPassCreateInfo);
}

} // namespace wind