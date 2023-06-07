#include "RenderGraphBuilder.h"

namespace wind {
    void RenderGraphBuilder::AddRenderPass(std::string_view passName, PassSetupFunc setupFunc) {
        m_renderGraph->AddRenderPass(passName, setupFunc);
    }
}