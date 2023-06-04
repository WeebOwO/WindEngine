#pragma once

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"

#include "Runtime/Render/RenderGraph/RenderGraph.h"

namespace wind {
class Renderer {
public:
    PERMIT_COPY(Renderer)
    PERMIT_MOVE(Renderer)
    Renderer();
    void CreateRenderResouece();
    void CreateRenderPass();
    void Render();
private:
    RenderBackend& m_backend;
    std::unique_ptr<RenderGraph> m_renderGraph;
};
} // namespace wind