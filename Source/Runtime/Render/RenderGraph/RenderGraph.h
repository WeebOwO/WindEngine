#pragma once

#include <string_view>
#include <vector>

#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Render/RenderGraph/RenderGraphRegister.h"

namespace wind {

class RenderGraph {
public:
    struct BufferCreateInfo {

    };

    struct ImageCreateInfo {
        uint32_t width, height;

    };

    void    AddRenderPass(std::string_view passName, PassSetupFunc setupFunc);
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