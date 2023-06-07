#pragma once

#include <string_view>
#include <vector>

#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Render/RenderGraph/RenderGraphRegister.h"

namespace wind {

class RenderGraph {
public:
    friend class RenderGraphBuilder;

    void Setup();
    void Compile();
    void Exec();

private:
    void AddRenderPass(std::string_view passName, PassSetupFunc setupFunc);
private:
    std::vector<PassNode>     m_passNodes;
    std::vector<ResourceNode> m_resourceNodes;
    RenderGraphRegister       m_graphRegister;
};

} // namespace wind