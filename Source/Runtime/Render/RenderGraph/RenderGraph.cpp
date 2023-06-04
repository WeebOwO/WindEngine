#include "RenderGraph.h"

namespace wind {
    void RenderGraph::AddRenderPass(std::string_view passName, PassSetupFunc setupFunc) {
        m_passNodes.emplace_back(std::string(passName), setupFunc());
    }
}