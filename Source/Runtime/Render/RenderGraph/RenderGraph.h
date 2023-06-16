#pragma once

#include <string_view>
#include <vector>

#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Render/RenderGraph/RenderGraphRegister.h"

namespace wind {
class PassNode;
class ResourceNode;

class RenderGraph {
public:
    friend class RenderGraphBuilder;
    ~RenderGraph();

    void Setup();
    void Exec();

private:
    void AddRenderPass(std::string_view passName, PassSetupFunc setupFunc);
    void AddResourceNode(const std::string& name, ResourceNode* resource);
    
private:
    std::vector<PassNode*>     m_passNodes;
    std::vector<ResourceNode*> m_resourceNodes;
    RenderGraphRegister        m_graphRegister;
};

} // namespace wind