#include "RenderGraph.h"

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {
RenderGraph::~RenderGraph() { auto& device = RenderBackend::GetInstance().GetDevice(); }

void RenderGraph::AddResourceNode(const std::string& name, std::shared_ptr<ResourceNode> resource) {
    m_resourceNodes.push_back(resource);
    m_graphRegister.RegisterResource(name, resource.get());
}

void RenderGraph::AddRenderPass(std::string_view passName, PassSetupFunc setupFunc) {
    auto passNode          = std::make_shared<PassNode>();
    passNode->passName     = passName;
    passNode->passCallback = setupFunc(passNode.get());
    m_passNodes.push_back(passNode);
    return;
}

void RenderGraph::SetBackBufferName(std::string_view name) { m_backBufferName = name; }

void RenderGraph::Setup(SceneView* sceneView) {
    for (auto passNode : m_passNodes) {
        passNode->renderScene = sceneView;
    }
}

void RenderGraph::Exec() {
    auto&    backend           = RenderBackend::GetInstance();
    auto&    frame             = backend.GetCurrentFrame();
    uint32_t presentImageIndex = backend.GetCurrentImageIndex();

    auto frameCommandBuffer = frame.Commands;
    for (auto passNode : m_passNodes) {
        frameCommandBuffer.BeginRenderPass(passNode.get());
        frameCommandBuffer.BindPipeline(passNode.get());
        passNode->passCallback(frameCommandBuffer, &m_graphRegister);
        frameCommandBuffer.EndRenderPass();
    }
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