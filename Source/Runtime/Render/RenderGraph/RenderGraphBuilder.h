#pragma once

#include "Runtime/Render/RHI/Vma.h"
#include "Runtime/Render/RenderGraph/RenderGraph.h"
#include <cstdint>
#include <memory>

namespace wind {

struct RDGBufferDesc {

};

struct RDGTextureDesc {
    uint32_t width;
    uint32_t height;
    MemoryUsage memoryUsage;
};

// create in every frame
class RenderGraphBuilder {
public:
    void AddRenderPass(std::string_view passName, PassSetupFunc setupFunc);
    
    void CreateRDGTexture(const RDGTextureDesc& texutreDesc);
    void CreateRDGBuffer(const RDGBufferDesc& bufferDesc);
    
private:
    RenderGraph* m_renderGraph;
};
} // namespace wind