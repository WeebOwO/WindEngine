#include "RenderGraph.h"
#include "Runtime/Render/RenderGraph/Node.h"

namespace wind {
void RenderGraph::AddResourceNode(const std::string& name, ResourceNode resource) {
    m_resourceNodes.push_back(resource);
    m_graphRegister.RegisterResource(name, &resource);
}

void RenderGraph::AddRenderPass(std::string_view passName, PassSetupFunc setupFunc) {
    PassNode passNode;
    passNode.passName     = passName;
    passNode.passCallback = setupFunc(passNode);
    m_passNodes.push_back(passNode);
    return;
}

void RenderGraph::Exec() {

}

} // namespace wind