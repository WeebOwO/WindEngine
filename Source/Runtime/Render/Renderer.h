#pragma once

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RenderGraph.h"

namespace wind {
class Renderer {
public:
    Renderer();
    void InitRenderPass();
    PERMIT_COPY(Renderer)
    PERMIT_MOVE(Renderer)
    void Render();
private:
    RenderBackend& m_backend;
    RenderGraph m_renderGraph;
};
} // namespace wind