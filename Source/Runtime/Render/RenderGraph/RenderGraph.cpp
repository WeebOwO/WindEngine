#include "RenderGraph.h"

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {
RenderGraph::~RenderGraph() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    device.waitIdle();
    for (auto passNode : m_passNodes) {
        delete passNode;
    }
    for (auto resourceNode : m_resourceNodes) {
        delete resourceNode;
    }
}

void RenderGraph::AddResourceNode(const std::string& name, ResourceNode* resource) {
    m_resourceNodes.push_back(resource);
    m_graphRegister.RegisterResource(name, resource);
}

void RenderGraph::AddRenderPass(std::string_view passName, PassSetupFunc setupFunc) {
    auto* passNode         = new PassNode();
    passNode->passName     = passName;
    passNode->passCallback = setupFunc(passNode);
    m_passNodes.push_back(passNode);
    return;
}

void RenderGraph::SetBackBufferName(std::string_view name) { m_backBufferName = name; }

void RenderGraph::Setup(SceneView* sceneView) {
    for (auto* passNode : m_passNodes) {
        passNode->renderScene = sceneView;
    }
}

void RenderGraph::Exec() {
    auto&    backend           = RenderBackend::GetInstance();
    auto&    frame             = backend.GetCurrentFrame();
    uint32_t presentImageIndex = backend.GetCurrentImageIndex();

    auto frameCommandBuffer = frame.Commands;
    for (auto* passNode : m_passNodes) {
        // frameCommandBuffer.BeginRenderPass(passNode);
        // frameCommandBuffer.BindPipeline(passNode);
        passNode->passCallback(frameCommandBuffer, &m_graphRegister);
        // frameCommandBuffer.EndRenderPass();
    }
    ResourceNode* output       = m_graphRegister.GetResource(m_backBufferName);
    auto&         presentImage = RenderBackend::GetInstance().AcquireSwapchainImage(
        presentImageIndex, ImageUsage::TRANSFER_DESTINATION);

    ImageInfo sourceImageInfo{*output->imageHandle, ImageUsage::COLOR_ATTACHMENT};
    ImageInfo dstImageInfo{presentImage, ImageUsage::UNKNOWN};
    frameCommandBuffer.CopyImage(sourceImageInfo, dstImageInfo);
}

std::shared_ptr<Image> RenderGraph::GetImageResourceByName(const std::string& name) {
    return m_graphRegister.GetResource(name)->imageHandle;
}

std::shared_ptr<Buffer> RenderGraph::GetBufferResourceByName(const std::string& name) {
    return m_graphRegister.GetResource(name)->bufferHandle;
}

bool RenderGraph::Contains(const std::string& resourceName) {
    return m_graphRegister.Contains(resourceName);
}
} // namespace wind