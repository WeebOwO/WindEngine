#pragma once

#include <cstdint>
#include <memory>

#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Render/RenderGraph/RenderGraph.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"
#include "Runtime/Scene/Scene.h"

namespace wind {

// create in every frame
class RenderGraphBuilder {
public:
    RenderGraphBuilder(RenderGraph* renderGraph) : m_renderGraph(renderGraph) {}
    
    void AddRenderPass(std::string_view passName, PassSetupFunc setupFunc);
    void SetBackBufferName(std::string_view backBufferName);
    void Setup(SceneView* renderScene);
    void Compile();
    void Exec();

    std::shared_ptr<Image> CreateRDGTexture(const std::string& resourceName, const TextureDesc& textureDesc);
    std::shared_ptr<Buffer> CreateRDGBuffer(const std::string& resourceName, const BufferDesc& bufferDesc);

    std::shared_ptr<RDGRenderTarget> CreateRDGRenderTarget(const std::string& name, uint32_t width, uint32_t height);
private:
    RenderGraph* m_renderGraph;
};
} // namespace wind