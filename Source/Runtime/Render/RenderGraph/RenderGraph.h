#pragma once

#include <string_view>
#include <vector>

#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Render/RenderGraph/RenderGraphRegister.h"

namespace wind {
class PassNode;
class ResourceNode;

class RenderGraph {
public:
    friend class RenderGraphBuilder;
    ~RenderGraph();

    void Setup(SceneView* sceneView);
    void Exec();

private:
    bool Contains(const std::string& resourceName);
    std::shared_ptr<Image> GetImageResourceByName(const std::string& name);
    std::shared_ptr<Buffer> GetBufferResourceByName(const std::string& name);

    void AddRenderPass(std::string_view passName, PassSetupFunc setupFunc);
    void AddResourceNode(const std::string& name, ResourceNode* resource);
    void SetBackBufferName(std::string_view name);
    
private:
    std::string m_backBufferName;
    std::vector<PassNode*>     m_passNodes;
    std::vector<ResourceNode*> m_resourceNodes;

    RenderGraphRegister        m_graphRegister;
};

} // namespace wind