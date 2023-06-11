#pragma once

#include <cstdint>
#include <memory>

#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RHI/Vma.h"
#include "Runtime/Render/RenderGraph/RenderGraph.h"

namespace wind {

struct RDGRenderTarget {
    uint32_t width;
    uint32_t height;
    Image colorAttachment;
    Image depthAttachment;
};

struct TextureDesc {
    uint32_t width;
    uint32_t height;
    vk::Format format;
    ImageUsage::Value usage;
    MemoryUsage memoryUsage;
    ImageOptions::Value options;
};

struct BufferDesc {

};
// create in every frame
class RenderGraphBuilder {
public:
    RenderGraphBuilder(RenderGraph* renderGraph) : m_renderGraph(renderGraph) {}
    
    void AddRenderPass(std::string_view passName, PassSetupFunc setupFunc);
    
    std::shared_ptr<Image> CreateRDGTexture(const std::string& resourceName, const TextureDesc& textureDesc);
    std::shared_ptr<Buffer> CreateRDGBuffer(const std::string& resourceName, const BufferDesc& bufferDesc);

    std::shared_ptr<RDGRenderTarget> CreateRDGRenderTarget(const std::string& name, uint32_t width, uint32_t height);
private:
    RenderGraph* m_renderGraph;
};
} // namespace wind