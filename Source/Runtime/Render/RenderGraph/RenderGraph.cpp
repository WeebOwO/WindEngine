#include "RenderGraph.h"

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/CommandBuffer.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RenderGraph/Node.h"

namespace wind {
RenderGraph::~RenderGraph() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    device.waitIdle();
    for (auto passNode : m_passNodes) {
        delete passNode;
    }
    for(auto resourceNode : m_resourceNodes) {
        delete resourceNode;
    }
}

void RenderGraph::AddResourceNode(const std::string& name, ResourceNode* resource) {
    m_resourceNodes.push_back(resource);
    m_graphRegister.RegisterResource(name, resource);
}

void RenderGraph::AddRenderPass(std::string_view passName, PassSetupFunc setupFunc) {
    auto* passNode     = new PassNode();
    passNode->passName     = passName;
    passNode->passCallback = setupFunc(passNode);
    m_passNodes.push_back(passNode);
    return;
}

void RenderGraph::Exec() {
    auto&    backend            = RenderBackend::GetInstance();
    auto&    frame              = backend.GetCurrentFrame();
    uint32_t presentImageIndex  = backend.GetCurrentImageIndex();

    auto     frameCommandBuffer = frame.Commands;
    for (auto* passNode : m_passNodes) {
        frameCommandBuffer.BeginRenderPass(passNode);
        frameCommandBuffer.BindPipeline(passNode);
        passNode->passCallback(frameCommandBuffer, &m_graphRegister);
        frameCommandBuffer.EndRenderPass();
    }
    ResourceNode* output = m_graphRegister.GetResource("BackBuffer");
    auto& presentImage =
        RenderBackend::GetInstance().AcquireSwapchainImage(presentImageIndex, ImageUsage::TRANSFER_DESTINATION);

    ImageInfo sourceImageInfo {*output->imageHandle, ImageUsage::COLOR_ATTACHMENT};
    ImageInfo dstImageInfo {presentImage, ImageUsage::UNKNOWN};
    frameCommandBuffer.CopyImage(sourceImageInfo, dstImageInfo);
}
} // namespace wind