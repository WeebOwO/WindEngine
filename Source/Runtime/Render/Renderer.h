#pragma once

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"

#include "Runtime/Render/RenderGraph/RenderGraph.h"
#include "Runtime/Render/RenderGraph/RenderGraphBuilder.h"
#include "Runtime/Render/RenderGraph/RenderPass.h"

namespace wind {

enum class MeshPassType {
    BasePass = 0
};

enum class ShadingPath  {
    Forward, 
    Deferred
};

class Scene;

class Renderer {
public:
    PERMIT_COPY(Renderer)
    PERMIT_MOVE(Renderer)
    Renderer();
    void CreateRenderPass();

    void Render(Scene& scene);
private:
    void InitForwardPassResource();
    void RenderForward(RenderGraphBuilder& graphBuilder);
    void RenderDefered(RenderGraphBuilder& graphBuilder);

    ShadingPath m_shadingPath {ShadingPath::Forward};
    RenderBackend& m_backend;

    std::shared_ptr<RenderGraph> m_renderGraph;
};
} // namespace wind