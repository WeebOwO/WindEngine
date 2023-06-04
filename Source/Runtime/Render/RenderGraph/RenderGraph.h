#pragma once

#include <string_view>
#include <vector>

#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Render/RenderGraph/RenderGraphRegister.h"

namespace wind {

class RenderGraph {
public:
    struct BufferCreateInfo {

    };

    struct ImageCreateInfo {

    };

    void    AddRenderPass(std::string_view passName, PassSetupFunc setupFunc);
    Image*  RegisterAndCreateImage(const std::string& passName, const std::string& resourceName,
                                   DataRelation relation, const BufferCreateInfo& createInfo);
    Buffer* RegisterAndCreateBuffer(const std::string& passName, const std::string& resourceName,
                                    DataRelation relation, const ImageCreateInfo& createInfo);
    void    ImportPersistentResource();

    void Setup();
    void Compile();
    void Exec();
private:
    std::vector<PassNode>     m_passNodes;
    std::vector<ResourceNode> m_resourceNodes;
    RenderGraphRegister       m_graphRegister;
};

} // namespace wind