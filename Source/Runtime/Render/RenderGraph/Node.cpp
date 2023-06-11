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

void PassNode::DeclareColorAttachment(const std::string& name, RenderGraphBuilder& graphBuilder,
                                      TextureDesc textureDesc, vk::ImageLayout initialLayout,
                                      vk::ImageLayout finalLayout) {
    vk::AttachmentDescription colorAttachment{};
    std::shared_ptr<Image>    colorImage = graphBuilder.CreateRDGTexture(name, textureDesc);
    auto                      format     = RenderBackend::GetInstance().GetSwapChainSurfaceFormat();

    colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setFormat(format)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setSamples(vk::SampleCountFlagBits::e1);

    colorAttachmentDescriptions.push_back(colorAttachment);
    colorAttachments.push_back(colorImage);

    outputResources.push_back(name);
}

void PassNode::DeclareDepthAttachment(const std::string& name, RenderGraphBuilder& graphBuilder,
                                      TextureDesc textureDesc, vk::ImageLayout initialLayout,
                                      vk::ImageLayout finalLayout) {
    auto depthImage = graphBuilder.CreateRDGTexture(name, textureDesc);
    depthAttachmentDescription.setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthAttachmentOptimal)
        .setFormat(vk::Format::eD32Sfloat)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setSamples(vk::SampleCountFlagBits::e1);

    depthAttachment = depthImage;
    outputResources.push_back(name);
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
        .setLayout(vk::ImageLayout::eDepthAttachmentOptimal);

    vk::SubpassDescription subpassDescription;
    subpassDescription.setColorAttachmentCount(colorAttachmentDescriptions.size())
        .setColorAttachments(colorAttachmentReferences)
        .setPDepthStencilAttachment(&depthAttachmentRenference)
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    std::vector<vk::AttachmentDescription> attachments = colorAttachmentDescriptions;
    attachments.emplace_back(depthAttachmentDescription);

    vk::RenderPassCreateInfo renderPassCreateInfo;
    renderPassCreateInfo.setSubpassCount(1)
        .setSubpasses(subpassDescription)
        .setAttachments(attachments)
        .setAttachmentCount(attachments.size());

    renderPass = device.createRenderPass(renderPassCreateInfo);
}

} // namespace wind